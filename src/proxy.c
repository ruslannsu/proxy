#define _POSIX_C_SOURCE 200112L
#include <proxy.h>
#include <stdlib.h>
#include <logger.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../lib/picohttpparser/picohttpparser.h"
#include <assert.h>
#include <unistd.h>
#include <netdb.h>  


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

    proxy->server_thread_pool = thread_pool_create();
    if (!proxy->server_thread_pool) {
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



static void upstream_connection_create(int client_socket, char *ip) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    int port = 80;
    char *ip_ = "213.109.147.119";
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_);
    
    int err;

    err = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (err != 1) {
        printf("%s %s  \n", "connected to upstream", strerror(errno));
        fflush(stdout);
    }
    
}


void resolve_hostname(const char *hostname, size_t len, char ip[]) {
    char clean[256];
    size_t actual_len = len;
    
    if (len == 0) {
        actual_len = strlen(hostname);
    }
    
    size_t i;
    
    for (i = 0; i < actual_len && i < sizeof(clean) - 1; ++i) {
        if (hostname[i] >= ' ' && hostname[i] <= '~') { 
            clean[i] = hostname[i];
        } 
        else {
            break; 
        }
    }
    clean[i] = '\0';
    
    while (i > 0 && (clean[i-1] == ' ' || clean[i-1] == '\t')) {
        clean[--i] = '\0';
    }
    
    printf("Resolving: '%s' (original length: %zu, clean length: %zu)\n", 
           clean, actual_len, strlen(clean));
    
    struct addrinfo hints, *result, *rp;
    char ip_str[INET_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM; 
    
    int status = getaddrinfo(clean, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in*)rp->ai_addr;
            inet_ntop(AF_INET, &addr->sin_addr, ip_str, sizeof(ip_str));
            memcpy(ip, ip_str, INET_ADDRSTRLEN);
            break;
        }
    }
    
    freeaddrinfo(result);
}



static void client_task(void *args) {
    int err;
    
    sockets_t sockets = *(sockets_t*)args;

    
    char buf[4096], *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;
    
    

    while (1) {
        /* read the request */
        while ((rret = read(sockets.client_socket, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR);

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
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s\n", headers[0].value);
    printf("%c --!!! ", headers[0]);
    fflush(stdout);


  //  resolve_hostname(headers[0].value, (int)headers[0].value_len);
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

    

   
    

    char ip_buff[INET_ADDRSTRLEN];
    resolve_hostname(headers[0].value, (int)headers[0].value_len, ip_buff);
    ip_buff[INET_ADDRSTRLEN] = '\0';
    
    printf("%s ip buff", ip_buff);
    fflush(stdout);
    
    upstream_connection_create(sockets.proxy_socket, ip_buff);
    
}


static void server_task(void *args) {
    
}

void proxy_run(proxy_t *proxy) {
    log_message(INFO, "PROXY: RUNNING");
    sockets_t pairs[1024];

    
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

        pairs[index].client_socket = sock;
        pairs[index].proxy_socket= proxy->socket;

        task_t task = {.args=(void*)&pairs[index], .function=client_task};

        task_queue_add(proxy->thread_pool->task_queue, task);
    
        ++index;
    }

    sleep(10);
    log_message(INFO, "PROXY: STOP RUNNING");
    
}

