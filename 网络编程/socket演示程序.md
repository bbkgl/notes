# Linux下的socket演示程序

---

服务端server.cpp：

```cpp
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
    //创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);

    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    //向客户端发送数据
    char str[] = "http://c.biancheng.net/socket/";
    write(clnt_sock, str, sizeof(str));
   
    //关闭套接字
    close(clnt_sock);
    close(serv_sock);

    return 0;
}
```

客户端client.cpp：

```cpp
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(){
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   
    //读取服务器传回的数据
    char buffer[40];
    read(sock, buffer, sizeof(buffer)-1);
   
    printf("Message form server: %s\n", buffer);
   
    //关闭套接字
    close(sock);

    return 0;
}
```

## 源码解析：

### 服务端

1. 第11行通过socket()函数创建了一个套接字，参数AF_INET表示使用IPv4地址，SOCK_STREAM表示使用面向连接的套接字，IPPROTO_TCP表示使用TCP协议。在Linux中，socket也是一种文件，有文件描述符，可以使用write()/read()函数进行I/O操作；
2. 第19行通过bind()函数将套接字serv_sock与特定的IP地址和端口绑定，IP地址和端口都保存在sockaddr_in结构体中。socket()函数确定了套接字的各种属性，bind()函数让套接字与特定的IP地址和端口对应起来，这样客户端才能连接到该套接字；
3. 第22行让套接字处于被动监听状态。所谓被动监听，是指套接字一直处于睡眠中，直到客户端发起请求才被”唤醒“；
4. 第27行的accept()函数用来接收客户端的请求，程序一旦执行到accept()就会被阻塞（暂停运行），直到客户端发起请求；
5. 第31行的write()函数用来向套接字中写入数据，也就是向客户端发送数据；
6. 和普通文件一样，socket使用完毕后也要用close()关闭。

### 客户端

1. 第19行通过connect()函数向服务器发起请求，服务器的IP地址和端口号保存在sockaddr_in结构体中，直到服务器传回数据后，connect()才结束；
2. 第23行代码通过read()从套接字文件中读取数据。



