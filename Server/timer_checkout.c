// timer_checkout.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "timer_checkout.h"
#include "client_info.h"

#define TIMEOUT_SEC 30

extern pthread_mutex_t    client_list_mutex;
extern client_connect_t  *client_list_head;
void *timer_check_thread(void *arg) {
    (void)arg;
    while (1) {
        sleep(1);
        time_t now = time(NULL);

        pthread_mutex_lock(&client_list_mutex);
        client_connect_t **pp = &client_list_head;
        while (*pp) {
            client_connect_t *cli = *pp;
            if (!cli->is_closed && now - cli->last_active >= TIMEOUT_SEC) {
                printf("客户端 fd=%d 超时 %lds，准备移除\n", cli->netfd, now - cli->last_active);

                epoll_ctl(epfd, EPOLL_CTL_DEL, cli->netfd, NULL);
                close(cli->netfd);
                printf("已关闭客户端 fd=%d\n", cli->netfd);

                *pp = cli->next;  // 删除节点
                free(cli);        // 释放内存
                continue;         // 不移动 pp，因为已经更新
            }
            pp = &cli->next;
        }
        pthread_mutex_unlock(&client_list_mutex);
    }
    return NULL;
}
