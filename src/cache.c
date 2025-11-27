#include "cache.h"
#include "logger.h"
#include <errno.h>
#include <stdio.h>

cache_t *cache_create() {
    int err;

    cache_t *cache = malloc(sizeof(cache_t));
    if (!cache) {
        log_message(FATAL, "CAN NOT CREATE CACHE STRUCT. ERRNO:%s", strerror(errno));
    }

    cache->cache_size = 0;

    err = pthread_mutex_init(&cache->mutex, PTHREAD_PROCESS_PRIVATE);
    if (err != 0) {
        log_message(FATAL, "cant init mutex cache");
    }
    
    cache->cache_table = g_hash_table_new(g_str_hash, g_str_equal);
    if (!cache->cache_table) {
        log_message(FATAL, "CAN NOT CREATE CACHE TABLE. ERRNO:%s", strerror(errno));
    }

    return cache;
}

cache_content_t *cache_content_create(char *buffer, size_t size) {
    int err;

    cache_content_t *cache_content = malloc(sizeof(cache_content_t));
    if (!cache_content) {
        log_message(FATAL, "CAN NOT CREATE CACHE TABLE. ERRNO:%s", strerror(errno));
    }

    err = pthread_rwlock_init(&cache_content->lock, NULL);
    if (err != 0) {
        log_message(FATAL, "rw lock fatal");
    }
    
    time_t cur_time;
    time(&cur_time);

    cache_content->time = cur_time;
    cache_content->buffer = buffer;
    cache_content->buffer_size = size;

    return cache_content;
}

void cache_destroy(cache_t *cache) {
    //TODO: мапа отдает список
}

cache_content_t *cache_get(cache_t *cache, char *url) {
    return (cache_content_t*)g_hash_table_lookup(cache->cache_table, url);
}


int cache_contains(cache_t *cache, char *url) {
    return g_hash_table_contains(cache->cache_table, url);
}

//TODO
int cache_add(cache_t *cache, char *url, cache_content_t *cache_content) {
    g_hash_table_insert(cache->cache_table, url, cache_content);
    cache->cache_size += cache_content->buffer_size;

    return 0;
}