#include <logger.h>
#include <stdio.h>
#include <proxy.h>
#include <unistd.h>
#include <getopt.h>
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define RESET   "\033[0m"

static struct option options[] = {
    {"help", no_argument, 0 ,'h'},
    {"port", required_argument, 0, 'p'},
    {"threads", required_argument, 0, 't'},
    {"mode", required_argument, 0, 'm'},
    {"size", required_argument, 0, 's'},
    {"ttl", required_argument, 0, 'l'}

};

static void help_print() {
    printf("%s\n", "The parameter controls the number of active threads in the thread pool. Example: ./proxy -c 1000");
    printf("%s\n", "The '-p' parameter controls the selected port. Example: ./proxy -p 8080...");
    printf("%s\n", "The '-u' parameter requires no arguments in this case, the proxy runs in 'upstream' mode, where caching is disabled and data is transmitted directly to the client. Under development");
    printf("%s\n", "If no parameters are specified, the proxy will run with the default parameters: ./proxy -p 8080 -c 10 -u");
}

int main(int argc, char *argv[]) {
    logger_init("./logger.log", DEBUG);
    if (!logger) {
        return -1;
    }

    printf(RED     "\\"                                     YELLOW ".-'''-."                                  GREEN "                                 \n");
    printf(RED     " "                                     YELLOW "                                   '   _    \\"                               GREEN "                               \n");
    printf(RED     "_________   _...._"               YELLOW "               /   /` '.   \\"                             GREEN "                              \n");
    printf(RED     "\\        |.'      '-."           YELLOW "           .   |     \\  '"              CYAN "              .-.          .-" BLUE " \n");
    printf(RED     " \\        .'```'.    '. .-,.--." YELLOW " |   '      |  '"              CYAN "              \\ \\        / /" BLUE " \n");
    printf(RED     "  \\      |       \\     \\|  .-. |" YELLOW "\\    \\     / /____"     CYAN "     _____  \\ \\      / /"  BLUE "  \n");
    printf(RED     "   |     |        |    || |  | |" YELLOW " `.   ` ..' /`.   \\"  CYAN "  .'    /   \\ \\    / /"   BLUE "   \n");
    printf(RED     "   |      \\      /    . | |  | |" YELLOW "    '-...-'`   `.  `'    .'"     CYAN "     \\ \\  / /"    BLUE "    \n");
    printf(RED     "   |     |\\`'-.-'   .'  | |  '-" YELLOW "                  '.    .'"        CYAN "        \\ `  /"     BLUE "     \n");
    printf(RED     "   |     | '-....-'`    | |"      YELLOW "                      .'     `."        CYAN "        \\  /"      BLUE "      \n");
    printf(RED     "  .'     '.             | |"      YELLOW "                    .'  .'`.   `."      CYAN "      / /"       BLUE "       \n");
    printf(RED     "'-----------'           |_|"      YELLOW "                  .'   /    `.   `.|`-' /"        BLUE "        \n");
    printf(RED     " "                                   YELLOW "                '----'       '----''..'"       BLUE "       \n" RESET);


    int port = DEFAULT_PORT;
    size_t thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    int mode = CACHE_MODE;
    size_t cache_size = 100000000000;
    size_t cache_ttl = 5;

    int opt;
    while ((opt = getopt_long(argc, argv, "l:s:m:p:t:h", options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                help_print();
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                thread_pool_size = atoi(optarg);
                break;
            case 'm':
                if (!strcmp(optarg, "upstream")) {
                    mode = UPSTREAM_MODE;
                    break;
                }
                if (!strcmp(optarg, "cache")) {
                    mode = CACHE_MODE;
                    break;
                }
                mode = UPSTREAM_MODE;
                break;
            case 's':
                cache_size = atoi(optarg);
                break;
            case 'l':
                cache_ttl = atoi(optarg);
                break;
        }
    }

    log_message(INFO, "PROCESS START");


    proxy_t *proxy = proxy_create(port, thread_pool_size, mode, cache_size, cache_ttl);
    if (!proxy) {
        log_message(ERROR, "PROXY CREATE FAILED");
    }

    proxy_run(proxy);

    proxy_destroy(proxy);

    log_message(INFO, "SHUTDOWN GRACEFUL");

    free(logger);

    return 0;
}