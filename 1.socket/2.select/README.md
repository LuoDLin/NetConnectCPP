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
    select[等待事件&#40select&#41]
    setfd[设置fd_set&#40FD_SET&#41]
    finsh[所有事件处理完成]
    addset[添加客户端描述符到fd_set]
    socket --> setsockopt --> bind --> listen --> setfd --> select
    select --> |server fd可读| accept --> addset --> finsh
    select --> |client fd可读| read
    read -->|返回值等于0| close --> finsh
    read -->|返回值大于0| write --> finsh
    finsh --> setfd
    
```

- 服务器

  创建套接字=>设置端口复用(可选)=>绑定地址结构=>设置监听上限=>设置文件描述符集合=>监听文件描述符集合


  - 若有新的连接请求=>接受连接请求=>将新的连接套接字添加到文件描述符集合
  - 若有新的数据请求=>接收数据=>处理数据=>发送数据


- 说明

  - 使用 `select` 函数实现的服务器，可以同时处理多个客户端的连接请求，并且可以接收多个客户端的数据请求。又因为 `select` 函数是阻塞的，每次只有当有事件发生时才会返回，并且不知道是哪些客户端的数据，所以在处理数据请求时需要逐个检查每个客户端是否有数据。


## 函数API

### select

```c
#include <sys/select.h>
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

#### 描述

- `select` 函数用于在一组套接字上进行异步 I/O 多路复用，它可以用于监视多个套接字是否处于可读、可写或异常等状态，从而实现非阻塞的 I/O 操作

#### 参数

- `nfds`：监视的最大套接字描述符值加 1
- `readfds`：文件描述符集合，用于监视是否有数据可读
- `writefds`：文件描述符集合，用于监视是否可以进行写操作
- `exceptfds`：文件描述符集合，用于监视是否有异常情况
- `timeout`：超时时间，设置为 `NULL` 表示阻塞直到有事件发生，否则可以设定一个时间来指定阻塞的最大时间

#### 返回值

- 如果有套接字可读、可写或有异常情况发生，返回大于 0 的整数，表示就绪的套接字数量
- 如果超时时间内没有任何事件发生，返回 0
- 如果出现错误，返回 -1，并设置 `errno`

---

### FD_ZERO

```c
#include <sys/select.h>
void FD_ZERO(fd_set *set);
```

#### 描述

- `FD_ZERO` 函数用于将一个文件描述符集合清空，即将集合中的所有文件描述符都设置为未设置状态

#### 参数

- `set`：要清空的集合

#### 返回值

- 无返回值

---

### FD_ISSET

```c
#include <sys/select.h>
int FD_ISSET(int fd, fd_set *set);
```

#### 描述

- `FD_ISSET` 函数用于检查给定的文件描述符是否在集合中设置，如果设置了，则返回非零值，否则返回零

#### 参数

- `fd`：要检查的文件描述符
- `set`：要检查的集合

#### 返回值

- 如果文件描述符在集合中设置，返回非零值（通常是 1），否则返回零

---

### FD_CLR

```c
#include <sys/select.h>
void FD_CLR(int fd, fd_set *set);
```

#### 描述

- `FD_CLR` 函数用于从文件描述符集合中移除给定的文件描述符

#### 参数

- `fd`：要从集合中移除的文件描述符
- `set`：要修改的集合

#### 返回值

- 无返回值

----

### FD_SET

```c
#include <sys/select.h>
void FD_SET(int fd, fd_set *set);
```

#### 描述

- `FD_SET` 函数用于将给定的文件描述符添加到文件描述符集合中，将其设置为已设置状态

#### 参数

- `fd`：要添加到集合中的文件描述符
- `set`：要修改的集合

#### 返回值

- 无返回值

---