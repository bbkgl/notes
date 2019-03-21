# epoll介绍

> epoll内部实现是红黑树！

## 三个函数

### epoll_create()

> 该函数生成一个epoll专用的文件描述符。

**函数原型**

```cpp
int epoll_create(int size);
```

**参数介绍**

- size：epoll上能关联的最大描述符数；

### epoll_ctl()

> 该函数用于控制某个epoll文件描述符事件，可以注册，修改，删除。

**函数原型**

```cpp
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

**参数介绍**

- epfd：epoll_create生成的epoll专用描述符；

- op：

  - EPOLL_CTL_ADD      注册
  - EPOLL_CTL_MOD     修改
  - EPOLL_CTL_DEL       删除

- fd：关联的文件描述符；

- event：告诉内核要监听什么事件.

  ```cpp
  // epoll_data_t联合体
  typedef union epoll_data {
      void *ptr;
      int fd;
      uint32_t u32;
      uint64_t u64;
  } epoll_data_t;
                                                         
  struct epoll_event {
      uint32_t events;
      epoll_data_t data;
  }
  ```

  - events：
    - EPOLLIN          读
    - EPOLLOUT      写
    - EPOLLERR        异常

### epoll_wait()

> 等待IO事件，可以设置阻塞的函数，对应poll()和select()函数。

**函数原型**

```cpp
int epoll_wait(
	int epfd;
    strcut epoll_event *events;  // 数组
    int maxevents;
    int time out;
)
```

**参数**

- epfd：要检测的句柄；

- events：用于回传处理事件的数组；

- maxevents：告诉内核这个events的大小；

- timeout：为超时时间。

  - -1：永久阻塞
  - 0：立即返回
  - `>0`：超时时间

  

## epoll模型（伪代码）

```cpp
int main() {
    int lfd = socket();
    bind();
    listen();
    
    // 创建根结点
    int epfd = epoll_create(3000);
    // 存储发生事件的fd对应信息
    epoll_event all[3000];
    // 初始化
    // 监听的文件描述符lfd挂到epoll树上
    epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    
    while (true) {
        // 委托内核检测事件
        int ret = epoll_wait(epfd, all, 3000, -1);
        // 根据ret遍历all数组
        for (int i = 0; i < ret; i++) {
            int fd = all[i].data.fd;
            // 有新的连接
            if (fd == lfd) {
                // 接收连接请求-accept不阻塞
                int cfd = accept();
                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLLCTL_ADD, cfd, &ev);
            }
            // 已经连接的客户端有数据发送过来
            else {
                // 只处理客户端发来的数据
                if (!all[i].events & EPOLLIN) // 如果不是EPOLLIN，即EPOLLIN为0 
                    continue;
                // 读数据
                int len = recv();
                if (len == 0) {
                    close(fd);
                    // 检测的fd从树上删除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                }
                // 写数据
                send();
            }
        }
    }
    
    return 0;
}
```

