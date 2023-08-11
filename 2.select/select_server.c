#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define SERVER_PORT 8080
#define CLIENT_MAX 1024

void sys_err(const char *str){
    perror(str);
    exit(1);
}

int main(){
    // 创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
        sys_err("socket create failed");
    }

    // 设置地址重用
    int optval = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
        sys_err("setsockopt failed");
    }

    // 绑定地址结构
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        sys_err("bind failed");
    }

    // 监听
    if(listen(server_fd, 128) == -1){
        sys_err("listen failed");
    }

    // 初始化文件描述符集合
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);

    int client_fds[CLIENT_MAX] = {0}; // 保存客户端的文件描述符
    int client_num = 0;               // 客户端的数量
    int maxfd = server_fd;  // 保存最大的文件描述符

    printf("server is running...\n");
    struct sockaddr_in client_addr;
    int client_fd = -1, client_addr_len = sizeof(client_addr);
    // 循环监听事件
    while(1){
        // 监听文件描述符集合
        fd_set tmpfds = readfds;
        int ret = select(maxfd + 1, &tmpfds, NULL, NULL, NULL);
        if(ret == -1){      // 出错
            sys_err("select failed ");
        }else if(ret == 0){ // 超时
            printf("time out\n");
            continue;
        }
        
        // 判断是否有新的连接
        if(FD_ISSET(server_fd,&tmpfds) ){
            // 接收客户端连接
            if((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len)) < 0){
                sys_err("accept failed");
            }

            // 打印客户端的 ip 和 port
            printf("client ip: %s, port: %d, client_fd:%d\n", 
                inet_ntoa(client_addr.sin_addr), 
                ntohs(client_addr.sin_port),
                client_fd);

            // 将新的文件描述符加入到文件描述符集合中
            FD_SET(client_fd, &readfds);
            client_num ++;  // 客户端数量加一

            for(int i = 0 ; i < CLIENT_MAX ; i ++ ){
                if( client_fds[i] == 0 ){
                    client_fds[i] = client_fd;
                    break;
                }
            }
            // 更新最大的文件描述符
            maxfd = maxfd > client_fd ? maxfd : client_fd;
            ret --; // 待处理的事件减一 
        }

        char buf[1024];
        for( int i = 0; ret > 0 ; i ++ ){
            if( FD_ISSET(client_fds[i],&tmpfds) ){
                ret --; // 待处理的事件减一
                int read_len = read(client_fds[i], buf, sizeof(buf));

                if(read_len == -1 ){  // 读取出错
                    sys_err("read failed");
                }else if(read_len == 0 ){
                    printf("client closed...\n");
                    // 维护文件描述符信息
                    int fd = client_fds[i];
                    client_num --;  
                    client_fds[i] = 0;
                    close(fd);
                    FD_CLR(fd, &readfds);
                    
                    // 更新最大的文件描述符
                    if( fd == maxfd ){ 
                        maxfd = server_fd;
                        for(int j = 0 , k = client_num; k ; j ++ ){
                            if( client_fds[j] != 0 ){
                                maxfd = (maxfd>client_fds[j])?maxfd:client_fds[j];
                                k --;
                            }
                        }
                    }
                    continue;
                }
                buf[read_len] = '\0';
                printf("fd:%d recv buf: %s\n",client_fds[i], buf);  // 打印客户端发送的数据
                write(client_fds[i], buf, read_len);     // 将数据回写给客户端

            }
        }
    }


    // 关闭套接字
    close(server_fd);
    return 0;
}
