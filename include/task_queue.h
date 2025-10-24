#include <stdio.h>
#include <pthread.h>

#define MAX_SIZE 1000000000
#define START_SIZE 5
#define REBALANCE_POINT 0.7
#define REALLOC_COEFF 2;

typedef struct task_t {
    void (*function)(void*);
    void *args;
}task_t;


typedef struct task_queue_t {
    size_t head;
    size_t tail;
    size_t size;
    size_t capacity;
    task_t *tasks;
    pthread_mutex_t mutex;
}task_queue_t;

task_queue_t *task_queue_create();

int task_queue_add(task_queue_t *queue, task_t task);

task_t task_queue_get(task_queue_t *queue);

int queue_is_empty(task_queue_t *queue);
