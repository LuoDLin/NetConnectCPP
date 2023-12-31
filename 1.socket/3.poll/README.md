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
    poll[等待事件&#40poll&#41]
    initset[初始化pollfd数组]
    finsh[所有事件处理完成]
    addset[添加客户端描述符到pollfd数组]
    socket --> setsockopt --> bind --> listen --> initset --> poll
    poll --> |server fd可读| accept --> addset --> finsh
    poll --> |client fd可读| read
    read -->|返回值等于0| close --> finsh
    read -->|返回值大于0| write --> finsh
    finsh --> poll    
```

创建套接字=>设置端口复用(可选)=>绑定地址结构=>设置监听上限=>初始化文件描述符数组=>等待事件


- 若有新的连接请求=>接受连接请求=>将新的连接套接字添加到文件描述符数组
- 若有新的数据请求=>接收数据=>处理数据=>发送数据

`select` 和 `poll` 都是用来同时监视多个文件描述符的工具，但`select` 使用文件描述符集合，而 `poll` 使用结构体数组，这样就导致了`select` 的文件描述符数量有限，而 `poll` 没有这个限制，能够处理更多的文件描述符，同时在文件描述符较多时，`poll` 通常更高效，因为它避免了 `select` 使用的一些效率问题


## 函数API

### poll

```c
#include <sys/poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

#### 描述

- `poll` 函数，类似于 `select` 函数，但提供更强大的功能和灵活性它可以监视多个文件描述符是否处于可读、可写或异常等状态，从而实现非阻塞的 I/O 操作

#### 参数

- `fds`：每个结构体描述一个要监视的文件描述符及其关注的事件
- `nfds`：数组中元素的数量，即要监视的文件描述符的个数
- `timeout`：超时时间，以毫秒为单位设置为负值表示无限等待，设置为 0 表示立即返回，设置为正值表示等待指定毫秒数后返回

#### 返回值

- 如果有文件描述符处于可读、可写或有异常情况发生，返回大于 0 的整数，表示就绪的文件描述符数量
- 如果超时时间内没有任何事件发生，返回 0
- 如果出现错误，返回 -1，并设置 `errno`

---

## 结构体说明

### struct pollfd

#### 成员说明

- `fd`：要监视的文件描述符，当 `poll` 函数调用时，会检查这个文件描述符是否就绪
- `events`：表示关注的事件，可以使用以下常量进行设置：
  - `POLLIN`：表示文件描述符可读，当有数据可以从文件描述符读取时，会设置该标志
  - `POLLOUT`：表示文件描述符可写，当可以向文件描述符写入数据时，会设置该标志
  - `POLLERR`：表示文件描述符发生错误，当文件描述符出现错误时，会设置该标志
  - `POLLHUP`：表示文件描述符挂起，当文件描述符挂起（通常是对端关闭连接）时，会设置该标志
- `revents`：表示实际发生的事件，由系统设置，在调用 `poll` 函数后，会填充这个字段，以指示实际发生了哪些事件