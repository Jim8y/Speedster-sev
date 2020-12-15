//
// Created by sgx on 9/7/20.
//

#include "../include/util.h"
#include "lntee_t.h"
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>
/*
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

void debug(const char * str){
    printf("DEBUG FROM EEVM : %s",str);
}