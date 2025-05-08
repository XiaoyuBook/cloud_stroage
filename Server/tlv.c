#include "tlv.h"
// TLV 2+2+变长
void handle_tlv_task(const char *buf, int len, int client_fd){
    int offset = 0;
    while(offset + sizeof(tlv_t) <= len ) {
        tlv_t *tlv = (tlv_t*)(buf + offset);
        uint16_t type = ntohs(tlv->type);
        uint16_t length = ntohs(tlv->length);
        if(offset + sizeof(tlv_t) + length > len)
    }
}
