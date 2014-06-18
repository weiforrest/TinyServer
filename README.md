TinyServer
----------
自己写的一个http服务器，主要使用了多进程池和epoll来设计。
使用的模型是Reactor模型，master进程负载均衡，worker处理连接。
同时使用epoll统一事件源。

整个程序分为了几个模块：
processpoll: 负责进程池的创建，运行，通信。
fdwapper: 封装了epoll的操作。
log: 负责日志处理。
manager: worker进程对应一个manager，管理http_conn连接。
http_conn: 对应一个http连接，处理http请求处理。
