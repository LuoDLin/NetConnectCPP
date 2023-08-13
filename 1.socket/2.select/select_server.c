#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>

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

int Select(int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout){
    int ret = select(nfds, readfds, writefds, exceptfds, timeout);
    if(ret == -1){
        sys_err("select failed");
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

    // 初始化文件描述符集合
    fd_set readfds;
    int client_fds[CLIENT_MAX] = {0};

    printf("server is running...\n");
   
    // 循环监听事件
    while(1){
        
        // 每次循环都要重新设置文件描述符集合
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        for( int i = 0 ; i < CLIENT_MAX ; i ++ )
            if( client_fds[i] != 0 )
                FD_SET(client_fds[i], &readfds);
        
        int ret = Select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
        
        if(FD_ISSET(server_fd,&readfds) ){ // 有新的客户端连接
            printf("new client connect...\n");
            int client_fd = Accept(server_fd, NULL, NULL); 
            for(int i = 0 ; i < CLIENT_MAX ; i ++ )
                if( client_fds[i] == 0 ){
                    client_fds[i] = client_fd;
                    break;
                }
        }

        char buf[1024];
        for( int i = 0 ; i < CLIENT_MAX ; i ++ ){   //接收客户端数据
            if( !client_fds[i] || !FD_ISSET(client_fds[i],&readfds) ) // 客户端无数据
                continue;
            
            int read_len = Read(client_fds[i], buf, sizeof(buf));
            if(read_len == 0 ){
                printf("client closed...\n");
                close(client_fds[i]);
                client_fds[i] = 0;
            }else{
                printf("fd:%d recv buf: %s\n",client_fds[i], buf); 
                write(client_fds[i], buf, read_len);
            }
            
        }
    }
    // 关闭套接字
    close(server_fd);
    return 0;
}
