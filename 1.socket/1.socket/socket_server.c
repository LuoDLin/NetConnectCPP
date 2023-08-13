#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080

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

int main(){
    // 创建套接字
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);

    // 设置地址重用
    int optval = 1;
    Setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    // 绑定地址结构
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 监听
    Listen(server_fd, 128);

    printf("server is running...\n");
    while(1){
        int client_fd = Accept(server_fd, NULL, NULL);
        printf("client connected...\n");
        char buf[1024];
        while(1){
            // 读取客户端发送的数据 read/recv
            // recv(client_fd, buf, sizeof(buf), 0);
            int len = Read(client_fd, buf, sizeof(buf));

           if(len == 0 ){
                printf("client closed...\n");
                break;
            }else{
                printf("recv buf: %s\n", buf);  // 打印客户端发送的数据
                write(client_fd, buf, len);     // 将数据回写给客户端
            }
        }
        close(client_fd);
    }

    // 关闭套接字
    close(server_fd);
    return 0;
}
