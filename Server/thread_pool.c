#include "thread_pool.h"

void *thread_work(void *arg) {
    thread_pool_t *thread_pool = (thread_pool_t*)arg;


    while(1) {
        pthread_mutex_lock(&thread_pool->mutex);


        while(thread_pool->queue_head == NULL && !thread_pool->shutdown) {
            pthread_cond_wait(&thread_pool->cond, &thread_pool->mutex);
        }


        if(thread_pool->shutdown && thread_pool->queue_head == NULL) {
            pthread_mutex_unlock(&thread_pool->mutex);
            pthread_exit(NULL);
        }
        task_node_t *node = thread_pool->queue_head;
        if(node != NULL) {
            thread_pool->queue_head = node->next;
            if(thread_pool->queue_head == NULL){
                thread_pool->queue_tail = NULL;
            }
        }
        pthread_mutex_unlock(&thread_pool->mutex);
        
        if(node) {
            node->task.function(node->task.arg);
        }

    }
    return NULL;
}

thread_pool_t *thread_pool_create(int thread_count ) {
    // 线程相关数据结构初始化
    thread_pool_t *thread_pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    thread_pool->thread_count = thread_count;
    thread_pool->thread_arr = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    thread_pool->shutdown = false;
    // 任务队列相关数据结构初始化
    thread_pool->queue_head = NULL;
    thread_pool->queue_tail = NULL;
    
    // 锁相关初始化
    pthread_mutex_init(&thread_pool->mutex, NULL);
    pthread_cond_init(&thread_pool->cond, NULL);

    for(int i = 0; i < thread_count; i++) {
        pthread_create(&thread_pool->thread_arr[i], NULL, thread_work, thread_pool);
    }
    return thread_pool;
}



int thread_pool_add_task(thread_pool_t *thread_pool, task_fn function, void *arg) {
    task_node_t *new_node = (task_node_t*)malloc(sizeof(task_node_t));
    new_node->task.function = function;
    new_node->task.arg = arg;
    new_node->next = NULL;
    pthread_mutex_lock(&thread_pool->mutex);


    if(thread_pool->queue_tail == NULL) {
        thread_pool->queue_head = thread_pool->queue_tail = new_node;
    }else {
        thread_pool->queue_tail->next = new_node;
        thread_pool->queue_tail = new_node;
    }
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
    
    task_node_t *node = thread_pool->queue_head;
    while(node) {
        task_node_t *temp = node;
        node = node->next;
        free(temp);
    }
    free(thread_pool->thread_arr);
    pthread_mutex_destroy(&thread_pool->mutex);
    pthread_cond_destroy(&thread_pool->cond);
    free(thread_pool);

    return 0;
}
