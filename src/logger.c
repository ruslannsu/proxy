#include "logger.h"
#include <stdlib.h>

logger_t *logger_create(char *file_path, log_level_t log_level) {
    logger_t *logger = malloc(sizeof(logger_t));
    if (!logger) {
        return NULL;
    }
    

}


