#include "tcp.h"

int epoll_add(int epfd, int fd){
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    return 0;
}

int epoll_del(int epfd, int fd) {
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    return 0;
}

int tcp_init(const char *ip, int port){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int flag = 1;
    int opt_ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ERROR_CHECK(opt_ret, -1, "setsockopt");
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "bind");
    
    listen(sockfd, 50);
    return sockfd;

}



