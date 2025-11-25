#define _GNU_SOURCE
#include <glib.h>
#include <time.h>
#include <pthread.h>

#define CACHE_MAX_SIZE 100000000


typedef struct cache_t {
    GHashTable *cache_table;
    size_t cache_size;
    pthread_mutex_t mutex;
} cache_t;

typedef struct cache_content_t {
    char *buffer;
    size_t buffer_size;
    time_t time;
    pthread_rwlock_t lock;
}cache_content_t;

cache_t *cache_create();

void cache_destroy(cache_t *cache);

int cache_add(cache_t *cache, char *url, cache_content_t *cache_content);

cache_content_t *cache_get(cache_t *cache, char *url);

int cache_contains(cache_t *cache, char *url);

cache_content_t *cache_content_create(char *buffer, size_t size);