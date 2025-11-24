#include <glib.h>
#include <time.h>

#define CACHE_MAX_SIZE 100000000


typedef struct cache_t {
    GHashTable *cache_table;
    size_t cache_size;
} cache_t;

typedef struct cache_content_t {
    char *buffer;
    time_t time;
}cache_content_t;



cache_t *cache_create();

void cache_destroy(cache_t *cache);

int cache_add(cache_t *cache, char *url, char *buffer, size_t buffer_size);