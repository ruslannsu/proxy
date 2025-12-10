#include "cache.h"
#include "logger.h"
#include <errno.h>
#include <stdio.h>

pthread_mutex_t alive_lock = PTHREAD_MUTEX_INITIALIZER;

cache_t *cache_create(size_t cache_max_size) {
    int err;

    cache_t *cache = malloc(sizeof(cache_t));
    if (!cache) {
        log_message(FATAL, "CAN NOT CREATE CACHE STRUCT. ERRNO:%s", strerror(errno));
    }

    cache->cache_size = 0;

    cache->cache_max_size = cache_max_size;

    err = pthread_mutex_init(&cache->mutex, PTHREAD_PROCESS_PRIVATE);
    if (err != 0) {
        log_message(FATAL, "cant init mutex cache");
    }

    err = pthread_mutex_init(&cache->alive_lock, NULL);
    if (err != 0) {
        log_message(FATAL, "alive lock fatal");
    }
    
    cache->cache_table = g_hash_table_new_full(
        g_str_hash,                        
        g_str_equal,                       
        free, NULL                           
    );
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

    cache_content->alive_lock = &alive_lock;
    
    time_t cur_time;
    time(&cur_time);

    cache_content->time = cur_time;
    cache_content->buffer = buffer;
    cache_content->buffer_size = size;
    cache_content->destroyed = 0;

    return cache_content;
}

void cache_content_destroy(cache_content_t *cache_content) {
    free(cache_content->buffer);
    free(cache_content);
}


int cache_check_inval(cache_t *cache, char *key) {
    cache_content_t *cache_content = (cache_content_t*)(cache_get(cache, key));

    time_t cur_time;
    time(&cur_time);
    if ((cur_time - cache_content->time)  > 5) {
        return 1;
    } 

    return 0;
}

void cache_remove(cache_t *cache, char *key) {
    cache_content_t *cache_content = (cache_content_t*)(cache_get(cache, key));
    free(cache_content->buffer);
    free(cache_content);
    g_hash_table_steal(cache->cache_table, key);
    
}


void cache_cleaner(cache_t *cache) {
    GList *keys = g_hash_table_get_keys(cache->cache_table);
    GList *node = keys;
    size_t cache_size = 0;
    
    while (node != NULL) {
        if (node->data != NULL) {
            char *key = (char*)node->data;
            cache_content_t *cache_content = g_hash_table_lookup(cache->cache_table, key);
            
            if (cache_content != NULL) {
                cache_content_destroy(cache_content);
            }
        }
        node = node->next;
    }
    
    g_list_free(keys); 
    return cache_size;
}



cache_content_t *cache_get(cache_t *cache, char *url) {
    return (cache_content_t*)g_hash_table_lookup(cache->cache_table, url);
}

int cache_contains(cache_t *cache, char *url) {
    //cache_cleaner(cache);
    return g_hash_table_contains(cache->cache_table, url);
    
}

static size_t cache_size_get(cache_t *cache) {
    GList *keys = g_hash_table_get_keys(cache->cache_table);
    GList *node = keys;
    size_t cache_size = 0;
    
    while (node != NULL) {
        if (node->data != NULL) {
            char *key = (char*)node->data;
            cache_content_t *cache_content = g_hash_table_lookup(cache->cache_table, key);
            
            if (cache_content != NULL) {
                cache_size += cache_content->buffer_size;
            }
        }
        node = node->next;
    }
    
    g_list_free(keys); 
    return cache_size;
}

int cache_place_check(cache_t *cache, size_t buffer_size) {
    
    int err;
    
    err = pthread_mutex_lock(&cache->mutex);
    if (err != 0) {
        log_message(FATAL, "cache unlock mutex fail");
    }
    size_t cache_size = cache_size_get(cache);

    printf("%d %s\n", cache_size, "CACHE");
    
    if (buffer_size > cache->cache_max_size) {
        printf("\n%d %s \n", buffer_size, "buffer");
        printf("\n%d %s \n", cache->cache_max_size, "max");

        err = pthread_mutex_unlock(&cache->mutex);
        if (err != 0) {
            log_message(FATAL, "cache unlock mutex fail");
        }
        return -1;
    }

    err = pthread_mutex_unlock(&cache->mutex);
    if (err != 0) {
        log_message(FATAL, "cache unlock mutex fail");
    }
}


//TODO
int cache_add(cache_t *cache, char *url, cache_content_t *cache_content) {
    g_hash_table_insert(cache->cache_table, url, cache_content);
    
    return 0;
}

void cache_destroy(cache_t *cache) {
    cache_cleaner(cache);
    g_hash_table_destroy(cache->cache_table);
    free(cache);
}