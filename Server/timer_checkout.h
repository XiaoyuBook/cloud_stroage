#include <my_header.h>
#include "client_info.h"
void *timer_check_thread(void *arg);
void add_client_to_list(client_connect_t *client);
void remove_client_from_list(int netfd);
client_connect_t *find_client_by_fd(int netfd);
extern pthread_mutex_t client_list_mutex;
extern client_connect_t *client_list_head;
