# IO多路转接–select

## select函数介绍

### 函数原型

```cpp
int select(int nfds, 
           fd_set *readfds, 
           fd_set *writefds, 
           fd_set *exceptfds, 
           struct timeval *timeout);
```

- 参数：

  - nfds：要检测的文件描述符中最大的fd+1或者直接传1024；

    - 关于1024、标志位以及fd_set的解释可以参考fd_set的源码。

  - **readfds**：读集合；

  - writefds：写集合，一般传NULL；

  - exceptfds：异常集合，一般传NULL；

  - timeout：如果碰到所有缓冲区都没有可读的数据，阻塞的时间长度。

    结构体如下，赋值时秒和微秒都要赋值。

    ```cpp
    struct timeval {
        long tv_sec;  // 秒
        long tv_usec; // 微秒
    }
    ```

    - 如果传入NULL：永久阻塞
      - 当检测到fd变化的时候才返回；
    - timeval a：时间到了后无论缓冲区有没有数据都返回
      - a.tv_sec = 10；
      - a.tv_usec = 0。

- 返回值：整型，返回有几个活动事件。

### 文件描述符集

- 文件描述符类型：fd_set rdset；

- 文件描述符操作函数：

  - 全部清空

    - ```cpp
      void FD_ZERO(fd_set *set);
      ```

  - 从集合中删除某一项

    - ```cpp
      void FD_CLR(int fd, fd_set *set);
      ```

  - 将整个个文件描述符添加到集合

    - ```cpp
      void FD_SET(int fd, fd_set *set);
      ```

  - 判断某个文件描述符是否在集合中

    - ```cpp
      int FD_ISSET(int fd, fd_set *set);
      ```

### 使用select函数的优缺点：

- 优点：
  - 跨平台
- 缺点：
  - 每次调用select，都需要把fd集合从用户态拷贝到内核态，这个开销在fd数量大时会很大；

## select函数是如何工作的

假设客户端A、B、C、D、E、F连接到服务器，分别对用文件描述符3,4,100,100,102,103。

首先需要定义fd_set结构体变量文件描述符表reads：

```cpp
fd_set reads;
```

然后调用6次FD_SET函数，进行标志位值的设定：

```cpp
FD_SET(3, &reads);
FD_SET(4, &reads);
FD_SET(100, &reads);
FD_SET(101, &reads);
FD_SET(102, &reads);
FD_SET(103, &reads);
```

然后调用select()函数：

```cpp
int nums = select(103 + 1, &reads, NULL, NULL, NULL);
```

内核会把reads表拷贝到内核态，然后根据标志位的值去找对应的文件描述符，检测缓冲区是否有数据，如果有数据，将标志位设为1，如果没有数据，将标志位设为0，最后返回表中活动I/O的个数。

## select多路转接伪代码

```cpp
int main() {
    int lfd = socket();
    bind();
    listen();
    
    // 创建文件描述符表
    fd_set reads, temp;
    // 初始化
    FD_ZERO(&reads);
    // 监听的lfd加入到读集合
    FD_SET(lfd, &reads);
    int maxfd = lfd;
    
    while (true) {
        // 委托检测
        temp = reads;
        int ret = select(maxfd + 1, &temp, NULL, NULL, NULL);
        
        // 判断监听文件描述符是不是有新的连接申请
        if (FD_ISSET(lfd, &temp)) {
            // 如果文件描述符有新的连接申请，接受新连接
            int cfd = accept();
            // cfd加入集合
            FD_SET(cfd, &reads);
            // 更新maxfd
            maxfd = maxfd < cfd ? cfd : maxfd;
        } else {
            // 客户端发送数据，不能写3
            for (int i = lfd + 1; i < maxfd; i++) {
                if (FD_ISSET(i, &temp)) {
                    int len = read();
                    if (len == 0) {
                        // 将文件描述符i从reads表中删除
                        FD_CLR(i, &reads);
                    }
                    write();
                }
            }
        }
    }
}
```

实现代码（服务端）：

```cpp
#include <cstdio>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(2333);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
    bind(lfd, (sockaddr *)&serv_addr, sizeof(sockaddr));

    listen(lfd, 36);
    printf("accept connect...\n");

    sockaddr_in clnt_addr;
    socklen_t clnt_len = sizeof(clnt_addr);

    // 创建文件描述符表
    fd_set reads, temp;
    // 对表进行初始化
    FD_ZERO(&reads);
    // 将监听套接字的标识符置1，如果没有连接请求，那么select时会把该套接字置0
    FD_SET(lfd, &reads);
    int maxfd = lfd;

    while (true) {
        // 委托检测
        temp = reads;
        int ret = select(maxfd + 1, &temp, NULL, NULL, NULL);

        if (ret == -1) {
            std::cerr << "Select error!\n";
            exit(1);
        }

        // 判断是否是监听套接字活动
        if (FD_ISSET(lfd, &temp)) {
            char buff[] = "Hello! Connect successful!";
            int cfd = accept(lfd, (sockaddr *)&clnt_addr, &clnt_len);
            FD_SET(cfd, &reads);
            printf("New client ip: %s, port: %d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
            maxfd = maxfd < cfd ? cfd : maxfd;
            write(cfd, buff, sizeof(buff));
        }
        // 检测通信套接字活动
        for (int cfd = lfd + 1; cfd <= maxfd; cfd++) {
            if (FD_ISSET(cfd, &temp)) {
                char buff[1024];
                int len = read(cfd, buff, sizeof(buff));
                if (len == 0) {
                    FD_CLR(cfd, &reads);
                    printf("There is a client has been closed!\n");
                    close(cfd);
                } else if (len == -1) {
                    std::cerr << "Recv error!\n";
                    exit(1);
                } else {
                    printf("Message from client: %s\n", buff);
                    write(cfd, buff, sizeof(buff));
                }
            }
        }
    }

    close(lfd);
    return 0;
}
```

