#include <task_queue.h>
#include <logger.h>
#include <errno.h>

task_queue_t *task_queue_create() {
    task_queue_t *q = malloc(sizeof(task_queue_t));   
    if (!q) {
        log_message(FATAL, "TASK QUEUE CREATE FAILED. ERRNO: %s", errno);
    }

    q->head = 0;
    q->tail = START_SIZE - 1;
    q->size = 0;
    q->capacity = START_SIZE;

    q->tasks = malloc(sizeof(task_t) * START_SIZE);
    if (!q->tasks) {
        log_message(FATAL, "TASK QUEUE CREATE FAILED. ERRNO: %s", errno);
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
    if (queue->capacity == queue->size) {
        realloc_queue(queue);
    }

    queue->tasks[queue->tail] = task;
    
    ++queue->tail;
    ++queue->size;

    return 0;
}


