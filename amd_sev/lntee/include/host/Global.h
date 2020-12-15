//
// Created by nuanbing on 1/31/20.
//
#include "eEVM/common.h"
#include <iostream>

#ifndef LNTEE_GLOBAL_H
#define LNTEE_GLOBAL_H

typedef struct {
    std::string ip;
    int port;
    std::string msg;
} MSG_connect;

class Global {
public:
    static std::string ip;
    static int port;
    static char pubkey_hex[PUBKEY_LEN * 2 + 3];
    static char addr_hex[ADDRESS_LEN * 2 + 3];

//    static SEV_Handler *sev_handler;
//    static oe_enclave_t *enclave;
    static void from_hex(const char *src, char *target);

    static std::string to_hex(const unsigned char *data, size_t len);
};


#endif //LNTEE_GLOBAL_H
