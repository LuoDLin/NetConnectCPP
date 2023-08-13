#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <event2/event.h>


#define SERVER_PORT 8080
#define CLIENT_MAX 10

void sys_err(const char *str){
    perror(str);
    exit(1);
}

int Socket(int domain, int type, int protocol){
    int fd = socket(domain, type, protocol);
    if(fd == -1){
        sys_err("socket create failed");
    }
    return fd;
}

int Setsockopt(int fd, int level, int optname,const void *optval, socklen_t optlen){
    int ret = setsockopt(fd, level, optname, optval, optlen);
    if(ret == -1){
        sys_err("setsockopt failed");
    }
    return ret;
}

int Bind (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len){
    int ret = bind(fd, addr, len);
    if(ret == -1){
        sys_err("bind failed");
    }
    return ret;
}

int Listen(int fd, int backlog){
    int ret = listen(fd, backlog);
    if(ret == -1){
        sys_err("listen failed");
    }
    return ret;
}

int Accept(int fd, __SOCKADDR_ARG addr, socklen_t *len){
    int ret = accept(fd, addr, len);
    if(ret == -1){
        sys_err("accept failed");
    }
    return ret;
}

int Read(int fd, void *buf, size_t count){
    int ret = read(fd, buf, count);
    if(ret == -1){
        sys_err("read failed");
    }
    ((char*)buf)[ret] = '\0';
    return ret;
}

int Epoll_create(int size) {
    int fd = epoll_create(size);
    if (fd == -1) {
        perror("epoll_create");
        exit(-1);
    }
    return fd;
}

void Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) {
    int ret = epoll_ctl(epfd, op, fd, event);
    if (ret == -1) {
        perror("epoll_ctl");
        exit(-1);
    }
}

int Epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    int n = epoll_wait(epfd, events, maxevents, timeout);
    if (n == -1) {
        perror("epoll_wait");
        exit(-1);
    }
    return n;
}

void client_read_cb(int fd, short event, void *arg){
    char buf[BUFSIZ];
    int len = Read(fd, buf, sizeof(buf));
    
    if( len == 0){
        printf("client closed\n");
        close(fd);
        return;
    }
    printf("client read: %s\n", buf);
    write(fd, buf, len);
}

void accept_cb(evutil_socket_t fd, short event, void * arg){
    struct event_base *base = (struct event_base*)arg;
    int client_fd = Accept(fd, NULL, NULL);
    printf("client connected\n");
    struct event *client_event = event_new(base, client_fd, EV_READ | EV_PERSIST, client_read_cb, base);
    event_add(client_event, NULL);
}


int main(){
    // 创建套接字
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);

    // 设置地址重用
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    // 绑定地址结构
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 监听
    Listen(server_fd, 128);

    struct event_base *base = event_base_new();
    if(base == NULL){
        printf("event_base_new failed\n");
        return -1;
    }
    struct event *listen_event = event_new(base, server_fd, EV_READ | EV_PERSIST, accept_cb, base);
    event_add(listen_event, NULL);

    printf("server start\n");
    event_base_dispatch(base);
    event_base_free(base);
    return 0;
}
