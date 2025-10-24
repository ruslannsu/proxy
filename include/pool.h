#include <stdio.h>
#include <pthread.h>
#include <task_queue.h>

#define THREAD_COUNT 10

typedef struct thread_pool_t {
    pthread_t *threads;
    task_queue_t *task_queue;
    
}thread_pool_t;

thread_pool_t *thread_pool_create();

void thread_pool_run(thread_pool_t *thread_pool);