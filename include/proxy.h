
typedef struct proxy_t {
    int port;
    void *threads;
}proxy_t;


proxy_t *proxy_create(int port, void *threads);

//TODO:
proxy_t *proxy_destroy(proxy_t proxy);


