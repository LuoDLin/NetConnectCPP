### socket

~~~c
int socket(int domain, int type, int protocol);
~~~

`socket` 函数用于创建一个套接字（socket），它是网络通信中的基本构建块，允许不同计算机之间进行数据传输。

##### 参数

`domain`: 地址族, `AF_INET` 表示IPv4地址族，`AF_INET6` 表示IPv6地址族

`type`: 套接字类型, `SOCK_STREAM`面向连接的流套接字(TCP), `SOCK_DGRAM`无连接的数据报套接字(UDP)

`protocol`: 协议，通常为0，表示系统会根据 `domain` 和 `type` 自动选择合适的协议，如TCP或UDP

---

### setsockopt

```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

---

### bind

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

---

### listen

```c
int listen(int sockfd, int backlog);
```

---

### accept

~~~c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
~~~

---







