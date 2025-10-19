#include "logger.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>



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

void log_message(log_level_t level, char *format, ...) {
    char log_message[1024];

    va_list args;

    va_start(args, format);

    vsnprintf(log_message, sizeof(log_message), format, args);

    char *level_str;
    char log_complete_message[1024];
    switch(level) {
        case INFO:
            snprintf(log_complete_message, sizeof(log_complete_message), "[INFO] %s \n", log_message);            
            write(logger->log_file, log_complete_message, strlen(log_complete_message));
            break;

        case FATAL:
            snprintf(log_complete_message, sizeof(log_complete_message), "[FATAL] %s \n", log_message);            
            write(logger->log_file, log_complete_message, strlen(log_complete_message));

            exit(1);

            break;
        
        case ERROR:
            snprintf(log_complete_message, sizeof(log_complete_message), "[ERROR] %s \n", log_message);            
            write(logger->log_file, log_complete_message, strlen(log_complete_message));
       
            break;    
    }
    
}


