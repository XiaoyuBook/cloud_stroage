#include <my_header.h>
#include "thread_pool.h"


void test_task(void *arg) {
    int num = *(int*)arg;
    printf("Task %d is running in the thread%ld\n", num, pthread_self());
    sleep(1);
    free(arg);
}
int main(int argc, char *argv[]){  
    int thread_num = 4;
    thread_pool_t *thread_pool = thread_pool_create(thread_num);
    for(int i = 0; i < 20; i++) {
        int *arg = (int*)malloc(sizeof(int));
        *arg = i+1;
        if(thread_pool_add_task(thread_pool, test_task, arg) != 0){
            printf("Failed to add task %d (queue full)\n", *arg);
            free(arg);
        }
    }
    sleep(6);
    thread_pool_destroy(thread_pool);
    printf("Thread pool destroyed.\n");


    return 0;
}

