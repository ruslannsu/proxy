#include <logger.h>
#include <stdio.h>
#include <proxy.h>

int main() {
    logger_init("./logger.log", DEBUG);
    if (!logger) {
        return -1;
    }
    
    log(INFO, "MAIN: PROCESS START");

    proxy_t *proxy = proxy_create(8080, NULL);

   // sleep(1000);
    
    return 0;
}