#include <pool.h>

#define RESPONSE_BUFFER_SIZE 100000000



typedef struct proxy_t {
    int socket;
    thread_pool_t *thread_pool;
    thread_pool_t *server_thread_pool;
}proxy_t;

typedef struct sockets_t {
    int *proxy_socket;
    int *client_socket;
} sockets_t;

proxy_t *proxy_create(int port);

void proxy_run(proxy_t *proxy);

//TODO:
proxy_t *proxy_destroy(proxy_t proxy);


