#include <proxy.h>
#include <stdlib.h>
#include <logger.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../lib/picohttpparser/picohttpparser.h"
#include <assert.h>


proxy_t *proxy_create(int port) {
    int err;

    proxy_t *proxy = malloc(sizeof(proxy_t));
    if (!proxy) {
        log_message(FATAL, "PROXY CREATION: BAD ALLOC. ERRNO: %s", strerror(errno));
    }

    proxy->thread_pool = thread_pool_create();
    if (!proxy->thread_pool) {
        log_message(FATAL, "PROXY CREATION: THREAD POOL CREATION FAILED");
    }
    
    proxy->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy->socket == -1) {
        log_message(ERROR, "PROXY CREATION: CANT CREATE SOCK. ERRNO: %s ", strerror(errno));
        return NULL;
    }

    const int enable = 1;
    err = setsockopt(proxy->socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (err != 0) {
        log_message(FATAL, "PROXY CREATION: CANT SETSOCKOPT. ERRNO: %s", strerror(errno));
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    err = bind(proxy->socket, (struct sockaddr*)&addr, sizeof(addr));
    if (err != 0) {
        log_message(FATAL, "PROXY CREATION: CANT BIND SOCK. ERRNO: %s", strerror(errno));
        return NULL;
    }

    err = listen(proxy->socket, 3);
    if (err < 0) {
        log_message(FATAL, "PROXY DO NOT LISTEN");
        
    }

    log_message(INFO, "PROXY: CREATION COMPLETE");

    return proxy;
}



static void client_task(void *args) {
    int err;
    
    int sock = *(int*)args;
    
    char buf[4096], *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;

    while (1) {
        /* read the request */
        while ((rret = read(sock, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR)
            ;
        if (rret <= 0)
            return;
        prevbuflen = buflen;
        buflen += rret;
        /* parse the request */
        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(buf, buflen, &method, &method_len, &path, &path_len,
                                &minor_version, headers, &num_headers, prevbuflen);
        if (pret > 0)
            break; /* successfully parsed the request */
        else if (pret == -1)
            return;
        /* request is incomplete, continue the loop */
        assert(pret == -2);
        if (buflen == sizeof(buf))
            return;
    }

    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", (int)method_len, method);
    printf("path is %.*s\n", (int)path_len, path);
    printf("HTTP version is 1.%d\n", minor_version);
    printf("headers:\n");
    for (int i = 0; i != num_headers; ++i) {
        printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
            (int)headers[i].value_len, headers[i].value);
    }


    
        
}

void proxy_run(proxy_t *proxy) {
    log_message(INFO, "PROXY: RUNNING");
    int client_sockets[1024];

    int err;
    struct sockaddr_in addr;
    socklen_t sock_len = sizeof(addr);
    thread_pool_run(proxy->thread_pool);

    size_t index = 0;
    while (1) {
        int sock = accept(proxy->socket, (struct sockaddr*)&addr, &sock_len);
        if (sock < 0) { 
            log_message(ERROR, "PROXY RUNNING: CONNECTION ACCEPT ERR. ERRNO: %s", strerror(errno));
        }

        log_message(INFO, "PROXY NEW CONNECTION WITH ADDR: %s, PORT: %d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        client_sockets[index] = sock;

        task_t task = {.args=&(client_sockets[index]), .function=client_task};

        task_queue_add(proxy->thread_pool->task_queue, task);
    
        ++index;
    }

    sleep(10);
    log_message(INFO, "PROXY: STOP RUNNING");
    
}

