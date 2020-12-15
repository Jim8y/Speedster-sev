//
// Created by root on 9/8/19.
//

#include <common/eth_ecp.h>

mbedtls_ecp_keypair *TEESingleton::ecp_ctx = new mbedtls_ecp_keypair();

void TEESingleton::print_mpi_to_hex(const char *name, const mbedtls_mpi *X) {
    static char buf[1024];
    size_t n;
    memset(buf, 0, sizeof(buf));
    mbedtls_mpi_write_string(X, 16, buf, sizeof(buf) - 1, &n);
    if (n) {
        printf("%s = 0x%s\n", name, buf);
    } else {
        printf("%s = TOOLONG\n", name);
    }
}