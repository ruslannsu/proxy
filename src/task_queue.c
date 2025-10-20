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