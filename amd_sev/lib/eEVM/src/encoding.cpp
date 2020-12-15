#include "eEVM/encoding.h"

#include <iterator>
#include <string>

using std::string;
using std::vector;

uint8_t get_n_th_byte(uint64_t in, int n) {
    if (n > 8) {
        // printf_sgx("n is too big\n");
        return 0xFF;
    }
    return (in >> (8 * n)) & 0xff;
}

/*!
 * encode in in big-endian order. Prepend zeros to make the result of 32-byte
 * aligned.
 * @param out
 * @param in input int
 * @param len length of int in byte
 * @return
 */
int append_as_uint256(bytes &out, uint64_t in, int len) {
    if (len > 32) {
        // printf_sgx("Error: too big\n");
        return -1;
    }
    // padding with 0
    for (int i = 0; i < 32 - len; i++) {
        out.push_back(0);
    }
    // push big-endian int
    for (int i = len - 1; i >= 0; i--) {
        out.push_back(get_n_th_byte(in, i));
    }
    return 0;
}

uint8_t bytesRequired(uint64_t _i) {
    return byte_length<uint64_t>(_i);
}

void bytes::replace(const bytes &in) {
    vector<uint8_t>::clear();
    vector<uint8_t>::insert(vector<uint8_t>::end(), in.begin(), in.end());
}

void bytes::from_hex(const char *src) {
    this->clear();
    auto b = lntee::from_hex(src);
    this->insert(this->begin(), b.begin(), b.end());
}

void bytes::to_rlp(bytes &out) {
    rlp_string(this->begin(), this->end(), out);
}

void bytes::dump(const string &title) {
    // #ifndef NDEBUG
    //     hexdump(title.c_str(), std::vector<uint8_t>::data(),
    //     std::vector<uint8_t>::size());
    // #endif
}

void bytes::toString() {
    dump("bytes");
}

bytes20::bytes20(const char *hex) {
    auto b = lntee::from_hex(hex);
    if (b.size() != SIZE)
        throw invalid_argument("wrong size");

    std::copy(b.begin(), b.end(), _b.begin());
}

void bytes20::from_binary(const char *binary) {
    for (int i = 0; i < 20; i++)
        _b[i] = *(binary + i);
}

void bytes20::from_hex(const char *hex) {
    auto b = lntee::from_hex(hex);
    if (b.size() != SIZE)
        throw invalid_argument("wrong size");

    std::copy(b.begin(), b.end(), _b.begin());
}

bytes32::bytes32(uint64_t in) {
    // push big-endian int (i.e. prepend 0 until 32 bytes)
    BYTE b_in = itob(in, 32);
    vector::insert(vector::end(), b_in.begin(), b_in.end());
}

bytes32::bytes32(std::string in) {
    if (in.length() > 32) {
        throw std::invalid_argument("too big");
    }
    // push string
    vector::insert(vector::end(), in.begin(), in.end());
    // rear padding with 0
    vector::insert(vector::end(), 32 - in.length(), 0);
}

void bytes32::replace(const BYTE &in) {
    if (in.size() > 32) {
        throw std::invalid_argument("too large");
    }

    vector<uint8_t>::clear();
    this->insert(this->begin(), in.begin(), in.end());
}

std::vector<uint8_t> itob(uint64_t num, size_t width) {
    std::vector<uint8_t> out;

    if (num == 0 && width == 0) {
        return out;
    }

    size_t len_len = byte_length<size_t>(num);
    for (long i = len_len - 1; i >= 0; i--) {
        out.push_back(static_cast<uint8_t>((num >> (8 * i)) & 0xFF));
    }

    // prepend zero until width
    if (width > out.size()) {
        out.insert(out.begin(), width - out.size(), 0x0);
    }

    return out;
}
