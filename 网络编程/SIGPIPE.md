# SIGPIPE

> 参考：[signal(SIGCHLD, SIG_IGN)和signal(SIGPIPE, SIG_IGN);](https://blog.csdn.net/xinguan1267/article/details/17357093)

## signal(SIGPIPE, SIG_IGN)

TCP是全双工的信道，可以看作两条单工信道，TCP连接两端的两个端点各负责一条，当对端调用close()时，虽然本意是关闭整条信道，但本端只收到FIN包，按照TCP协议的语义，表示对端只是关闭了其所负责的那一条单工信道，仍然可以继续接收数据。

也就是说，因为TCP协议的限制，一个端点无法获知对端的socket是调用了close还是shutdown。

对一个已经收到FIN包的socket调用read()方法，如果接收缓冲已空, 则返回0, 这就是常说的表示连接关闭。但第一次对其调用write方法时, 如果发送缓冲没问题, 会返回正确写入(发送)。但发送的报文会导致对端发送RST报文，因为对端的socket已经调用了close()， 完全关闭, 既不发送, 也不接收数据。所以， 第二次调用write方法(假设在收到RST之后)，会生成SIGPIPE信号，导致进程退出。为了避免进程退出, 可以捕获SIGPIPE信号，或者忽略它, 给它设置SIG_IGN信号处理函数：

```cpp
signal(SIGPIPE, SIG_IGN);
```

这样，第二次调用write()方法时，会返回-1，同时errno置为SIGPIPE，程序便能知道对端已经关闭。