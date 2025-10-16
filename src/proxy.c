#include <proxy.h>
#include <stdlib.h>
#include <logger.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>




proxy_t *proxy_create(int port, void *threads) {
    int err;

    proxy_t *proxy = malloc(sizeof(proxy_t));
    if (!proxy) {
        log(FATAL, "PROXY CREATION: BAD ALLOC");
    }

    proxy->thread_pool = threads;

    proxy->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy->socket == -1) {
        log(ERROR, "PROXY CREATION: CANT CREATE SOCK");
        return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htonl(port);
    
    err = bind(proxy->socket, (struct sockaddr*)&addr, sizeof(addr));
    if (err != 0) {
        log(ERROR, "PROXY CREATION: CANT BIND SOCK");
        return NULL;
    }

    

    log(INFO, "PROXY: CREATION COMPLETE");

    return proxy;
}