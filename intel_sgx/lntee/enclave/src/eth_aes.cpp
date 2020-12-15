//
// Created by root on 9/5/19.
//
#include <eth_aes.h>
#include "mbedtls/gcm.h"
#include <eEVM/debug.h>
//#include "sgx_trts.h"
//#include "sgx_tcrypto.h"
//#include "stdlib.h"
//#include <string.h>
int ret = 0;
char iv[] = "abababababababab";
char key[] = "abcdefghijklmnop";
mbedtls_gcm_context aes;

void initial_aes() {
    mbedtls_gcm_init(&aes);
}

void aes_encrypt(unsigned char *msg, int block, unsigned char *cipher) {
    int size = block * 16;
    memset(cipher, 0, size);

    ret = mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char *) key, strlen(key) * 8);
    if (ret != 0)
        DEBUG("ERROR");

    mbedtls_gcm_starts(&aes, MBEDTLS_GCM_ENCRYPT, (const unsigned char *) iv, strlen(iv), NULL, 0);

    ret = mbedtls_gcm_update(&aes, size, (const unsigned char *) msg, cipher);
    if (ret != 0)
        std::cout << "ERROR WHILE DECRYPT " << std::endl;
}

void aes_decrypt(char *cipher, int block, unsigned char *msg) {
    int size = block * 16;
    memset(msg, 0, size);
    ret = mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char *) key, strlen(key) * 8);
    if (ret != 0)
        DEBUG("ERROR");

    mbedtls_gcm_starts(&aes, MBEDTLS_GCM_DECRYPT, (const unsigned char *) iv, strlen(iv), NULL, 0);

    /**
     *
     *ctx	The GCM context. This must be initialized.
     *length	The length of the input data. This must be a multiple of 16 except in the last call before mbedtls_gcm_finish().
     *input	The buffer holding the input data. If length is greater than zero, this must be a readable buffer of at least that size in Bytes.
     *output	The buffer for holding the output data. If length is greater than zero, this must be a writable buffer of at least that size in Bytes.
     */
    ret = mbedtls_gcm_update(&aes, size, (const unsigned char *) cipher, msg);
    if (ret != 0)
        std::cout << "ERROR WHILE DECRYPT " << std::endl;
    DEBUG("");

}

void aes_finish() {
    mbedtls_gcm_free(&aes);
}

void aes_test() {
}