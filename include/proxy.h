#include <pool.h>
#include "../lib/picohttpparser/picohttpparser.h"
#include "cache.h"

#define RESPONSE_BUFFER_SIZE 100000000
#define DEFAULT_PORT 8080
#define DEFAULT_THREAD_POOL_SIZE 10
#define RUN 1
#define SHUTDOWN 0
#define UPSTREAM_MODE 0
#define CACHE_MODE 1

typedef struct sockets_t {
    int proxy_socket;
    int client_socket;
    void *proxy;
} sockets_t;

typedef struct proxy_t {
    int socket;
    int mode;
    thread_pool_t *thread_pool;
    cache_t *cache;
    sockets_t pairs[1024];
}proxy_t;

typedef struct http_parse_t {
    char buf[4096];
    char *method; 
    char *path;
    int pret;
    int minor_version;
    struct phr_header headers[100];
    size_t buflen;
    size_t prevbuflen;
    size_t method_len;
    size_t path_len;
    size_t num_headers;
    ssize_t rret;

} http_parse_t;

proxy_t *proxy_create(int port, size_t thread_pool_size, int mode, size_t cache_max_size);

void proxy_run(proxy_t *proxy);

//TODO:
void proxy_destroy(proxy_t *proxy);


