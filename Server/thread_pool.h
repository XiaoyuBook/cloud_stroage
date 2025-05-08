#include <my_header.h>
typedef void*(*task_fn)(void *arg); 
typedef struct {
    task_fn function;
    void *arg;
}task_t;

typedef struct task_node_s{
    task_t task;
    struct task_node_s *next;
}task_node_t;



typedef struct {
    pthread_t *thread_arr;
    task_t *task_queue;

    task_node_t *queue_head;
    task_node_t *queue_tail;
    int task_count;
    int thread_count;
    

    bool shutdown;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

}thread_pool_t;


thread_pool_t *thread_pool_create(int thread_count);
int thread_pool_add_task(thread_pool_t *thread_pool, task_fn function, void *arg);
int thread_pool_destroy(thread_pool_t *thread_pool);
