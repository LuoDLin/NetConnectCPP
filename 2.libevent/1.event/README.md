## 函数API

### evconnlistener_new_bind

```c
struct evconnlistener *evconnlistener_new_bind(struct event_base *base, evconnlistener_cb cb, void *ptr, unsigned flags, int backlog, const struct sockaddr *sa, int socklen);
```

**描述**：

- `evconnlistener_new_bind` 函数用于创建一个新的监听器对象，用于监听指定地址和端口上的连接请求

**参数**：

- `base`：事件基础结构
- `cb`：连接请求的回调函数，在有新连接请求时调用
- `ptr`：传递给回调函数的用户自定义参数
- `flags`：选项标志，控制监听器的行为
- `backlog`：监听套接字的连接请求队列的大小
- `sa`：指向目标地址结构的指针，指定要监听的地址和端口
- `socklen`：地址结构的大小

**返回值**：

- 成功：返回一个指向新创建的监听器对象的指针
- 失败：返回 `NULL`

---

### event_base_new

```c
struct event_base *event_base_new(void);
```

**描述**：

- `event_base_new` 函数用于创建一个事件处理的基础结构。这个基础结构用于管理事件，文件描述符的监听，以及事件循环

**返回值**：

- 成功：返回一个指向新创建的 `event_base` 结构的指针
- 失败：返回 `NULL`

---

### event_new

```c
struct event *event_new(struct event_base *base, evutil_socket_t fd, short events, event_callback_fn callback, void *arg);
```

**描述**：

- `event_new` 函数用于创建一个事件对象，它将关联一个文件描述符（或套接字）和一组特定的事件类型。当这些事件发生时，指定的回调函数将被调用

**参数**：

- `base`：指向之前创建的事件基础结构的指针
- `fd`：要关联的文件描述符
- `events`：指定要监听的事件类型，如读、写等
- `callback`：事件发生时要调用的回调函数
- `arg`：传递给回调函数的用户定义参数

**返回值**：

- 成功：返回一个指向新创建的事件对象的指针
- 失败：返回 `NULL`

---

### event_add

```c
int event_add(struct event *ev, const struct timeval *timeout);
```

**描述**：

- `event_add` 函数用于将事件添加到事件基础结构中，使得该事件开始被监听

**参数**：

- `ev`：要添加的事件对象
- `timeout`：指定事件在多长时间后开始被监听，可以为 `NULL`，表示立即监听

**返回值**：

- 成功：返回 0
- 失败：返回 -1

---

### event_base_dispatch

```c
int event_base_dispatch(struct event_base *base);
```

**描述**：

- `event_base_dispatch` 函数用于启动事件循环，一直运行，直到没有要处理的事件或被中止

**参数**：

- `base`：事件基础结构

**返回值**：

- 成功：返回 0
- 失败：返回 -1

---

### event_base_loop

```c
int event_base_loop(struct event_base *base, int flags);
```

**描述**：

- `event_base_loop` 函数用于开始事件循环，它会一直运行，直到没有要处理的事件或被中止

**参数**：

- `base`：事件基础结构
- `flags`：控制事件循环行为的标志，如是否阻塞等

**返回值**：

- 成功：返回 0
- 失败：返回 -1

---

### event_assign

```c
void event_assign(struct event *ev, struct event_base *base, evutil_socket_t fd, short events, event_callback_fn callback, void *arg);
```

**描述**：

- `event_assign` 函数用于将已经存在的事件对象重新分配给一个不同的事件基础结构，以便在不同的事件循环中使用

**参数**：

- `ev`：要重新分配的事件对象
- `base`：目标事件基础结构
- `fd`：关联的文件描述符
- `events`：指定要监听的事件类型，如读、写等
- `callback`：事件发生时要调用的回调函数
- `arg`：传递给回调函数的用户定义参数

---

### event_set

```c
void event_set(struct event *ev, evutil_socket_t fd, short events, event_callback_fn callback, void *arg);
```

**描述**：

- `event_set` 函数用于设置一个已经存在的事件对象的属性，包括关联的文件描述符、事件类型和回调函数

**参数**：

- `ev`：要设置属性的事件对象
- `fd`：关联的文件描述符
- `events`：指定要监听的事件类型，如读、写等
- `callback`：事件发生时要调用的回调函数
- `arg`：传递给回调函数的用户定义参数

---

### event_free

```c
void event_free(struct event *ev);
```

**描述**：

- `event_free` 函数用于释放先前通过 `event_new` 创建的事件对象。释放后的事件对象不能再使用。

**参数**：

- `ev`：要释放的事件对象。

---

### event_base_free

```c
void event_base_free(struct event_base *base);
```

**描述**：

- `event_base_free` 函数用于释放先前通过 `event_base_new` 创建的事件基础结构。释放后的事件基础结构不能再使用

**参数**：

- `base`：要释放的事件基础结构

---