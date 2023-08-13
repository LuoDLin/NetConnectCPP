#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

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

int Connect(int fd, __CONST_SOCKADDR_ARG addr, socklen_t len){
    int ret = connect(fd, addr, len);
    if(ret == -1){
        sys_err("connect failed");
    }
    return ret;
}

int main(){
    // 创建套接字
    int client_fd = Socket(AF_INET, SOCK_STREAM, 0);

    // 连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);
    Connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 与服务器通信
    char buf[1024];
    while(1){

        printf("input: ");
        scanf("%s", buf);

        if( !strcmp(buf, "exit") ){
            break;
        }

        // 将用户输入的数据发送给服务器
        write(client_fd, buf, sizeof(buf));

        // 读取服务器回写的数据
        int read_len = Read(client_fd, buf, sizeof(buf));
        if(read_len == 0){
            printf("server closed...\n");
            break;
        }
        
        printf("recv buf: %s\n", buf);
    }

    // 关闭套接字
    close(client_fd);
    return 0;
}