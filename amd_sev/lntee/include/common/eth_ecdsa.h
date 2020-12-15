#include <stdint.h>

#include <mbedtls/bignum.h>
#include "eEVM/common.h"
#include "eEVM/simple/simpleglobalstate.h"
#include "eEVM/util.h"
#include <eEVM/debug.h>
#include "mbedtls/bignum.h"
#include "mbedtls/config.h"
#include "mbedtls/error.h"
#include <common/shared.h>
#include <common/eth_ecp.h>

#ifndef ENCLAVE_ECDSA_H
#define ENCLAVE_ECDSA_H

#if defined(__cplusplus)
extern "C"
{
#endif

int ecdsa_sign(const uint8_t *hash);

bool ecdsa_verify(const uint8_t *hash, const uint8_t *sig, size_t sig_len);

int get_pubkey_addr(const mbedtls_mpi *seckey,
                    unsigned char *pubkey,
                    unsigned char *address);

int provision_ecdsa_key(const char *secret, size_t secret_len);

int get_address(unsigned char *pubkey, unsigned char *address);

void ecdsa_test();

#if defined(__cplusplus)
}
#endif
#endif