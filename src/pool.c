#include <pool.h>
#include <logger.h>
#include <errno.h>



thread_pool_t *thread_pool_create() {
    thread_pool_t *thread_pool = malloc(sizeof(thread_pool));
    if (!thread_pool) {
        log_message(FATAL, "THREAD POOL CREATE FAILED. ERRNO: %s", errno);
    }

    thread_pool->threads = malloc(sizeof(pthread_t));
    if (!thread_pool->threads) {
        log_message(FATAL, "THREAD POOL CREATE FAILED. ERRNO: %s", errno);
    }

   // thread_pool->task_queue = malloc(sizeof())
    return thread_pool;
}





