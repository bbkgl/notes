#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
    // 创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 绑定套接字
    sockaddr_in serv_addr;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);
    serv_addr.sin_family = AF_INET;
    bind(serv_sock, (sockaddr *)&serv_addr, sizeof(serv_addr));

    // 进入监听
    listen(serv_sock, 20);

    // 接收客户端请求
    sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);

    // 接收和回复客户端消息
    char buff[200];

    while (true) {
        int clnt_sock = accept(serv_sock, (sockaddr *)&clnt_addr, &clnt_addr_len);

        read(clnt_sock, buff, sizeof(buff) - 1);
        if (strcmp(buff, "exit") == 0) {
            printf("收到断开连接要求，正在断开连接...");
            break;
        }
        printf("Message from client: %s\n", buff);
        std::string s = "I has received your Message \"";
        s = s + buff + "\".";
        write(clnt_sock, s.c_str(), s.length() + 2);

        // 关闭和客户端通信的套接字
        close(clnt_sock);
        memset(buff, 0, sizeof(buff));
    }

    // 关闭服务端接收连接的套接字
    close(serv_sock);

    return 0;
}