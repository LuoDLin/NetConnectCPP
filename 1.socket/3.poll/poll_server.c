#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/poll.h>

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

int Poll(struct pollfd *fds, nfds_t nfds, int timeout){
    int ret = poll(fds, nfds, timeout);
    if(ret == -1){
        sys_err("poll failed");
    }
    return ret;
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

    // 初始化文件描述符集合
    struct pollfd client_fds[CLIENT_MAX+1]={0};
    client_fds[0].fd = server_fd;
    client_fds[0].events = POLLIN;

    printf("server is running...\n");
    while(1){    
        int ret = Poll(client_fds, CLIENT_MAX+1, -1);
        
        if(client_fds[0].revents & POLLIN ){ // 有新的客户端连接
            printf("new client connect...\n");
            client_fd = Accept(server_fd, NULL, NULL); 
            for(int i = 0 ; i < CLIENT_MAX + 1; i ++ )
                if( client_fds[i].fd == 0 ){
                    client_fds[i].fd = client_fd;
                    client_fds[i].events = POLLIN;
                    break;
                }
        }

        for( int i = 1 ; i <= CLIENT_MAX ; i ++ ){   //接收客户端数据
            if( !client_fds[i].fd || !(client_fds[i].revents & POLLIN) ) // 客户端无数据
                continue;

            int len = Read(client_fds[i].fd, buf, sizeof(buf));
            if(len == 0 ){
                printf("client closed...\n");
                close(client_fds[i].fd);
                client_fds[i].fd = 0;
                client_fds[i].events = 0;
            }else{
                printf("recv buf: %s\n", buf);  // 打印客户端发送的数据
                write(client_fd, buf, len);     // 将数据回写给客户端
            }
        }
    }
    // 关闭套接字
    close(server_fd);
    return 0;
}
