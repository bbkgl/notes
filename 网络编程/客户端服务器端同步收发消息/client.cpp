#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // 创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 套接字参数设定
    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(sockaddr));
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(2333);
    serv_addr.sin_family = AF_INET;
    connect(serv_sock, (sockaddr *)&serv_addr, sizeof(sockaddr));

    const unsigned int size = 50;            // 定义每次收发消息的最大长度
    char buff[size];
    while (true) {

        // 给服务器端发送消息
        printf("Please input your message(type \"exit\" to exit): ");
        scanf("%s", buff);
        send(serv_sock, buff, size, 0);
        if (strcmp(buff, "exit") == 0) {    // 如果发送的是exit，则断开
            printf("...exit...\n");
            break;
        }

        int len = recv(serv_sock, buff, size, 0);  // 接收服务器端消息
        if (strcmp(buff, "exit") == 0) {           // 如果服务器端提出断开连接
            printf("...exit...\n");
            break;
        }
        printf("Message from server: %s.\n", buff);   // 打印消息
    }

    // 关闭套接字
    close(serv_sock);

    return 0;
}
