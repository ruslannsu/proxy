#include <stdio.h>
#include <pthread.h>
#include <task_queue.h>

#define THREAD_COUNT 100

typedef struct thread_pool_t {
    pthread_t *threads;
    size_t thread_pool_size;
    task_queue_t *task_queue;

}thread_pool_t;

thread_pool_t *thread_pool_create(size_t size);

void thread_pool_run(thread_pool_t *thread_pool);

void thread_poll_destroy(thread_pool_t *thread_popl);