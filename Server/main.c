#include "thread_pool.h"
#include "get_conf.h"
#include "tcp.h"
#include "client_info.h"
#include "timer_checkout.h"
#include "command.h"
#define THREAD_NUM 10
int epfd;
int exit_pipe[2];
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;
client_connect_t *client_list_head = NULL;

void handler(int signum) {
    printf("father got signal, signum = %d\n", signum);
    write(exit_pipe[1], "1", 1); 
}

// 工作线程函数（暂时空）

int main(int argc, char *argv[]) {
    ARGS_CHECK(argc, 2);

    const char *config_path = argv[1];
    server_config config;

    if (get_config(config_path, &config) != 0) {
        fprintf(stderr, "读取配置失败，程序退出\n");
        return -1;
    }

    printf("IP: %s\n", config.ip);
    printf("Port: %d\n", config.port);
    printf("Worker Num: %d\n", config.worker_num);

    pipe(exit_pipe);
    if (fork()) {
        close(exit_pipe[0]);  // 父进程关闭读端
        signal(SIGUSR1, handler);
        wait(NULL);  // 等待子进程退出
        printf("process will close!\n");
        exit(0);
    }

    close(exit_pipe[1]);  // 子进程关闭写端
    pthread_t time_thread;
    pthread_create(&time_thread, NULL, timer_check_thread, NULL);
    thread_pool_t *thread_pool = thread_pool_create(config.worker_num);
    int sockfd = tcp_init(config.ip, config.port);
    int epfd = epoll_create(1);
    epoll_add(epfd, sockfd);
    epoll_add(epfd, exit_pipe[0]);

    struct epoll_event ready_set[1024];
    int should_exit = 0;

    while (!should_exit) {
    int ready_num = epoll_wait(epfd, ready_set, 1024, -1);
    for (int i = 0; i < ready_num; i++) {
        int fd = ready_set[i].data.fd;

        if (fd == sockfd) {
            client_connect_t *client = (client_connect_t*)malloc(sizeof(client_connect_t));
            client->netfd = accept(sockfd, NULL, NULL);
            fcntl(client->netfd, F_SETFL, fcntl(client->netfd, F_GETFL) | O_NONBLOCK);
            client->last_active = time(NULL);
            client->buf_len = 0;
            client->is_closed = 0;

            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = client->netfd;
            epoll_ctl(epfd, EPOLL_CTL_ADD, client->netfd, &ev);

            add_client_to_list(client);
            printf("新客户端接入：fd=%d\n", client->netfd);
        } else if (fd == exit_pipe[0]) {
            printf("收到退出信号，准备关闭线程池\n");
            thread_pool_destroy(thread_pool);
            should_exit = 1;
            break;
        } else {
            client_connect_t *client = find_client_by_fd(fd);
            if (client) {
                thread_pool_add_task(thread_pool, command, client);
            } else {
                printf("未找到相应客户端!\n");
                close(fd);
            }
        }
    }
}
    close(sockfd);
    close(epfd);
    printf("主进程退出完成\n");
    return 0;
}
