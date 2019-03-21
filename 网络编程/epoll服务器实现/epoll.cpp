#include <cstdio>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

struct Client {
    int fd;
    sockaddr_in addr;
};

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(2333);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 设置端口重用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
    bind(lfd, (sockaddr *)&serv_addr, sizeof(sockaddr));

    // 监听
    listen(lfd, 36);
    printf("Accept connect ...\n");

    // 客户端addr
    std::unordered_map<int, Client> client_map;
    socklen_t clnt_len = sizeof(sockaddr);

    // 根结点
    int epfd = epoll_create(3000);
    // 传入记录事件的数组
    epoll_event all[3000];

    // 初始化，将监听的套接字挂到树上
    epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    while (true) {
        // 委托内核检测事件
        int ret = epoll_wait(epfd, all, 3000, -1);

        // 遍历事件数组
        for (int i = 0; i < ret; i++) {
            int fd = all[i].data.fd;
            // 有新的连接
            if (fd == lfd) {
                Client client;
                int cfd = accept(lfd, (sockaddr *)&client.addr, &clnt_len);
                client_map[cfd] = client;
                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                char buff[30] = "Hello! Connect successful!\n";
                send(cfd, buff, sizeof(buff), 0);
                printf("New client, ip: %s port: %d\n", inet_ntoa(client.addr.sin_addr), ntohs(client.addr.sin_port));
            }
            // 如果是已经连接的客户端有新的消息进来
            else {
                // 只处理发过来的数据
                if (!all[i].events & EPOLLIN) continue;
                char buff[1024] = {0};
                int len = recv(fd, buff, sizeof(buff), 0);
                if (len == 0) {
                    close(fd);
                    client_map.erase(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    printf("There is a client(ip: %s port: %d) closed!\n",
                            inet_ntoa(client_map[fd].addr.sin_addr), ntohs(client_map[fd].addr.sin_port));
                } else if (len == -1) {
                    std::cerr << "Read error!\n";
                    exit(1);
                } else {
                    printf("A new message from client(ip: %s port: %d): %s\n",
                           inet_ntoa(client_map[fd].addr.sin_addr), ntohs(client_map[fd].addr.sin_port), buff);
                    send(fd, buff, sizeof(buff), 0);
                }
            }
        }
    }

    close(lfd);
    return 0;
}
