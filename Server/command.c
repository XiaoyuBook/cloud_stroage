#include "command.h"
void *thread_work(void *arg){
    client_connect_t *client = (client_connect_t*)arg;
    bzero(client->buf,sizeof(client->buf));
    int count = recv(client->epfd, client->buf, sizeof(client->buf), 0);
    if(count == 0) {
        printf("客户端已主动断开连接!\n");
        remove_client(client->netfd, client->epfd);
        return NULL;
    }
    client->buf_len = count;
    if(client->buf_len < sizeof(tlv_t)) {
        printf("数据格式有问题!\n");
        return NULL;
    }

    tlv_t *msg = (tlv_t *)client->buf;
    uint16_t type = ntohs(msg->type);
    uint16_t length = ntohs(msg->length);
    char *value = msg->value;
    printf("接收到 TLV: type=%d, length=%d\n", type, length);
    switch(type){
    case TLV_TYPE_CD:       

        break;
    case TLV_TYPE_LS:  

        break;
    case TLV_TYPE_PUTS:  

        break;
    case TLV_TYPE_GETS:

        break;
    case TLV_TYPE_REMOVE:

        break;
    case TLV_TYPE_PWD:

        break;
    case TLV_TYPE_MKDIR:

        break;
    case TLV_TYPE_RMDIR:

        break;
    case TLV_TYPE_LOGIN:

        break;
    case TLV_TYPE_REGISTER:

        break;
    default:
        break;  
    }
}
// 发送错误响应给客户端
void send_error_response(client_connect_t *client, const char *error_msg) {
    tlv_t *response = (tlv_t *)client->buf;
    response->type = TLV_TYPE_ERROR;
    response->length = strlen(error_msg) + 1;
    strcpy(response->value, error_msg);
    send(client->netfd, client->buf, sizeof(tlv_t) + response->length, 0);
}

// 发送成功响应给客户端
void send_success_response(client_connect_t *client, const char *success_msg) {
    tlv_t *response = (tlv_t *)client->buf;
    response->type = TLV_TYPE_SUCCESS;
    response->length = strlen(success_msg) + 1;
    strcpy(response->value, success_msg);
    send(client->netfd, client->buf, sizeof(tlv_t) + response->length, 0);
}

void send_salt_response(client_connect_t *client, const char *salt) {
    tlv_t *response = (tlv_t *)client->buf;
    response->type = TLV_TYPE_SALT;
    response->length = strlen(salt) + 1;
    strcpy(response->value, salt);
    send(client->netfd, client->buf, sizeof(tlv_t) + response->length, 0);
}
// 正确的哈希计算方式（注册和登录保持一致）
void compute_hash(const char *password, const char *salt, char *hash_result) {
    char password_with_salt[256];
    snprintf(password_with_salt, sizeof(password_with_salt), "%s%s", password, salt);  // 密码 + 盐
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)password_with_salt, strlen(password_with_salt), hash);       // SHA1

    // 转换为十六进制字符串
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(hash_result + i * 2, "%02x", hash[i]);
    }
    hash_result[SHA_DIGEST_LENGTH * 2] = '\0';
}
