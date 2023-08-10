### socket

~~~c
int socket(int domain, int type, int protocol);
~~~

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

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

---







