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
#include <pool.h>
#include "../lib/picohttpparser/picohttpparser.h"
#include <poll.h>
#include <task_queue.h>

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

void proxy_run(proxy_t *proxy) {

    log_message(INFO, "PROXY: RUNNING");

    int err;
    struct sockaddr_in addr;
    socklen_t sock_len = sizeof(addr);

    /*
    int sock;
    while (1) {

        sock = accept(proxy->socket, (struct sockaddr*)&addr, &sock_len);
        if (sock < 0) { 
            log_message(ERROR, "PROXY RUNNING: CONNECTION ACCEPT ERR. ERRNO: %s", strerror(errno));
        }
        
        log_message(INFO, "PROXY NEW CONNECTION WITH ADDR: %s, PORT: %d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        
      //  client_t client = {.addr=addr, .socket=sock};
    //    task_t task = {.function=client_task, .args=(void*)&client};



    }
    */
 //   task_t task = {.args = "hello", .function = client_task};

 //   task_queue_add(proxy->thread_pool->task_queue, task);

    

    
    log_message(INFO, "PROXY: STOP RUNNING");
    
    
    
    

    
}

static void client_task(void *args) {
    printf("%s \n" , (char*)args);
}