
typedef struct proxy_t {
    int socket;
    void *thread_pool;
}proxy_t;


proxy_t *proxy_create(int port, void *threads);

//TODO:
proxy_t *proxy_destroy(proxy_t proxy);


