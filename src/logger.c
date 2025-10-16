#include "logger.h"
#include <stdlib.h>
#include <fcntl.h>



logger_t *logger = NULL;

void logger_init(char *file_path, log_level_t log_level) {
    logger = malloc(sizeof(logger_t));
    if (!logger) {
        exit(1);
    }

    logger->log_level = log_level;

    logger->log_file = open(file_path, O_CREAT | O_RDWR | O_APPEND, 0644);
    if (logger->log_file == -1) {
        exit(1);
    }
    
}

void log(log_level_t level, char *log_message) {
    char *level_str;
    switch(level) {
        case INFO:
            level_str = "[INFO]";

            write(logger->log_file, level_str, strlen(level_str));
            write(logger->log_file, log_message, strlen(log_message));
            write(logger->log_file, "\n", strlen("\n"));

            break;

        case FATAL:
            level_str = "[FATAL]";

            write(logger->log_file, level_str, strlen(level_str));
            write(logger->log_file, log_message, strlen(log_message));
            write(logger->log_file, "\n", strlen("\n"));
            
            exit(1);

            break;
    }
    
}


