#include <proxy.h>
#include <stdlib.h>

proxy_t *proxy_create(int port, void *threads) {
    proxy_t *proxy = malloc(sizeof(proxy_t));
    if (!proxy) {
        return NULL;
    }

    proxy->port = port;
    proxy->threads = threads;

    return proxy;
}