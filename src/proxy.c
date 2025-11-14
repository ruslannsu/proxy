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
#include <assert.h>
#include <unistd.h>
#include <netdb.h>  
#include <signal.h>


int process_status = RUN;


//TODO: вообще так не круто обработчик переназначать, надо на sigaction
void shutdown_handler(int sig) {
    process_status = SHUTDOWN;    
}
 
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

    err = listen(proxy->socket, 5000);
    if (err < 0) {
        log_message(FATAL, "PROXY DO NOT LISTEN");
        
    }

    err = signal(SIGINT, shutdown_handler);
    
    log_message(INFO, "PROXY: CREATION COMPLETE");

    return proxy;
}


static int parse_http_headers(int client_socket, http_parse_t *parse) {
    ssize_t rret;
    size_t buflen = 0, prevbuflen = 0;
    size_t num_headers;
    size_t pathlen;
    int pret, minor_version;

    while (1) {
        while ((rret = read(client_socket, parse->buf + buflen, sizeof(parse->buf) - buflen)) == -1 && errno == EINTR);

        if (rret <= 0)
            return -1;
        prevbuflen = buflen;
        buflen += rret;
        num_headers = sizeof(parse->headers) / sizeof(parse->headers[0]);
        pret = phr_parse_request(parse->buf, buflen, &parse->method, &parse->method_len, &parse->path, &parse->path_len,
                                &minor_version, parse->headers, &num_headers, prevbuflen);
        if (pret > 0)
            break; 
        else if (pret == -1)
            return -1;

        assert(pret == -2);
        fflush(stdout);

        if (buflen == sizeof(parse->buf))
                return -1;
    }

    return 0;


}


//TODO: буфер здесь тоже создавать не стоит
static int http_response_parse(int sock, char **http_response, size_t *http_response_size) {
    size_t size = RESPONSE_BUFFER_SIZE;

    char *buf = malloc(sizeof(char) * size);
    if (!buf) {
        log_message(FATAL, "MEM ALLOCATION FOR RESP BUFFER FAILED");
    }

    char *method, *path;
    char *msg;
    size_t msg_len;
    int status;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;

    while (1) {
        while ((rret = read(sock, buf + buflen, size - buflen)) == -1 && errno == EINTR);
        if (rret <= 0)
            return -1;
        prevbuflen = buflen;
        buflen += rret;
        num_headers = sizeof(headers) / sizeof(headers[0]);
        
        pret = phr_parse_response(buf, buflen, &minor_version, &status, &msg, &msg_len, headers, &num_headers, prevbuflen);

        if (pret > 0)
            break; 
        else if (pret == -1)
            return -1;

        assert(pret == -2);
        if (buflen == size)
            return -1;
    }

    size_t http_body_size;
    for (size_t i = 0; i != num_headers; ++i) {
        if (strncmp(headers[i].name, "Content-Length", 14) == 0) {
            //printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name, (int)headers[i].value_len, headers[i].value);
            size_t val_size = headers[i].value_len;
            char val_buf[val_size];
            memcpy(val_buf, headers[i].value, val_size);
            http_body_size = atoi(val_buf);
            break;
        }
    }

    
    size_t bytes_left = http_body_size - (buflen - pret);
    size_t bytes_count = 0;
   
    size_t read_bytes_count = 1024;

    int err;

    while (bytes_count != bytes_left) {
        if (buflen >= http_body_size + pret) {
            break;
        }

        if ((bytes_left - bytes_count) > read_bytes_count) {
            read_bytes_count = bytes_left - bytes_count;
        }
        
        err = read(sock, buf + buflen + bytes_count, read_bytes_count);
        if (err < 0) {
            break;
        }

        bytes_count += err;
    }
    
    *http_response = buf;
    *http_response_size = http_body_size + pret;

    return 0;
}

static int upstream_connection_create(int ups_socket, char *ip) { 
    struct sockaddr_in server_addr;
    int port = 80;
  
    char *ip_ = ip;
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_);
    
    int err;

    err = connect(ups_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (err != 0) {
        log_message(ERROR, "PROXY UPSTREAM 'CONNECT' FAILED. ERRNO: %s", strerror(errno));
        return -1;
    }

    return 0;
    
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
    
    struct addrinfo hints, *result, *rp;
    char ip_str[INET_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM; 
    
    int status = getaddrinfo(clean, NULL, &hints, &result);
    if (status != 0) {
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
//TODO:перед каждым return надо чистить ресурсы
static void client_task(void *args) {
    int err;

    sockets_t sockets = *(sockets_t*)args;

    http_parse_t http_parse;

    err = parse_http_headers(sockets.client_socket, &http_parse);
    if (err != 0) {
        log_message(ERROR, "CLIENT TASK: CAN NOT PARSE HTTP HREADERS");
        return;
    }

    struct phr_header *headers = http_parse.headers;
    char *path = http_parse.path;

    char ip_buff[INET_ADDRSTRLEN];
    resolve_hostname(headers[0].value, (int)headers[0].value_len, ip_buff);
    ip_buff[INET_ADDRSTRLEN] = '\0';
     

    int ups_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ups_sock < 0) {
        log_message(ERROR, "UPS SOCKET CREATION FAILED");
    }


    err = upstream_connection_create(ups_sock, ip_buff);
    if (err != 0) {
        log_message(ERROR, "UPSTREAM CONNECTION FAILED");
    }

    char request[4096];
    int req_len = snprintf(request, sizeof(request),
    "GET %s HTTP/1.0\r\n"
    "Connection: close\r\n"
    "\r\n",
    path, headers[0].value);

    err = send(ups_sock, request, req_len, 0);
    if (err == -1) {
        log_message(ERROR, "SEND TO UPSTREAM FAILED, ERRNO: %s", strerror(errno));
        return;
    }

    char *http_response;
    size_t http_response_size;
    err = http_response_parse(ups_sock, &http_response, &http_response_size);
    if (err != 0) {
        log_message(ERROR, "HTTP RESPONSE PARSE FAILED. IP:%s", ip_buff);
    }

    err = send(sockets.client_socket, http_response, http_response_size, 0);
    if (err == -1) {
        log_message(ERROR, "SEND TO CLIENT FAILED, ERRNO: %s", strerror(errno));
        return;
    }
    
    size_t print_len = http_response_size;

    free(http_response);
    close(sockets.client_socket);
    close(ups_sock);
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

        if (process_status == SHUTDOWN) {
            log_message(INFO, "PROXY STOPPED(SHUTDOWN)");
            break;
        }
        
        log_message(INFO, "PROXY NEW CONNECTION WITH ADDR: %s, PORT: %d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        pairs[index].client_socket = sock;
        pairs[index].proxy_socket= proxy->socket;

        task_t task = {.args=(void*)&pairs[index], .function=client_task};

        task_queue_add(proxy->thread_pool->task_queue, task);
    
        ++index;
    }

    log_message(INFO, "PROXY: STOP RUNNING");
    
}


void proxy_destroy(proxy_t *proxy) {
    thread_poll_destroy(proxy->thread_pool);
    free(proxy);
    log_message(INFO, "PROXY STRUCT DESTROYED");
}
