#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>


#define SERVER_PORT 8080
#define CLIENT_MAX 10

void sys_err(const char *str){
    perror(str);
    exit(1);
}


struct event_base *Event_base_new(void){
    struct event_base *base = event_base_new();
    if(base == NULL){
        sys_err("event_base_new failed");
        return NULL;
    }
    return base;
}

void client_read_cb(struct bufferevent *bev, void *ctx){
    char buf[BUFSIZ];
    size_t len = bufferevent_read(bev, buf, sizeof(buf));
    buf[len] = '\0';
    printf("client read: %s\n", buf);
    bufferevent_write(bev, buf, len);
}

void event_cb(struct bufferevent *bev, short what, void *ctx){
    if(what & BEV_EVENT_EOF){
        printf("connection closed...\n");
        bufferevent_free(bev);
    }else if(what & BEV_EVENT_ERROR){
        perror("bufferevent_read");
    }
}

void accept_cb(struct evconnlistener *listen, evutil_socket_t fd, struct sockaddr * addr, int socklen, void *arg){
    printf("client connected\n");
    struct event_base *base = evconnlistener_get_base(listen);

    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(bev == NULL){
        perror("bufferevent_socket_new");
        exit(1);
    }
    bufferevent_setcb(bev, client_read_cb, NULL, event_cb, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
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
    if(listener == NULL){
        perror("evconnlistener_new_bind");
        exit(1);
    }

    printf("server start\n");
    event_base_dispatch(base);  // 代替while(1)循环
    event_base_free(base);
    return 0;
}
