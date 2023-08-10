#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

int main(){
    // 创建套接字
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);
    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 与服务器通信
    char buf[1024];
    while(1){
        // 读取用户输入
        scanf("%s", buf);

        if( !strcmp(buf, "exit") ){
            break;
        }

        // 将用户输入的数据发送给服务器
        write(client_fd, buf, sizeof(buf));

        // 读取服务器回写的数据
        int read_len = read(client_fd, buf, sizeof(buf));
        buf[read_len] = '\0';

        // 打印服务器回写的数据
        printf("recv buf: %s\n", buf);
    }

    // 关闭套接字
    close(client_fd);
    return 0;
}