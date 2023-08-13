#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>

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



int main(){
    char buf[1024];
    int optval = 1,server_fd,client_fd;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    server_fd = Socket(AF_INET, SOCK_STREAM, 0);// 创建套接字
    Setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));// 设置地址重用
    Bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));// 绑定地址结构
    Listen(server_fd, 128);// 监听

    int epfd = Epoll_create(CLIENT_MAX);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    Epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &event);

    struct epoll_event events[CLIENT_MAX + 1];  // 用于接收epoll_wait返回的事件
    printf("server is running...\n");
    while(1){
        int n = Epoll_wait(epfd, events, CLIENT_MAX + 1, -1);
        
        for( int i = 0 ; i < n ; i ++ ){
            if( events[i].data.fd == server_fd ){ // 有新的客户端连接
                printf("new client connect...\n");
                int client_fd = Accept(server_fd, NULL, NULL); 
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                Epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
            }else{
                int len = Read(events[i].data.fd, buf, sizeof(buf));
                if(len == 0 ){
                    // Epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd); // close 之后 epoll 会自动删除
                    printf("client closed...\n");
                }else{
                    printf("recv buf: %s\n", buf);  // 打印客户端发送的数据
                    write(client_fd, buf, len);     // 将数据回写给客户端
                }
            }
        }
    }
    // 关闭套接字
    close(server_fd);
    return 0;
}
