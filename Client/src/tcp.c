
#include <my_header.h>
// 用于建立tcp连接
int tecp_connect(char *ip, char *port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);
    int ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    return sockfd;
}