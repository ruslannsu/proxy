

typedef enum log_level_t {
    INFO,
    DEBUG,
    FATAL,
}log_level_t;

typedef struct logger_t {
    int log_file;
    log_level_t log_level;
}logger_t;

logger_t *logger_create(char *file_path, log_level_t log_level);
 



void log(logger_t *logger, log_level_t level, char *log_message);




