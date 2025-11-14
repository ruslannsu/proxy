#include <glib.h>

#define CACHE_MAX_SIZE 100000000


typedef struct cache_t {
    GHashTable *cache;
    size_t cache_size;
} cache_t;

cache_t *cache_create();

void cache_destroy(cache_t *cache);

