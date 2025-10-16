#include <logger.h>
#include <stdio.h>
#include <proxy.h>

int main() {
    logger_init("./logger.log", DEBUG);
    if (!logger) {
        printf("BAD");
        fflush(stdout);
        return -1;
    }
    
    proxy_t *proxy = proxy_create(3, NULL);


    l
    return 0;
}