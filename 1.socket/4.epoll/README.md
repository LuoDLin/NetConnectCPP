## 代码流程图

``` mermaid
graph LR
    socket[创建套接字&#40socket&#41]
    setsockopt[设置端口复用&#40setsockopt&#41]
    bind[绑定地址结构&#40bind&#41]
    listen[设置监听上限&#40listen&#41]
    accept[接受连接请求&#40accept&#41]
    read[读取数据&#40read&#41]
    write[发送数据&#40write&#41]
    close[关闭套接字&#40close&#41]
    epoll[等待事件&#40epoll_wait&#41]
    init[epoll的创建&#40epoll_create&#41]
    finsh[所有事件处理完成]
    add[添加客户端描述符到epoll&#40epoll_ctl&#41]
    del[从epoll中删除对应描述符&#40epoll_ctl&#41]
    socket --> setsockopt --> bind --> listen --> init --> epoll
    epoll --> |server fd可读| accept --> add --> finsh
    epoll --> |client fd可读| read
    read -->|返回值等于0| del--> close --> finsh
    read -->|返回值大于0| write --> finsh
    finsh --> epoll    
```

创建套接字=>设置端口复用(可选)=>绑定地址结构=>设置监听上限=>初始化文件描述符数组=>等待事件


- 若有新的连接请求=>接受连接请求=>将新的连接套接字添加到文件描述符数组
- 若有新的数据请求=>接收数据=>处理数据=>发送数据

`epoll` 使用了事件通知，可以有效地处理大规模并发连接，而不会随着文件描述符数量的增加而性能下降。`select` 和 `poll` 则需要轮询所有被监视的文件描述符，效率较低。此外，`epoll` 支持水平触发和边缘触发两种工作模式，而 `select` 和 `poll` 只支持水平触发。总而言之，`epoll` 在处理高并发、大规模连接的情况下表现更出色，适合于构建高性能的网络服务器，而 `select` 和 `poll` 则适用于一些较简单的情景。

需要注意的一点就是在关闭套接字之后，`epoll`会自动将其从监听列表中删除，你无需手动执行删除操作。也就是说，如果在`close(fd)`之后再使用`epoll_ctl`操作`fd`可能就会返回操作失败，操作失败的原因为`Bad file descriptor`，因为`close(fd)`之后`epoll`的监听列表里面已经没有该文件描述符了。


## 函数API

### epoll_create

```c
#include <sys/epoll.h>
int epoll_create(int size);
```

#### 描述

- `epoll_create` 函数用于创建一个 `epoll` 实例，返回一个文件描述符，该实例可以用于监视多个文件描述符的事件

#### 参数

- `size`：用于指定预期需要监视的文件描述符数量。但是这只是一个预留的大小，不是强制的

#### 返回值

- 成功：返回一个新的 `epoll` 实例的文件描述符
- 失败：返回 -1，并设置 `errno`

---

### epoll_ctl

```c
#include <sys/epoll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

#### 描述

- `epoll_ctl` 函数用于向 `epoll` 实例中添加、修改或删除要监视的文件描述符和关注的事件

#### 参数

- `epfd`：表示之前创建的 `epoll` 实例的文件描述符
- `op`：指定操作类型，可以是以下三种之一：
  - `EPOLL_CTL_ADD`：添加文件描述符到 `epoll` 实例中
  - `EPOLL_CTL_MOD`：修改已有文件描述符的事件设置
  - `EPOLL_CTL_DEL`：从 `epoll` 实例中删除文件描述符
- `fd`：表示要进行操作的文件描述符
- `event`：用于描述要监视的事件类型

#### 返回值

- 成功：返回 0
- 失败：返回 -1，并设置 `errno`

---

###  epoll_wait

```c
#include <sys/epoll.h>
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

#### 描述

- `epoll_wait` 函数用于等待事件的发生，一旦有事件发生，它会返回相关的文件描述符和事件类型

#### 参数

- `epfd`：表示之前创建的 `epoll` 实例的文件描述符
- `events`：用于存储发生事件的文件描述符和事件类型
- `maxevents`：表示 `events` 数组的大小，即最多能够一次处理的事件数量
- `timeout`：表示等待事件发生的超时时间，以毫秒为单位。设置为负值表示无限等待，设置为 0 表示立即返回，设置为正值表示等待指定毫秒数后返回

#### 返回值

- 成功：返回就绪的文件描述符数量
- 超时：返回 0
- 失败：返回 -1，并设置 `errno`

---

## 结构体说明

###  struct epoll_event

#### 成员说明

- `events`：用于描述监视的事件类型，可以使用以下常量进行设置：
  - `EPOLLIN`：表示关注可读事件（文件描述符可读）
  - `EPOLLOUT`：表示关注可写事件（文件描述符可写）
  - `EPOLLERR`：表示关注错误事件
  - `EPOLLHUP`：表示关注挂起事件（通常是连接关闭）
  - `EPOLLRDHUP`：表示关注对端关闭连接事件
  - `EPOLLET`：设置为启用边缘触发模式，即只在状态变化时通知
  - `EPOLLONESHOT`：设置为一次性事件，即事件发生后需要重新添加到 `epoll` 实例中
- `data`：用于存储用户数据的联合体，可以存储不同类型的数据，根据不同的应用场景选择合适的成员来存储数据：
  - `ptr`：指针类型的用户数据
  - `fd`：整数类型的文件描述符
  - `u32`：32 位整数类型的数据
  - `u64`：64 位整数类型的数据