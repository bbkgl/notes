# epoll原理

> 参考[epoll原理](https://www.cnblogs.com/pluser/p/epoll_principles.html)
>
> 参考[epoll内核原理极简图文解读](https://blog.csdn.net/linkedin_38454662/article/details/73337208 )

## 解释

当有要处理一百万个连接时，epoll的工作流程是：

1. 通过epoll_create()函数创建一个epoll对象，epoll对象中有一个结构体`struct eventpoll`，包括一棵红黑树和一个双向链表，返回一个epoll对象的文件描述符；
2. 通过epoll_ctl(EPOLL_CTL_ADD)函数向epoll红黑树中添加结点，每个文件描述符对应一个结构体`struct epitem`；
3. 调用epoll_wait()函数收集活跃的套接字，放入到双向链表中，最后填到传入epoll_wait()函数中的数组中；

从源码的解释来说更好：

```cpp
struct eventpoll {
    // 红黑树根结点，树中存储着所有委托检测的事件和套接字
    struct rb_root rbr;
    
    // 双向链表保存着将要通过epoll_wait()返回给用户的、活跃的事件
    struct list_head rdllist;
}
```

- 每个epoll对象都对应着一个`eventpoll`结构体，新添加的事件节点都会被挂载到红黑树上，这样重复添加的事件就能很快被识别出来。
- 每个添加到红黑树中的事件都会与设备驱动程序建立回调关系，也就是相应的事件发生时会调用这里的回调方法。这个回调方法在内核中叫做ep_poll_callback，它会把活跃的事件放入到双向链表中。

在epoll中对于每一个事件都会建立一个结构体`struct epitem`：

```cpp
struct epitem {
    // 结点信息
    ...
}
```

当调用epoll_wait()检查是否有活跃的事件时，只是检查双向链表`rdllist`是否为空，为空返回0，不为空的话就将活跃的事件填入到传入的数组中，同时返回活跃事件的数量。

## 总结

- epoll_create()会创建epoll对象，该对象有一个eventpoll结构体，结构体中有关键成员：存放所有事件的红黑树`struct rb_root rbr`，保存epoll_wait()返回的活跃事件的双向链表`struct list_head rdllist`；
- epoll_ctl()方法向epoll对象中注册事件、删除和修改事件，主要是从红黑树中进行增删改操作，并与设备驱动建立、修改或者删除回调关系。事件活跃以后调用回调，使得活跃的事件被添加到双向链表rdllist中；
- epoll_wait()检查双向链表中是否为空，不为空返回活跃事件数量，同时填充到传入函数的数组中，为空则返回0；
- 和select、poll相比的优势：
  - select、poll每次只能返回活跃的事件数量，仍需轮询找出活跃的事件；
  - select、poll每调用一次，需要将大量的时间和文件描述符传给内核，委托内核去检测，这样的从用户态到内核的复制会非常影响效率，而且内核仍需轮询检测；