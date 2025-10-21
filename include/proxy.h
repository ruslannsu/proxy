typedef struct proxy_t {
    int socket;
    void *thread_pool;
}proxy_t;

proxy_t *proxy_create(int port, void *threads);

void proxy_run(proxy_t *proxy);

//TODO:
proxy_t *proxy_destroy(proxy_t proxy);


