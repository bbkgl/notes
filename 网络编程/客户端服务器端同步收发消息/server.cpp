#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // 创建socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 绑定socket
    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(sockaddr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = ntohs(2333);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(serv_sock, (sockaddr *)&serv_addr, sizeof(sockaddr));

    // 进入监听状态
    listen(serv_sock, 20);

    // 接收客户端的请求
    sockaddr_in clnt_addr;
    bzero(&clnt_addr, sizeof(clnt_addr));
    socklen_t clnt_addr_len = sizeof(sockaddr);
    int clnt_sock = accept(serv_sock, (sockaddr *)&clnt_addr, &clnt_addr_len);

    const unsigned int size = 50;            // 定义每次收发消息的最大长度
    char buff[size];
    while (true) {
        int len = recv(clnt_sock, buff, size, 0);  // 接收客户端消息
        if (strcmp(buff, "exit") == 0) {           // 如果客户端提出断开连接
            printf("...exit...\n");
            break;
        }
        printf("Message from client: %s.\n", buff);   // 打印消息
        printf("Please input your message(type \"exit\" to exit): ");

        // 回复客户端消息
        scanf("%s", buff);
        send(clnt_sock, buff, size, 0);
        if (strcmp(buff, "exit") == 0) {    // 如果回复的是exit，则断开
            printf("...exit...\n");
            break;
        }
    }

    // 关闭套接字
    close(clnt_sock);
    close(serv_sock);

    return 0;
}
