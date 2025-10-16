#include <logger.h>
#include <stdio.h>


int main() {
    logger_t *logger = logger_create("./logger.log", DEBUG);
    if (!logger) {
        printf("BAD");
        fflush(stdout);
    }
    
    log(logger, INFO, "PROXY FIRST LOGfsd debug");
    return 0;
}