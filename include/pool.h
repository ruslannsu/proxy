#include <stdio.h>
#include <pthread.h>
#include <task_queue.h>

typedef struct thread_pool_t {
    pthread_t *threads;
    task_queue_t *task_queue;
    
}thread_pool_t;

thread_pool_t *thread_pool_create();