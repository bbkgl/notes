#include <cstdio>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <algorithm>

// 常量定义
const unsigned int EPOLL_TREE_SIZE = 3000;   // epoll红黑树最大结点数
const unsigned BUFF_SIZE = 30;               // 每次读取和发送的字节数

class Info {
public:
    int fd;
    sockaddr_in addr;

    Info(int _fd, sockaddr_in _addr);
};

Info::Info(int _fd, sockaddr_in _addr) : fd(_fd), addr(_addr) {}

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_len = sizeof(sockaddr_in);
    serv_addr.sin_port = htons(2333);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 启用IO复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
    // 绑定
    bind(lfd, (sockaddr *)&serv_addr, addr_len);

    // 监听
    listen(lfd, 36);
    printf("Accept connect...\n");

    // 红黑树头结点
    int epfd = epoll_create(EPOLL_TREE_SIZE);

    // 初始化，将监听套接字挂树上
    epoll_event all[EPOLL_TREE_SIZE];
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = new Info(lfd, serv_addr);
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    // 通信的字节串
    char buff[BUFF_SIZE];

    while (true) {
        // 委托内核
        int ret = epoll_wait(epfd, all, EPOLL_TREE_SIZE, -1);
        if (ret == -1) {
            std::cerr << "epoll_wait error!\n";
            exit(1);
        }

        for (int i = 0; i < ret; i++) {
            Info *info_ptr = static_cast<Info *>(all[i].data.ptr);

            // 如果是新的连接
            if (info_ptr->fd == lfd) {
                // 接收请求
                int cfd = accept(lfd, (sockaddr *)&clnt_addr, &addr_len);

                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                ev.events = EPOLLIN | EPOLLET; // 设置边沿触发模式
                ev.data.ptr = new Info(cfd, clnt_addr);
                // 新的连接挂树上
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                printf("New client, ip: %s, port: %d.\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                // 告知客户端连接成功
                strcpy(buff, "Hello, connect successful!\n");
                send(cfd, buff, BUFF_SIZE, 0);
            } else {// 新的通信到达
                std::string out;
                char buff[BUFF_SIZE];
                ssize_t len = -1;
                while ((len = recv(info_ptr->fd, buff, BUFF_SIZE, 0)) > 0) {
                    out.append(buff, len);
                }
                if (len == -1) {
                    printf("Message from client(ip: %s, port: %d): ", inet_ntoa(info_ptr->addr.sin_addr), ntohs(info_ptr->addr.sin_port));
                    if (errno == EAGAIN) {
                        std::cout << out;

                        // 输出大写
                        std::transform(out.begin(), out.end(), out.begin(), ::toupper);
                        send(info_ptr->fd, out.c_str(), out.size(), 0);
                    } else {
                        std::cerr << "Recv error!\n";
                        exit(1);
                    }
                } else if (len == 0) {
                    printf("Thre is a client(ip: %s, port: %d) closed!\n", inet_ntoa(info_ptr->addr.sin_addr), ntohs(info_ptr->addr.sin_port));

                    // 从树上删除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, info_ptr->fd, nullptr);

                    // 关闭通信的套接字
                    close(info_ptr->fd);
                }
            }
        }
    }

    close(lfd);
    return 0;
}
