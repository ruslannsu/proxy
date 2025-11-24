
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

    cache->cache_table = g_hash_table_new(g_str_hash, g_str_equal);
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


int cache_contains(cache_t *cache, char *url) {
    return g_hash_table_contains(cache->cache_table, url);
}

int cache_add(cache_t *cache, char *url, char *buffer, size_t buffer_size) {
    time_t cur_time;
    time(&cur_time);
    //возможно тут можно не ходить на кучу, а просто структурку в хэш закидывать, но я хз как работать будет, так что пока что нет
    cache_content_t *cache_content = cache_content_create(buffer, cur_time);
    //уже проверяется ошибка на память
    g_hash_table_insert(cache->cache_table, url, cache_content);

    cache->cache_size += buffer_size;

    return 0;
}