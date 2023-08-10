#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

void sys_err(const char *str){
    perror(str);
    exit(1);
}

int main(){
    // 创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        sys_err("socket create failed\n");
    }

    // 绑定地址结构
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        sys_err("bind failed\n");
    }

    // 监听
    if(listen(server_fd, 128) == -1){
        sys_err("listen failed\n");
    }

    // 接收客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if(client_fd == -1){
        sys_err("accept failed\n");
    }

    char buf[1024];
    while(1){
        // 读取客户端发送的数据 read/recv
        // recv(client_fd, buf, sizeof(buf), 0);
        int len = read(client_fd, buf, sizeof(buf));

        if(len <= 0 ){  // 读取出错
            printf("read error!\n");
            break;
        }else{  // 读取成功
            printf("recv buf: %s\n", buf);  // 打印客户端发送的数据
            write(client_fd, buf, len);     // 将数据回写给客户端
        }
    }

    // 关闭套接字
    close(client_fd);
    close(server_fd);
    return 0;
}
