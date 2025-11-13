#include <pool.h>
#include "../lib/picohttpparser/picohttpparser.h"

#define RESPONSE_BUFFER_SIZE 100000000
#define DEFAULT_PORT 8080
#define DEFAULT_THREAD_POOL_SIZE 10
#define RUN 1
#define SHUTDOWN 0

typedef struct proxy_t {
    int socket;
    thread_pool_t *thread_pool;
}proxy_t;

typedef struct sockets_t {
    int *proxy_socket;
    int *client_socket;
} sockets_t;

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



proxy_t *proxy_create(int port);

void proxy_run(proxy_t *proxy);

//TODO:
void proxy_destroy(proxy_t *proxy);


