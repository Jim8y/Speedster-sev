//
// Created by compass on 9/15/19.
//

#include <contract/Helper.h>

void Helper::append_argument(const uint256_t &arg) {
    const auto pre_size = function_call.size();
    function_call.resize(pre_size + 32u);
    eevm::to_big_endian(arg, function_call.data() + pre_size);
}

void Helper::append_argument(std::vector<uint8_t> &code, const uint256_t &arg) {
    // To ABI encode a function call with a uint256_t (or Address) argument,
    // simply append the big-endian byte representation to the code (function
    // selector, or bin). ABI-encoding for more complicated types is more
    // complicated, so not shown in this sample.
    const auto pre_size = code.size();
    code.resize(pre_size + 32u);
    eevm::to_big_endian(arg, code.data() + pre_size);
}

std::string Helper::strip(const std::string &s) {
    return (s.size() >= 2 && s[1] == 'x') ? s.substr(2) : s;
}

std::vector<uint8_t> Helper::to_bytes(const std::string &_s) {
    auto s = strip(_s);
    const size_t byte_len = (s.size() + 1) / 2; // round up
    std::vector<uint8_t> v(byte_len);
    // Handle odd-length strings
    size_t n = 0;
    if (s.size() % 2 != 0) {
        v[0] = static_cast<uint8_t>(strtoul(s.substr(0, 1).c_str(), nullptr, 16));
        ++n;
    }

    auto x = n;
    for (auto i = n; i < byte_len; ++i, x += 2) {
        v[i] = static_cast<uint8_t>(strtoul(s.substr(x, 2).c_str(), nullptr, 16));
    }
    return v;
}