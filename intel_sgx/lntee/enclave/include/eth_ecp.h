//
// Created by root on 9/5/19.
//
#include "mbedtls/ecp.h"
#include "eEVM/common.h"
#include <eEVM/simple/simpleglobalstate.h>

#ifndef EEVM_ETH_ECP_H
#define EEVM_ETH_ECP_H

// Define a enclave global ctx to store the private key and ecp group
class TEESingleton {
public:
    static mbedtls_ecp_keypair *ecp_ctx;

    static void print_mpi_to_hex(const char *name, const mbedtls_mpi *X);
};

#endif //EEVM_ETH_ECP_H
