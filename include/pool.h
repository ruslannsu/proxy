#include <stdio.h>
#include <pthread.h>


typedef struct thread_pool_t {
    pthread_t *threads;
}thread_pool_t;

thread_pool_t *thread_pool_create();