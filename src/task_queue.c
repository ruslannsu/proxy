#include <task_queue.h>
#include <logger.h>
#include <errno.h>
#include <string.h>

//TODO: FIX: POSIX THREADS DO NOT FILL ERRNO!!! ----FIXED(mb)

task_queue_t *task_queue_create() {
    task_queue_t *q = malloc(sizeof(task_queue_t));   
    if (!q) {
        log_message(FATAL, "TASK QUEUE CREATE FAILED. ERRNO: %s", strerror(errno));
    }

    q->head = 0;
    q->tail = 0;
    q->size = 0;
    q->capacity = START_SIZE;

    q->tasks = malloc(sizeof(task_t) * START_SIZE);
    if (!q->tasks) {
        log_message(FATAL, "TASK QUEUE CREATE FAILED. ERRNO: %s", strerror(errno));
    }

    int err;
    
    err = pthread_mutex_init(&q->mutex, NULL);
    if (err != 0) {
        log_message(FATAL, "TASK QUEUE CREATE FAILED. MUTEX INIT FAILED. ERR: %s", strerror(err));
    }

    return q;
}

static void *realloc_queue(task_queue_t *queue) {
    queue->capacity = queue->capacity * REALLOC_COEFF;
    queue->tasks = realloc(queue->tasks, queue->capacity);
    if (!queue->tasks) {
        log_message(FATAL, "QUEUE: REALLOC QUEUE FAILED");
    }
} 

int task_queue_add(task_queue_t *queue, task_t task) {
    int err;

    err = pthread_mutex_lock(&queue->mutex);
    if (err != 0) {
        log_message(FATAL, "QUEUE ADD FAILED: MUTEX LOCK FAILED. ERR: %s", strerror(err));
    }

    if (queue->capacity == queue->size) {
        realloc_queue(queue);
    }

    queue->tasks[queue->tail] = task;
    
    ++queue->tail;
    ++queue->size;

    err = pthread_mutex_unlock(&queue->mutex);
    if (err != 0) {
        log_message(FATAL, "QUEUE ADD FAILED: MUTEX UNLOCK FAILED. ERRN: %s", strerror(err));
    }

    return 0;
}

int queue_is_empty(task_queue_t *queue) {
    return queue->size == 0;
}


task_t task_queue_get(task_queue_t *queue) {
    int err;

    err = pthread_mutex_lock(&queue->mutex);
    if (err != 0) {
        log_message(FATAL, "QUEUE ADD FAILED: MUTEX LOCK FAILED. ERR: %s", strerror(err));
    }

    task_t task;

    if (queue_is_empty(queue)) {
        task.args = NULL;
        task.function = NULL;

        err = pthread_mutex_unlock(&queue->mutex);
        if (err != 0) {
            log_message(FATAL, "QUEUE ADD FAILED: MUTEX UNLOCK FAILED. ERR: %s", strerror(err));
        }
        
        return task;
    }

    task = queue->tasks[queue->head];

    ++queue->head;
    --queue->size;

    err = pthread_mutex_unlock(&queue->mutex);
    if (err != 0) {
        log_message(FATAL, "QUEUE ADD FAILED: MUTEX UNLOCK FAILED. ERR: %s", strerror(err));
    }

    return task;
}