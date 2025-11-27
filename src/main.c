#include <logger.h>
#include <stdio.h>
#include <proxy.h>
#include <unistd.h>
#include <getopt.h>

static struct option options[] = {
    {"help", no_argument, 0 ,'h'},
    {"port", required_argument, 0, 'p'},
    {"threads", required_argument, 0, 't'},
    {"mode", required_argument, 0, 'm'}
};

static void help_print() {
    printf("%s\n", "The parameter controls the number of active threads in the thread pool. Example: ./proxy -c 1000");
    printf("%s\n", "The '-p' parameter controls the selected port. Example: ./proxy -p 8080...");
    printf("%s\n", "The '-u' parameter requires no arguments in this case, the proxy runs in 'upstream' mode, where caching is disabled and data is transmitted directly to the client. Under development");
    printf("%s\n", "If no parameters are specified, the proxy will run with the default parameters: ./proxy -p 8080 -c 10 -u");
}

int main(int argc, char *argv[]) {
    logger_init("./logger.log", DEBUG);
    if (!logger) {
        return -1;
    }

    int port = DEFAULT_PORT;
    size_t thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    int mode = UPSTREAM_MODE;
    
    int opt;
    while ((opt = getopt_long(argc, argv, "m:p:t:h", options, NULL)) != -1) {
        switch (opt) {
            case 'h':  
                help_print();
                break;
            case 'p':
                port = atoi(optarg);  
                break;
            case 't':
                thread_pool_size = atoi(optarg);  
                break; 
            case 'm':
                if (!strcmp(optarg, "upstream")) {
                    mode = UPSTREAM_MODE;
                    break;
                }
                if (!strcmp(optarg, "cache")) {
                    mode = CACHE_MODE;
                    break;
                }
        
                mode = UPSTREAM_MODE;
                break;
        }
    }

    printf("%d\n -mode", mode);
    log_message(INFO, "PROCESS START");

    printf("%d", thread_pool_size);

    proxy_t *proxy = proxy_create(port, thread_pool_size, mode);
    if (!proxy) {
        log_message(ERROR, "PROXY CREATE FAILED");
    }
    
    proxy_run(proxy);

    proxy_destroy(proxy);

    log_message(INFO, "SHUTDOWN GRACEFUL");

    free(logger);

    return 0;
}