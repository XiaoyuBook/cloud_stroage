#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H
#include <my_header.h>
#include <time.h>
#include <pthread.h>
extern int epfd;
typedef struct client_connect {
    int                 netfd;
    time_t              last_active;
    char                buf[1024];
    size_t              buf_len;
    struct client_connect *next;
    int is_closed;
} client_connect_t;

extern pthread_mutex_t    client_list_mutex;
extern client_connect_t  *client_list_head;
void add_client_to_list(client_connect_t *client);
client_connect_t *find_client_by_fd(int netfd);
void remove_client(int netfd);
#endif 
