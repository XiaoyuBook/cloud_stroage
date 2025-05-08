#ifndef TLV_H
#define TLV_H 
#define TLV_TYPE_CD        1
#define TLV_TYPE_LS        2  
#define TLV_TYPE_PUTS      3   
#define TLV_TYPE_GETS      4 
#define TLV_TYPE_REMOVE    5
#define TLV_TYPE_PWD       6
#define TLV_TYPE_MKDIR     7
#define TLV_TYPE_RMDIR     8
#define TLV_TYPE_LOGIN     9
#define TLV_TYPE_REGISTER  10
#define TLV_TYPE_SALT      11 
#define TLV_TYPE_ERROR     12 
#define TLV_TYPE_SUCCESS   13 
#include<stdint.h>
#include <my_header.h>
typedef struct {
    uint16_t type;
    uint16_t length;
    char value[];
}__attribute__((packed))tlv_t;  // __attribute__((packed))表示让编译器别自动填充

#endif
