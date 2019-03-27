# EvenBase类

EvenBase类的作用就是抽象成网络服务器中的事件，且同一时间他只能被抽象成为一种事件，并将这些事件分发给相应的对象去处理，具体如下几种事件：

- 读事件；
- 写事件；
- 错误事件；

## EvenBase.h

### 私有属性

```cpp
private:
    // 以下两个变量对应epoll_event结构体中内容
    // 文件描述符
    const int fd_;
    // 关注的事件
    int events_;

    // 返回的活跃事件
    int revents_;

    // 事件处理函数
    ReadCallback read_callback_;
    Callback write_callback_;
    Callback error_callback_;
    Callback close_callback_;
```

1. fd_是当前事件对应的文件描述符；
2. events_是当前EvenBase事件的种类；
3. revents_指经过`void SetRevents`函数设置后的活跃事件；
4. callback指的相应的仿函数，用来调用回调函数。

的（）