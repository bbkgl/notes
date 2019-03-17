# shutdown()函数和socket实现文件传输

## shutdown()函数：优雅地断开连接

调用 close()/close[socket](http://c.biancheng.net/socket/)() 函数意味着完全断开连接，即不能发送数据也不能接收数据，这种“生硬”的方式有时候会显得不太“优雅”。

![close(assets/1350141P8-0.jpg)/closesocket() 断开连接](http://c.biancheng.net/uploads/allimg/190219/1350141P8-0.jpg)

上图演示了两台正在进行双向通信的主机。主机A发送完数据后，单方面调用 close()/closesocket() 断开连接，之后主机A、B都不能再接受对方传输的数据。实际上，是完全无法调用与数据收发有关的函数。

一般情况下这不会有问题，但有些特殊时刻，需要只断开一条数据传输通道，而保留另一条。

使用 shutdown() 函数可以达到这个目的，它的原型为：

```cpp
int shutdown(int sock, int howto);  //Linux
int shutdown(SOCKET s, int howto);  //Windows
```

sock 为需要断开的套接字，howto 为断开方式。

howto 在 Linux 下有以下取值：

-  SHUT_RD：断开输入流。套接字无法接收数据（即使输入缓冲区收到数据也被抹去），无法调用输入相关函数。
-  SHUT_WR：断开输出流。套接字无法发送数据，但如果输出缓冲区中还有未传输的数据，则将传递到目标主机。
-  SHUT_RDWR：同时断开 I/O 流。相当于分两次调用 shutdown()，其中一次以 SHUT_RD 为参数，另一次以 SHUT_WR 为参数。

**close()/closesocket()和shutdown()的区别**

确切地说，close() / closesocket() 用来关闭套接字，将套接字描述符（或句柄）从内存清除，之后再也不能使用该套接字，与C语言中的 fclose() 类似。应用程序关闭套接字后，与该套接字相关的连接和缓存也失去了意义，TCP协议会自动触发关闭连接的操作。

shutdown() 用来关闭连接，而不是套接字，不管调用多少次 shutdown()，套接字依然存在，直到调用 close() / closesocket() 将套接字从内存清除。

调用 close()/closesocket() 关闭套接字时，或调用 shutdown() 关闭输出流时，都会向对方发送 FIN 包。FIN 包表示数据传输完毕，计算机收到 FIN 包就知道不会再有数据传送过来了。

默认情况下，close()/closesocket() 会立即向网络中发送FIN包，不管输出缓冲区中是否还有数据，而shutdown() 会等输出缓冲区中的数据传输完毕再发送FIN包。也就意味着，调用close()/closesocket() 将丢失输出缓冲区中的数据，而调用 shutdown() 不会。

## socket编程实现文件传输功能

> **实现的功能：client从server上下载一个文件到本地。**

编写这个程序注意两个问题：

1. 文件大小不确定，有可能比缓冲区大得多，调用一次write()/send()函数不能完成文件内容的发送；

   要解决这个问题，可以使用while循环，例如：

   ```cpp
   //Server 代码
   int nCount;
   while( (nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0 ){
       send(sock, buffer, nCount, 0);
   }
   //Client 代码
   int nCount;
   while( (nCount = recv(clntSock, buffer, BUF_SIZE, 0)) > 0 ){
       fwrite(buffer, nCount, 1, fp);
   }
   ```

   - 对于Server端的代码，当读取到文件末尾，fread()会返回0，结束循环；
   - 对于 Client 端代码，有一个关键的问题，就是文件传输完毕后让 recv() 返回 0，结束 while 循环；
   - **注意：读取完缓冲区中的数据 recv() 并不会返回 0，而是被阻塞，直到缓冲区中再次有数据。**

2. Client 端如何判断文件接收完毕，也就是上面提到的问题——何时结束 while 循环。

   最简单的结束 while 循环的方法当然是文件接收完毕后让 recv() 函数返回 0，那么，如何让 recv() 返回 0 呢？recv() 返回 0 的唯一时机就是收到FIN包时。

   FIN 包表示数据传输完毕，计算机收到 FIN 包后就知道对方不会再向自己传输数据，当调用 read()/recv() 函数时，如果缓冲区中没有数据，就会返回 0，表示读到了”socket文件的末尾“。

   这里我们调用 shutdown() 来发送FIN包：server 端直接调用 close()/closesocket() 会使输出缓冲区中的数据失效，文件内容很有可能没有传输完毕连接就断开了，而调用 shutdown() 会等待输出缓冲区中的数据传输完毕。


服务器端：

```cpp
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 文件路径
    char *filename = "qq.png";
    FILE *fp = fopen(filename, "rb");

    // 套接字绑定
    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(serv_sock, (sockaddr *)&serv_addr, sizeof(sockaddr));

    // 进入监听
    listen(serv_sock, 20);

    // 接收客户端连接请求
    sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(sockaddr);
    bzero(&clnt_addr, sizeof(sockaddr));
    int clnt_sock = accept(serv_sock, (sockaddr *)&clnt_addr, &clnt_addr_len);

    // 定义缓冲区大小
    const unsigned BUFF_SIZE = 200;

    char buff[BUFF_SIZE];
    int n_cnt;

    // fread()函数读取文件完成后，会返回0
    while ((n_cnt = fread(buff, 1, BUFF_SIZE, fp)) > 0) {
        send(clnt_sock, buff, n_cnt, 0);
    }

    // SHUT_WR参数让服务端切断输出流，并让客户端的recv()函数返回一个0
    shutdown(clnt_sock, SHUT_WR);

    fclose(fp);
    close(clnt_sock);
    close(serv_sock);

    return 0;
}
```



客户端：

```cpp
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // 创建套接字和文件指针
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    char *filename = "qq.png";
    FILE *fp = fopen(filename, "wb");

    // 套接字参数设定
    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(sockaddr));
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);
    serv_addr.sin_family = AF_INET;
    connect(serv_sock, (sockaddr *)&serv_addr, sizeof(sockaddr));

    // 接收服务端消息和文件
    const unsigned int BUFF_SIZE = 200;            // 定义输入输出缓冲区大小
    char buff[BUFF_SIZE];

    int n_cnt;

    // 接收文件
    while ((n_cnt = recv(serv_sock, buff, BUFF_SIZE, 0)) > 0) {
        fwrite(buff, 1, n_cnt, fp);
    }


    // 关闭套接字
    fclose(fp);
    close(serv_sock);

    return 0;
}
```

总结一下连接-下载过程：

1. 服务器端和客户端建立连接；
2. 服务器端定义输出缓冲区；
3. 服务器端读取文件到缓冲区（如果文件大小大于缓冲区大小，第一次时缓冲区将被塞满，n_cnt变量返回当前读入缓冲区的数据量大小）；
4. 将缓冲区中数据发给客户端；
5. 客户端recv()函数会返回当前收到数据的大小；
6. 如果recv()函数返回值大于0，则继续写入文件-接收文件的循环，直至返回值为零；
7. **如果没有收到数据，输入缓冲区为空，则进入阻塞状态，recv()函数会一直等待，直到缓冲区中有文件，或者服务器端发送FIN数据报，表示发送完毕，recv()函数才会返回0，阻塞状态解除，跳出循环）。**