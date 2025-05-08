#include <my_header.h>
#define CONFIG_FILE "./conf/server.conf"
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 12345
#define DEFAULT_WORKER_NUM 10

typedef struct {
    char ip[16];
    int port;
    int worker_num;
}server_config;

int get_config(const char *path, server_config *config);
int create_default_config();
