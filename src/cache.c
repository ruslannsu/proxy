#include "cache.h"
#include "logger.h"
#include <errno.h>
#include <stdio.h>

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

void cache_content_destroy(cache_content_t *cache_content) {
    free(cache_content->buffer);
    free(cache_content);
}


void cache_cleaner(cache_t *cache) {
    int err;


    printf("he-----");
    fflush(stdout);
    GList *node = g_hash_table_get_keys(cache->cache_table);
    time_t cur_time;
    time(&cur_time);
    printf("he");
    fflush(stdout);
    
    while (node != NULL) {
        if (!node->data) {
            node = node->next;
            break;
        }
        printf("%s\n", (char*)node->data);
        fflush(stdout);
        char *key = (char*)node->data;
        break;
        cache_content_t *cache_content = g_hash_table_lookup(cache->cache_table, key);

        if (abs(cache_content->time - cur_time) > 10) {
            cache_content_destroy(cache_content);
            g_hash_table_remove(cache->cache_table, key);
        }
        
        node = node->next;
    }
}


void cache_destroy(cache_t *cache) {
    //TODO: мапа отдает список
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
    printf("%d %s \n", cache_size_get(cache), "cache size sum");

     

    if (buffer_size > cache->cache_max_size) {
        return -1;
    }

    /*
    if (buffer_size + cache->cache_size > cache->cache_max_size) {
        cache_cleaner(cache);
        if (buffer_size + cache->cache_size > cache->cache_max_size) {
            return -1;
        }
    }
    */

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