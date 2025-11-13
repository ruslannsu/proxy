#include <logger.h>
#include <stdio.h>
#include <proxy.h>
#include <unistd.h>
#include <getopt.h>




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
    int thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    int opt;
    while ((opt = getopt(argc, argv, "p:h")) != -1)  {
        switch (opt) {
            case 'p': 
                port = atoi(optarg);
                break;
            case 'h':
                help_print();
        }
    }

    log_message(INFO, "PROCESS START");

    proxy_t *proxy = proxy_create(port);
    if (!proxy) {
        log_message(ERROR, "PROXY CREATE FAILED");
    }

    proxy_run(proxy);


    proxy_destroy(proxy);
    log_message(INFO, "SHUTDOWN GRACEFUL");

    return 0;
}