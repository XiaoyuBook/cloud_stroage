#include <my_header.h>
typedef void(*task_fn)(void *arg); 
typedef struct {
    task_fn function;
    void *arg;
}task_t;



typedef struct {
    pthread_t *thread_arr;
    task_t *task_queue;

    int queue_size;
    int queue_head;
    int queue_tail;
    int task_count;
    int thread_count;
    

    bool shutdown;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

}thread_pool_t;


thread_pool_t *thread_pool_create(int thread_count, int queue_size);
int thread_pool_add_task(thread_pool_t *thread_pool, task_fn function, void *arg);
int thread_pool_destroy(thread_pool_t *thread_pool);