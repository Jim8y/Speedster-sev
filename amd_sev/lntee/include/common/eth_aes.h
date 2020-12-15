//
// Created by root on 9/5/19.
//
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/aes.h"
#include "mbedtls/ecp.h"
#include "mbedtls/md.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <eEVM/common.h>
#include <eEVM/simple/simpleglobalstate.h>
#include <eEVM/debug.h>

#ifndef EEVM_ETH_AES_H
#define EEVM_ETH_AES_H

#if defined(__cplusplus)
extern "C" {
#endif

void initial_aes();

/**
 * Encrypt the message
 * @param share_key share key
 * @param msg messages to be encrypted, the buffer should be integer multiple of 16
 * @param plain_len must be integer multiple of 16
 * @param cipher buffer size should be bigger than plain_len+16+32
 */
void aes_encrypt(unsigned char *msg, int block, unsigned char *cipher);
/**
 * Decrypt the cipher message
 * @param cipher cipher text
 * @param enc_len
 * @param msg plain message decrypted, size should be bigger than enc_len-16-32(IV||cipher||digest)
 */
void aes_decrypt(char *cipher, int block, unsigned char *msg);

/**
 * End the aes
 */
void aes_finish();

void aes_test();

#if defined(__cplusplus)
}
#endif
#endif //EEVM_ETH_AES_H
