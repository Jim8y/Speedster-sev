#include <eth_ecdsa.h>

#include <stdexcept>
#include <string.h>
#include <string>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/platform.h"
//#define SIGN_DEBUG
//#undef SIGN_DEBUG
#include <eEVM/debug.h>
#define ECPARAMS MBEDTLS_ECP_DP_SECP256K1
#include <sys/time.h>

static long  int ms = 0;
// pubkey: 64 Bytes
// SHA3-256: 32 Bytes
// use lower 160 bits as address
const mbedtls_ecp_curve_info *curve_info;

void time_curr(const char *tag) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms_2 = tp.tv_sec * 1000 * 1000 + tp.tv_usec;
    std::cout << "TIME LOG: " << tag << " >>> " << ms_2 - ms << std::endl;
    ms = ms_2;
}

static int myrand(void *rng_state, unsigned char *output, size_t len) {
    srand(time(NULL));
    size_t use_len;
    int rnd;

    if (rng_state != NULL)
        rng_state = NULL;

    while (len > 0) {
        use_len = len;
        if (use_len > sizeof(int))
            use_len = sizeof(int);

        rnd = rand();
        memcpy(output, &rnd, use_len);
        output += use_len;
        len -= use_len;
    }
    return (0);
}

int get_pubkey_addr(
        const mbedtls_mpi *seckey,
        unsigned char *pubkey,
        unsigned char *address) {

    if (pubkey == NULL ||
        address == NULL ||
        seckey == NULL) {
        return -1;
    }

    unsigned char __pubkey[65];
    unsigned char __address[32];
    size_t buflen = 0;
    int ret;

    //    INFO();
    //    TEESingleton::print_mpi_to_hex("Secret key in TEESingleton::ecp_ctx",&TEESingleton::ecp_ctx->d);
    //    INFO();
    ret = mbedtls_ecp_mul(&TEESingleton::ecp_ctx->grp, &TEESingleton::ecp_ctx->Q, &TEESingleton::ecp_ctx->d,
                          &TEESingleton::ecp_ctx->grp.G, NULL, NULL);
    if (ret != 0) {
        std::cout << "Error: mbedtls_ecp_mul returned " << ret << std::endl;
        return -1;
    }

    ret = mbedtls_ecp_point_write_binary(
            &TEESingleton::ecp_ctx->grp, &TEESingleton::ecp_ctx->Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &buflen, __pubkey, 65);

    if (ret == MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL) {
        std::cout << "buffer too small" << std::endl;
        return -1;
    } else if (ret == MBEDTLS_ERR_ECP_BAD_INPUT_DATA) {
        std::cout << "bad input data" << std::endl;
        return -1;
    }

    if (buflen != 65) {
        std::cout << "ecp serialization is incorrect olen=%ld" << buflen
                  << std::endl;
    }

    eevm::keccak_256(__pubkey + 1, 64, __address);

    // copy to user space
    memcpy(pubkey, __pubkey + 1, 64);
    memcpy(address, __address + 12, 20);
    //    INFO();
    return 0;
}

/**
 * Set the global secret key
 * @param secret
 * @param secret_len
 * @param pubkey
 * @param address
 * @return
 */
int provision_ecdsa_key(const char *secret, size_t secret_len) {
    // used by edge8r
    (void) secret_len;
    uint8_t prikey[SECKEY_LEN];
    memcpy(prikey, secret, SECKEY_LEN);

    mbedtls_ecdsa_init(TEESingleton::ecp_ctx);
    mbedtls_ecp_group_load(&TEESingleton::ecp_ctx->grp, ECPARAMS);

    int ret = mbedtls_mpi_read_binary(&TEESingleton::ecp_ctx->d, prikey, SECKEY_LEN);
    if (ret != 0)
        LOG("Failed to read binary");

    //    INFO();
    //    TEESingleton::print_mpi_to_hex("Copy from bin ",&TEESingleton::ecp_ctx->d);
    //    INFO();
    return ret;
}

/**
 * Get the pubkey and address of TEE account
 * @param pubkey
 * @param address
 * @return
 */
int get_address(unsigned char *pubkey, unsigned char *address) {
    return get_pubkey_addr(&TEESingleton::ecp_ctx->d, pubkey, address);
}

int ecdsa_sign(const uint8_t *hash) {
//    DEBUG("");
//    time_curr("Start to Sign");
    unsigned char tmp[200];
    if (curve_info == NULL) {
        curve_info = mbedtls_ecp_curve_info_from_grp_id(MBEDTLS_ECP_DP_SECP256K1);
    }
    size_t sig_len;

    if (mbedtls_ecdsa_write_signature(TEESingleton::ecp_ctx, MBEDTLS_MD_SHA256, hash, curve_info->bit_size,
                                      tmp, &sig_len, myrand, NULL) != 0) {
        DEBUG("ERROR");
        return -1;
    }
//    time_curr("End to Sign");
//    for (int i = 0; i < 10000; i++) {
        ecdsa_verify(hash, tmp, sig_len);
//    }
//    time_curr("End to Verify");
//    DEBUG("");
//    std::cout << sig_len << std::endl;
//    std::cout << "Signature : "
//              << lntee::to_hex((unsigned char *) tmp, sig_len)
//              << std::endl;
    return 0;
}

bool ecdsa_verify(const uint8_t *hash, const uint8_t *sig, size_t sig_len) {
//    DEBUG("");
    int ret = 0;
    ret = mbedtls_ecdsa_read_signature(TEESingleton::ecp_ctx, hash, curve_info->bit_size,
                                       sig, sig_len);
    return true;
}