
#include "cache.h"
#include "logger.h"
#include <errno.h>
#include <stdio.h>

cache_t *cache_create() {
    cache_t *cache = malloc(sizeof(cache_t));
    if (!cache) {
        log_message(FATAL, "CAN NOT CREATE CACHE STRUCT. ERRNO:%s", strerror(errno));
    }

    cache->cache_size = 0;

    cache->cache_table = g_hash_table_new(g_int_hash, g_int_equal);
    if (!cache->cache_table) {
        log_message(FATAL, "CAN NOT CREATE CACHE TABLE. ERRNO:%s", strerror(errno));
    }

    return cache;
}

static cache_content_t *cache_content_create(char *buffer, time_t time) {
    cache_content_t *cache_content = malloc(sizeof(cache_content_t));
    if (!cache_content) {
        log_message(FATAL, "CAN NOT CREATE CACHE TABLE. ERRNO:%s", strerror(errno));
    }

    cache_content->buffer = buffer;
    
    cache_content->time = time;
    
    return cache_content;
}



void cache_destroy(cache_t *cache) {
    //TODO
}

void cache_add(char *buffer) {
    time_t cur_time;
    time(&cur_time);

    cache_content_t *cache_content = cache_content_create(buffer, cur_time);
    //уже проверяется ошибка на память

    
    
}