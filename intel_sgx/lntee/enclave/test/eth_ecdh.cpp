//
// Created by root on 9/5/19.
//

#include "eth_ecdh.h"

/**
 * Initialize the ecdh
 */
eth_ecdh::eth_ecdh() {
    fflush(stdout);
    mbedtls_ecdh_init(&ctx);
    fflush(stdout);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    fflush(stdout);
    mbedtls_entropy_init(&entropy);
}

/**
 * GET the secret key with the TEE private key as the random seed
 * @param key TEE account secret key
 */
void eth_ecdh::load_prikey(mbedtls_mpi *key) {
    INFO();
    unsigned char buf[64];
    size_t n;
    memset(buf, 0, sizeof(buf));
    mbedtls_mpi_write_string(key, 16, (char *) buf, sizeof(buf) - 1, &n);
    if (mbedtls_ctr_drbg_seed(&ctr_drbg,
                              mbedtls_entropy_func, &entropy,
                              (const unsigned char *) buf, n) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %s \n", ret);
        return;
    }
    mbedtls_ecp_group_load(&ctx.grp, MBEDTLS_ECP_DP_CURVE25519);
    ret = mbedtls_ecdh_gen_public(&ctx.grp, &ctx.d, &ctx.Q,
                                  mbedtls_ctr_drbg_random, &ctr_drbg);
    if (0 != ret) {
        mbedtls_printf(" failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret);
        return;
    }
    DEBUG("GENERATE ECDH PAIR");
//    TEESingleton::print_mpi_to_hex("ECDH prikey", &ctx.d);
//    TEESingleton::print_mpi_to_hex("ECDH pubkey", &ctx.Q.X);
//    TEESingleton::print_mpi_to_hex("TEE PRI", &TEESingleton::ecp_ctx->d);
}

/**
* Load the pubkey of peer in hex format
* @param bin_pubkey
 * @param share_key binary share key
* @param len length of the bin string
*/
void eth_ecdh::get_share_key(const char *bin_pubkey, unsigned char *share_key) {
    INFO();
    fflush(stdout);
    ret = mbedtls_mpi_lset(&ctx.Qp.Z, 1);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_mpi_lset returned %d\n", ret);
        return;
    }

    ret = mbedtls_mpi_read_binary(&ctx.Qp.X, (const unsigned char *) bin_pubkey, 32);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
        return;
    }
    ret = mbedtls_ecdh_compute_shared(&ctx.grp, &ctx.z,
                                      &ctx.Qp, &ctx.d,
                                      mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret);
        return;
    }
    mbedtls_printf(" ok\n");
    ret = mbedtls_mpi_write_binary(&ctx.z, share_key, 32);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
        return;
    }
    // DEBUG("GET SHARE KEY:");
    // // TEESingleton::print_mpi_to_hex("Share key:", &ctx.z);

    // *key_len = ctx.z.n * 8;
}


/**
* Get the curve 25591 pubkey of the TEE account
* @return The char * version of pubkey, binary version
*/
void eth_ecdh::get_ecdh_pubkey(unsigned char pubkey[32]) {
    INFO();
//    TEESingleton::print_mpi_to_hex("Pubkey:", &ctx.Q.X);
    ret = mbedtls_mpi_write_binary(&ctx.Q.X, pubkey, 32);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
        return;
    }
}

eth_ecdh::~eth_ecdh() {
    mbedtls_ecdh_free(&ctx);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}