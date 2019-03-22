# epoll三种工作模式

## 水平触发模式LT（默认）

> Level Trigger是指信号只要出于高水平，就一直会触发。

### 工作过程

**读操作**

1. 只要fd对应的读缓冲区有数据；
2. epoll_wait()就会直接返回；
3. 返回的次数与发送数据的次数没有关系，直到缓冲区中所有数据读完为止。

即只要fd对应的读缓冲区中没有全部读完，LT模式就会返回读就绪！

**写操作**

只要fd对应的写缓冲区中没有充满，LT模式就会返回写就绪！

## 边沿触发模式ET

> Edge Trigger是指指信号为上升沿或者下降沿时触发。

### 工作过程

**读操作**

1. 当缓冲区由不可读变为可读的时候，即缓冲区由空变为不空的时候;
2. 当有新数据到达时，即缓冲区中的待读数据变多的时候;
3. 当缓冲区有数据可读，且应用进程对相应的描述符进行`EPOLL_CTL_MOD` 修改`EPOLLIN`事件时。

**写操作**

1. 当缓冲区由不可写变为可写时；
2. 当有旧数据被发送走，即缓冲区中的内容变少的时候；
3. 当缓冲区有空间可写，且应用进程对相应的描述符进行`EPOLL_CTL_MOD` 修改`EPOLLOUT`事件时。

```cpp
// 初始化，将监听的套接字挂到树上
epoll_event ev;
ev.data.ptr = new Info(lfd, serv_addr);   // 指向一块存有Info结构体的地址空间，info里已经有了fd数据
ev.events = EPOLLIN | EPOLLET;            // 设置ET模式
epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
...
```

**ET模式的问题**

- 无法一次性将缓冲区内的数据读出；
- 只能在每次发生事件的时候，继续读取缓冲区数据；
- 只要缓冲区中有数据，无法接受该客户端的其他请求。
- 解决办法：利用while()循环将数据一次性全部读出。。。但这个做法会导致新的问题
  - 读出全部数据后，缓冲区为空，此时recv()函数会阻塞，等待新的数据进入缓冲区；
  - 此时整个服务器进程就阻塞住了，陷入了死循环。

## 边沿非阻塞触发模式

> 效率最高！

### 如何设置非阻塞

1. 使用open()代替read()/recv()
   - 设置flags
   - 必须 O_WDRW | O_NONBLOCK
   - 终端文件：/dev/tty

2. fcntl()

   ```cpp
   int flag = fcntl(fd, F_GETFL);
   flag |= O_NONBLOCK;
   fcntl(fd, F_SETFL, flag);
   ```

### 将缓冲区的全部数据都读出

**设置为非阻塞以后，recv()函数的返回值是怎么样的？**

```cpp
while ((len = recv(info_ptr->fd, buff, sizeof(buff), 0)) > 0) {
    out.append(buff, len);
}
if (len == 0) {
   ...
} else if (len == -1) {
    ...
}
```

1. `>0`：即当前读取到的数据长度；
2. `0`：客户端关闭了连接；
3. `-1`：两种情况
   - errno == EAGAIN：说明缓冲区中已经没有了数据（非阻塞模式下，强制读缓冲区数据）；
   - errno != EAGAIN：读取错误！

**[实现代码1](epoll边沿非阻塞触发/epoll_et_nonblock1.cpp)**

