#include <logger.h>
#include <stdio.h>
#include <proxy.h>

int main() {
    logger_init("./logger.log", DEBUG);
    if (!logger) {
        return -1;
    }

    log_message(INFO, "PROCESS START");

    proxy_t *proxy = proxy_create(8080);;
    if (!proxy) {
        log_message(ERROR, "PROXY CREATE FAILED. ");
    }

    proxy_run(proxy);
    return 0;
}