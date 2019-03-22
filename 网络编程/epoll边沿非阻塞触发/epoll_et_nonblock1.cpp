#include <cstdio>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <fcntl.h>
#include <errno.h>

struct Info {
    int fd;
    sockaddr_in addr;

    Info(int tfd, sockaddr_in taddr) : fd(tfd), addr(taddr) {}   // 构造函数
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
    sockaddr_in clnt_addr;
    socklen_t clnt_len = sizeof(sockaddr);

    // 根结点
    int epfd = epoll_create(3000);
    // 传入记录事件的数组
    epoll_event all[3000];

    // 初始化，将监听的套接字挂到树上
    epoll_event ev;
    ev.data.ptr = new Info(lfd, serv_addr);   // 指向一块存有Info结构体的地址空间，info里已经有了fd数据
    ev.events = EPOLLIN;            // 设置ET模式
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    while (true) {
        // 委托内核检测事件
        int ret = epoll_wait(epfd, all, 3000, -1);

        // 遍历事件数组
        for (int i = 0; i < ret; i++) {
            Info *info_ptr = (Info *)all[i].data.ptr;  // 定义新的指针指向info(fd, addr)变量
            // 有新的连接
            if (info_ptr->fd == lfd) {
                int cfd = accept(lfd, (sockaddr *)&clnt_addr, &clnt_len);
                if (cfd == -1) {
                    std::cerr << "Accept error!\n";
                    exit(1);
                }
                // 设置通信的文件描述符为非阻塞
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                ev.data.ptr = new Info(cfd, clnt_addr);    // 新的客户端info结构体
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);  // 新的(fd, addr)挂到树上
                char buff[30] = "Hello! Connect successful!\n";
                send(cfd, buff, sizeof(buff), 0);
                printf("New client, ip: %s port: %d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
            }
            // 如果是已经连接的客户端有新的消息进来
            else {
                // 只处理发过来的数据
                if (!all[i].events & EPOLLIN) continue;
                char buff[3] = {0};
                std::string out;
                int len = -1;
                printf("A new message from client(ip: %s port: %d): ",
                       inet_ntoa(info_ptr->addr.sin_addr), ntohs(info_ptr->addr.sin_port));
                while ((len = recv(info_ptr->fd, buff, sizeof(buff), 0)) > 0) {
                    out.append(buff, len);
                }
                if (len == 0) {
                    printf("I closed!\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, info_ptr->fd, NULL);
                    close(info_ptr->fd);
                } else if (len == -1) {
                    if (errno == EAGAIN) {
                        std::cout << out;
                        send(info_ptr->fd, out.c_str(), out.size(), 0);
                    } else {
                        std::cerr << "Read error!\n";
                        exit(1);
                    }
                }
            }
        }
    }

    close(lfd);
    return 0;
}
