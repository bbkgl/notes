#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(){
    // 设置连接服务端的参数
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 向服务端发送消息
    char buff[100];
    while (strcmp(buff, "exit") != 0) {
        // 创建套接字
        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        // 重新建立连接
        connect(sock, (sockaddr *)&serv_addr, sizeof(sockaddr));

        // 发送消息
        printf("Please input your message: ");
        memset(buff, 0, sizeof(buff));
        scanf("%s", buff);
        printf("My message: %s\n", buff);
        write(sock, buff, sizeof(buff) - 1);
        read(sock, buff, sizeof(buff) - 1);
        printf("Message from server: %s\n", buff);
    }

    return 0;
}