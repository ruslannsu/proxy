#include <stdlib.h>

typedef enum log_level_t {
    INFO,
    DEBUG,
    ERROR,
    FATAL,

}log_level_t;

typedef struct logger_t {
    int log_file;
    log_level_t log_level;
}logger_t;


extern logger_t *logger;

void logger_init(char *file_path, log_level_t log_level);
 

void log(log_level_t level, char *log_message);




