//
// Created by sgx on 9/7/20.
//

#ifndef LNTEE_UTIL_H
#define LNTEE_UTIL_H

#include <assert.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

int printf(const char* fmt, ...);

void debug(const char * str);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */