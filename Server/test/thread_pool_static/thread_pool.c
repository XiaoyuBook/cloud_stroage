#include "thread_pool.h"

void *thread_work(void *arg) {
    thread_pool_t *thread_pool = (thread_pool_t*)arg;


    while(1) {
        pthread_mutex_lock(&thread_pool->mutex);


        while(thread_pool->task_count == 0 && !thread_pool->shutdown) {
            pthread_cond_wait(&thread_pool->cond, &thread_pool->mutex);
        }


        if(thread_pool->shutdown && thread_pool->task_count == 0) {
            pthread_mutex_unlock(&thread_pool->mutex);
            pthread_exit(NULL);
        }
        task_t task = thread_pool->task_queue[thread_pool->queue_head];
        thread_pool->queue_head = (thread_pool->queue_head + 1) % thread_pool->queue_size;
        thread_pool->task_count--;
        pthread_mutex_unlock(&thread_pool->mutex);


        task.function(task.arg);
    }
    return NULL;
}

thread_pool_t *thread_pool_create(int thread_count, int queue_size) {
    // 线程相关数据结构初始化
    thread_pool_t *thread_pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    thread_pool->thread_count = thread_count;
    thread_pool->thread_arr = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    thread_pool->shutdown = false;
    // 任务队列相关数据结构初始化
    thread_pool->queue_size = queue_size;
    thread_pool->queue_head = thread_pool->queue_tail = thread_pool->task_count = 0;
    thread_pool->task_queue = (task_t*)malloc(sizeof(task_t)*queue_size);
    
    // 锁相关初始化
    pthread_mutex_init(&thread_pool->mutex, NULL);
    pthread_cond_init(&thread_pool->cond, NULL);

    for(int i = 0; i < thread_count; i++) {
        pthread_create(&thread_pool->thread_arr[i], NULL, thread_work, thread_pool);
    }
    return thread_pool;
}



int thread_pool_add_task(thread_pool_t *thread_pool, task_fn function, void *arg) {
    pthread_mutex_lock(&thread_pool->mutex);
    
    // 如果任务队列满载，则退出
    if(thread_pool->task_count == thread_pool->queue_size) {
        pthread_mutex_unlock(&thread_pool->mutex);
        return -1;
    }


    int idx = thread_pool->queue_tail;
    thread_pool->task_queue[idx].function = function;
    thread_pool->task_queue[idx].arg = arg;
    thread_pool->queue_tail = (idx+1) % thread_pool->queue_size;
    thread_pool->task_count++;

    pthread_cond_signal(&thread_pool->cond);
    pthread_mutex_unlock(&thread_pool->mutex);
    return 0;
}



int thread_pool_destroy(thread_pool_t *thread_pool) {
    pthread_mutex_lock(&thread_pool->mutex);

    thread_pool->shutdown = true;
    pthread_cond_broadcast(&thread_pool->cond);
    pthread_mutex_unlock(&thread_pool->mutex);


    for(int i = 0; i < thread_pool->thread_count; i++) {
        pthread_join(thread_pool->thread_arr[i], NULL);
    }
    

    free(thread_pool->thread_arr);
    free(thread_pool->task_queue);
    pthread_mutex_destroy(&thread_pool->mutex);
    pthread_cond_destroy(&thread_pool->cond);
    free(thread_pool);

    return 0;
}