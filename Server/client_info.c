#include "client_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>


void add_client_to_list(client_connect_t *client) {
    pthread_mutex_lock(&client_list_mutex);
    client->next = client_list_head;
    client_list_head = client;
    pthread_mutex_unlock(&client_list_mutex);
}

client_connect_t *find_client_by_fd(int netfd) {
    pthread_mutex_lock(&client_list_mutex);
    client_connect_t *cur = client_list_head;
    while (cur) {
        if (cur->netfd == netfd) {
            pthread_mutex_unlock(&client_list_mutex);
            return cur;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
    return NULL;
}
void remove_client(int fd) {
    client_connect_t *prev = NULL, *cur = client_list_head;
    while (cur) {
        if (cur->netfd == fd) {
            if (cur->is_closed) return;
            cur->is_closed = 1;

            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
            printf("已关闭客户端 fd=%d\n", fd);
            close(fd);

            if (prev) {
                prev->next = cur->next;
            } else {
                client_list_head = cur->next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
