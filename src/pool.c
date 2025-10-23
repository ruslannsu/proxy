#include <pool.h>
#include <logger.h>
#include <errno.h>
#include <string.h>



thread_pool_t *thread_pool_create() {
    thread_pool_t *thread_pool = malloc(sizeof(thread_pool_t));
    if (!thread_pool) {
        log_message(FATAL, "THREAD POOL CREATE FAILED. ERRNO: %s", strerror(errno));
    }

    thread_pool->threads = malloc(sizeof(pthread_t) * THREAD_COUNT);
    if (!thread_pool->threads) {
        log_message(FATAL, "THREAD POOL CREATE FAILED. ERRNO: %s", strerror(errno));
    }

    thread_pool->task_queue = task_queue_create();
    if (!thread_pool->task_queue) {
        exit(3);
    }
    
    return thread_pool;
}


static void *reader_routine(void *args) {
    task_queue_t *queue = (task_queue_t*)(args);
    while (1) {   
        
        task_t task = task_queue_get(queue);
        
        if ((task.args == NULL) && (task.function == NULL))  {
            continue;
        }

        task.function(task.args);
            
    
        //sleep(3);
    }
}

void thread_pool_run(thread_pool_t *thread_pool) {
    int err;
    for (size_t i = 0; i < 5; ++i) {
        err = pthread_create(&thread_pool->threads[i], NULL, reader_routine, thread_pool->task_queue);
        if (err != 0) { 
            log_message(FATAL, "THREAD POOL RUN FAILED", strerror(err));
        }
    }
}









