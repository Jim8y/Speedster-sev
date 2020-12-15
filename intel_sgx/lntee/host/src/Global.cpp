//
// Created by root on 9/3/19.
//

#include "Global.h"
//#include <boost/algorithm/string/split.hpp>
//#include <boost/algorithm/string/classification.hpp>
std::string Global::ip = "127.0.0.1";
int Global::port = -1;
char Global::pubkey[PUBKEY_LEN * 2 + 3] = {'\0'};
char Global::addr[ADDRESS_LEN * 2 + 3] = {'\0'};
oe_enclave_t *Global::enclave = NULL;

static char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static uint8_t hex2int(char input)
{
    if (input >= '0' && input <= '9')
        return static_cast<uint8_t>(input - '0');
    if (input >= 'A' && input <= 'F')
        return static_cast<uint8_t>(input - 'A' + 10);
    if (input >= 'a' && input <= 'f')
        return static_cast<uint8_t>(input - 'a' + 10);
    return -1;
}
void Global::from_hex(const char *src, char *target)
{
    while (*src && src[1])
    {
        *(target++) = hex2int(*src) * HEX_BASE + hex2int(src[1]);
        src += 2;
    }
}

std::string Global::to_hex(const unsigned char *data, size_t len)
{
    std::stringbuf sb;
    for (unsigned i = 0; i < len; ++i)
    {
        sb.sputc(hexmap[(data[i] & 0xF0) >> 4]);
        sb.sputc(hexmap[data[i] & 0x0F]);
    }
    return sb.str();
}
