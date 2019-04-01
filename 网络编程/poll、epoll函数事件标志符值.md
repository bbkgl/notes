# poll、epoll函数事件标志符值

---

> 在linux中，poll、epoll的事件标志符值是一样的

## poll()

 常量 | 说明
:-:|:-:
POLLIN | 普通或优先级带数据可读
POLLRDNORM | 普通数据可读
POLLRDBAND | 优先级带数据可读
POLLPRI | 高优先级数据可读
POLLOUT | 普通数据可写
POLLWRNORM | 普通数据可写
POLLWRBAND | 优先级带数据可写
POLLERR | 发生错误
POLLHUP | 对方描述符挂起
POLLNVAL | 描述字不是一个打开的文件

## epoll()

 常量 | 说明
:-:|:-:
EPOLLIN | 普通或优先级带数据可读
EPOLLRDNORM | 普通数据可读
EPOLLRDBAND | 优先级带数据可读
EPOLLPRI | 高优先级数据可读
EPOLLOUT | 普通数据可写
EPOLLWRNORM | 普通数据可写
EPOLLWRBAND | 优先级带数据可写
EPOLLERR | 发生错误
EPOLLHUP | 对方描述符挂起
EPOLLNVAL | 描述字不是一个打开的文件

