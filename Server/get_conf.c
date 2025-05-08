#include "get_conf.h"

int create_default_config() {
    int fd = open(CONFIG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    ERROR_CHECK(fd, -1, "open");

    char buf[256];
    int len = sprintf(buf,
                      "#服务器默认配置文件\n"
                      "ip = %s\n"
                      "port = %d\n"
                      "worker_num = %d\n",
                      DEFAULT_IP, DEFAULT_PORT, DEFAULT_WORKER_NUM);

    if (write(fd, buf, len) != len) {
        fprintf(stderr, "config create failed!\n");
        close(fd);
        return -1;
    }

    close(fd);
    printf("已创建默认配置文件: %s\n", CONFIG_FILE);
    printf("可以修改以下默认配置:\n");
    printf("IP地址: %s\n", DEFAULT_IP);
    printf("端口号: %d\n", DEFAULT_PORT);
    printf("工作线程数: %d\n", DEFAULT_WORKER_NUM);
    return 0;
}

int get_config(const char *path, server_config *config) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        // 配置文件不存在，创建默认配置
        create_default_config();
        strcpy(config->ip, DEFAULT_IP);
        config->port = DEFAULT_PORT;
        config->worker_num = DEFAULT_WORKER_NUM;
        return 0;
    }

    char buf[1024];
    ssize_t count = read(fd, buf, sizeof(buf) - 1); // 修正了 sizeof 错误
    if (count <= 0) {
        perror("failed to get config");
        close(fd);
        return -1;
    }

    buf[count] = '\0';  // 保证字符串结束符
    close(fd);

    char *line = strtok(buf, "\n");
    while (line != NULL) {
        if (line[0] != '\0' && line[0] != '#') {
            char key[32], value[32];
            if (sscanf(line, " %31[^= \t] %*[= \t] %31s", key, value) == 2){
                if (strcmp(key, "ip") == 0) {
                    strncpy(config->ip, value, sizeof(config->ip) - 1);
                    config->ip[sizeof(config->ip) - 1] = '\0';
                } else if (strcmp(key, "port") == 0) {
                    config->port = atoi(value);
                } else if (strcmp(key, "worker_num") == 0) {
                    config->worker_num = atoi(value);
                }
            }
        }
        line = strtok(NULL, "\n");
    }

    return 0;
}

#ifdef TEST_CONFIG
int main() {
    server_config config;

    if (get_config(&config)) {
        fprintf(stderr, "Config load failed!\n");
        return 1;
    }

    printf("Config test:\n");
    printf("IP: %s\n", config.ip);
    printf("Port: %d\n", config.port);
    printf("Workers: %d\n", config.worker_num);

    return 0;
}
#endif

