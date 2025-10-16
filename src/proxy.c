#include <proxy.h>
#include <stdlib.h>
#include <logger.h>

proxy_t *proxy_create(int port, void *threads) {
    proxy_t *proxy = malloc(sizeof(proxy_t));
    log(FATAL, "PROXY: BAD ALLOC");
    if (!proxy) {
        log(FATAL, "PROXY: BAD ALLOC");
    }

    proxy->port = port;
    proxy->threads = threads;

    return proxy;
}