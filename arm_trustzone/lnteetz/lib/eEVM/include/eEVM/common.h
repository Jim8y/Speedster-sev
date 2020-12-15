#ifndef ENCLAVE_COMMONS_H
#define ENCLAVE_COMMONS_H

#ifdef __GNUC__
#  define DEPRECATED(func) func __attribute__((deprecated))
#elif defined(_MSC_VER)
#  define DEPRECATED(func) __declspec(deprecated) func
#else
#  pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#  define DEPRECATED(func) func
#endif

// used in eth_ecdsa.c
#define SECRETKEY_SEALED_LEN 1024
#define SECKEY_LEN 32
#define PUBKEY_LEN 64
#define ADDRESS_LEN 20

#define SECKEY_HEX_LEN 64
#define PUBKEY_HEX_LEN 128
#define ADDRESS_HEX_LEN 40

#define TC_KEY_NOT_PROVISIONED 0x9001

// #include "encoding.h"

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>
#include "eEVM/simple/simpleglobalstate.h"

#define ROUND_TO_32(x) ((x + 31) / 32 * 32)

const uint8_t HEX_BASE = 16;
const uint8_t DEC_BASE = 10;

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

namespace lntee {
    std::vector<uint8_t> from_hex(const char *src);

    void from_hex(const char *src, char *target);

    std::string to_hex(const unsigned char *data, size_t len);
}

template<typename T>
T swap_endian(T u) {
    assert(CHAR_BIT == 8);

    union {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

// convert [buf, buf + n] to a T
template<typename T>
T uint_bytes(const unsigned char *buf, size_t n, bool big_endian = true) {
    T ret;
    memcpy(&ret, buf + n - sizeof(T), sizeof(T));
    if (big_endian)
        ret = swap_endian<T>(ret);

    return ret;
}

#endif