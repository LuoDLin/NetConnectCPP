## 函数API

### bufferevent_socket_new

```c
struct bufferevent *bufferevent_socket_new(struct event_base *base, evutil_socket_t fd, int options);
```

**描述**：

- `bufferevent_socket_new` 函数用于创建一个基于套接字的缓冲事件，用于在读写套接字数据时提供缓冲管理和事件处理

**参数**：

- `base`：事件基础结构
- `fd`：关联的套接字描述符
- `options`：用于设置缓冲事件的选项，例如设置非阻塞等
    - **BEV_OPT_CLOSE_ON_FREE**: 释放 buffer event 时也会关闭套接字
    - **BEV_OPT_THREADSAFE**: 为 buffer event 使用额外的锁来确保线程安全性
    - **BEV_OPT_DEFER_CALLBACKS**: 回调函数（例如读取和写入回调）将会被延迟执行，直到调用了 `event_base_loop` 或 `event_base_dispatch` 等事件循环函数。这可以在某些情况下提高性能
    - **BEV_OPT_UNLOCK_CALLBACKS**: 在启用了 `BEV_OPT_THREADSAFE` 的情况下，这个选项允许在没有锁定的情况下调用回调函数。这可能会提高性能，但需要确保在多线程环境中没有竞争条件

**返回值**：

- 成功：返回一个指向新创建的缓冲事件对象的指针
- 失败：返回 `NULL`

### bufferevent_setcb

```c
int bufferevent_setcb(struct bufferevent *bufev, bufferevent_data_cb readcb, bufferevent_data_cb writecb, bufferevent_event_cb eventcb, void *cbarg);
```

**描述**：

- `bufferevent_setcb` 函数用于设置缓冲事件的回调函数，包括读取回调、写入回调和事件回调

**参数**：

- `bufev`：缓冲事件对象
- `readcb`：读取数据时调用的回调函数
- `writecb`：写入数据时调用的回调函数
- `eventcb`：事件发生时调用的回调函数
- `cbarg`：传递给回调函数的用户定义参数

**返回值**：

- 成功：返回 0
- 失败：返回 -1

### bufferevent_enable

```c
int bufferevent_enable(struct bufferevent *bufev, short event);
```

**描述**：

- `bufferevent_enable` 函数用于启用缓冲事件的特定事件类型，如读取事件、写入事件等

**参数**：

- `bufev`：缓冲事件对象
- `event`：要启用的事件类型
    - **EV_READ**： 读取事件，当缓冲中有可读数据时触发
    - **EV_WRITE**： 写入事件，当缓冲中有足够的空间时触发

**返回值**：

- 成功：返回 0
- 失败：返回 -1

### bufferevent_disable

```c
int bufferevent_disable(struct bufferevent *bufev, short event);
```

**描述**：

- `bufferevent_disable` 函数用于禁用缓冲事件的特定事件类型，停止监听指定类型的事件

**参数**：

- `bufev`：缓冲事件对象
- `event`：要禁用的事件类型
    - **EV_READ**： 读取事件，当缓冲中有可读数据时触发
    - **EV_WRITE**： 写入事件，当缓冲中有足够的空间时触发

**返回值**：

- 成功：返回 0
- 失败：返回 -1

### bufferevent_write

```c
int bufferevent_write(struct bufferevent *bufev, const void *data, size_t size);
```

**描述**：

- `bufferevent_write` 函数用于将数据写入缓冲事件，数据将被缓冲并异步写入套接字

**参数**：

- `bufev`：缓冲事件对象
- `data`：要写入的数据的指针
- `size`：要写入的数据大小

**返回值**：

- 成功：返回 0
- 失败：返回 -1

### bufferevent_read

```c
size_t bufferevent_read(struct bufferevent *bev, void *data, size_t size);
```

**描述**：

- `bufferevent_read` 函数用于从缓冲事件中读取数据到指定的缓冲区中

**参数**：

- `bufev`：缓冲事件对象
- `data`：目标缓冲区的指针
- `size`：要读取的数据大小

**返回值**：

- 返回实际读取的数据大小

