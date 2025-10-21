#include <logger.h>
#include <stdio.h>
#include <proxy.h>

int main() {
    logger_init("./logger.log", DEBUG);
    if (!logger) {
        return -1;
    }

    log_message(INFO, "PROCESS START");

    proxy_t *proxy = proxy_create(8080, NULL);

    proxy_run(proxy);
    
    return 0;
}