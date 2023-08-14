#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>


#define SERVER_PORT 8080
#define CLIENT_MAX 10

void sys_err(const char *str){
    perror(str);
    exit(1);
}

int Read(int fd, void *buf, size_t count){
    int ret = read(fd, buf, count);
    if(ret == -1){
        sys_err("read failed");
    }
    ((char*)buf)[ret] = '\0';
    return ret;
}

struct event_base *Event_base_new(void){
    struct event_base *base = event_base_new();
    if(base == NULL){
        sys_err("event_base_new failed");
        return NULL;
    }
    return base;
}

void client_read_cb(int fd, short event, void *arg){
    char buf[BUFSIZ];
    int len = Read(fd, buf, sizeof(buf));
    if( len == 0){
        struct event *ev = (struct event*)arg;
        printf("client closed\n");
        event_del(ev);
        event_free(ev);
        close(fd);
        return;
    }
    printf("client read: %s\n", buf);
    write(fd, buf, len);
}

void accept_cb(struct evconnlistener *listen, evutil_socket_t fd, struct sockaddr * addr, int socklen, void *arg){
    printf("client connected\n");
    struct event_base *base = (struct event_base*)arg;

    struct event *client_event = event_new(NULL, -1, 0, NULL, NULL);
    event_assign(client_event, base, fd, EV_READ | EV_PERSIST, client_read_cb, client_event);
    event_add(client_event, NULL);
}

int main(){
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    struct event_base *base = Event_base_new();

    struct evconnlistener * listener = evconnlistener_new_bind(base, accept_cb, base, 
        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 128, 
        (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("server start\n");
    event_base_dispatch(base);  // 代替while(1)循环
    event_base_free(base);
    return 0;
}
