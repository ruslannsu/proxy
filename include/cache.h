#define _GNU_SOURCE
#define CACHE_MAX_SIZE 100000000
#include <pthread.h>
#include <glib.h>
#include <time.h>


typedef struct cache_t {
    GHashTable *cache_table;
    size_t cache_size;
    size_t cache_max_size;
    pthread_mutex_t mutex;
} cache_t;

typedef struct cache_content_t {
    char *buffer;
    size_t buffer_size;
    time_t time;
    pthread_rwlock_t lock;
    int destroyed;
}cache_content_t;

cache_t *cache_create(size_t cache_max_size);

void cache_destroy(cache_t *cache);

int cache_add(cache_t *cache, char *url, cache_content_t *cache_content);

cache_content_t *cache_get(cache_t *cache, char *url);

int cache_contains(cache_t *cache, char *url);

cache_content_t *cache_content_create(char *buffer, size_t size);

int cache_place_check(cache_t *cache, size_t buffer_size);

int cache_check_inval(cache_t *cache, char *key);

void cache_remove(cache_t *cache, char *key);