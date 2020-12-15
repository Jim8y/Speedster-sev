
#ifndef TEST_AES_H
#define TEST_AES_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MBEDTLS_AES_ALT)
// Regular implementation
//

#if defined(MBEDTLS_CIPHER_MODE_XTS)

#endif /* MBEDTLS_CIPHER_MODE_XTS */

#else  /* MBEDTLS_AES_ALT */
#include "aes_alt.h"
#endif /* MBEDTLS_AES_ALT */

int mbedtls_aes_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* aes.h */
