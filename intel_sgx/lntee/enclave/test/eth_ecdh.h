//
// Created by root on 9/5/19.
//

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "eEVM/common.h"
#include "../include/eth_ecp.h"

#ifndef EEVM_ETH_ECDH_H
#define EEVM_ETH_ECDH_H

class eth_ecdh {
private:
    int ret = 0;
    mbedtls_ecdh_context ctx;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
public:

    /**
     * Initialize the ecp group with what
     * we already have to generate TEE account
     * @param grp group
     * @param d private key point
     */
    eth_ecdh();

    ~eth_ecdh();

    /**
     * GET the secret key with the TEE private key as the random seed
     * @param key TEE account secret key
     */
    void load_prikey(mbedtls_mpi *key);

    /**
     * Get the curve 25591 pubkey of the TEE account
     * @return The char * version of pubkey, binary version
     */
    void get_ecdh_pubkey(unsigned char pubkey[32]);//char *ecdh_pubkey

    /**
     * Load the pubkey of peer in hex format and get share key
     * @param bin_pubkey
     * @param share_key binary share key
     * @param len length of the bin string
     */
    void get_share_key(const char *bin_pubkey, unsigned char *share_key);
};

#endif //EEVM_ETH_ECDH_H
