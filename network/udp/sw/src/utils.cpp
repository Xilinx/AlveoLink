/* opyright 2019-2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include "utils.hpp"

namespace AlveoLink {
namespace network_udp {

uint32_t getIPint(const std::string p_ipAddrStr) {
    std::vector<std::string> l_ipAddrVec;
    std::stringstream l_str(p_ipAddrStr);
    std::string l_ipAddrStr;
    if (std::getline(l_str, l_ipAddrStr, '.').fail()) {
        throw NetInvalidIp("IP address is ill-formed.");
        return 0;
    } else {
        l_ipAddrVec.push_back(l_ipAddrStr);
    }
    while (std::getline(l_str, l_ipAddrStr, '.')) {
        l_ipAddrVec.push_back(l_ipAddrStr);
    }
    if (l_ipAddrVec.size() != 4) {
        throw NetInvalidIp("IP address is ill-formed.");
        return 0;
    }
    uint32_t l_ipAddr = 0;
    for (auto i = 0; i < 4; ++i) {
        l_ipAddr = l_ipAddr << 8;
        uint32_t l_val = std::stoi(l_ipAddrVec[i]);
        if (l_val > 255) {
            std::string l_errStr = l_ipAddrVec[i] + " should be less than 255.";
            throw NetInvalidIp(l_errStr);
            return 0;
        }
        l_ipAddr += l_val;
    }
    return l_ipAddr;
}

std::string getIPstr(const uint32_t p_ipAddr) {
    std::string l_ipStr;
    for (auto i=0; i<4; ++i) {
        uint32_t l_ipAddr = p_ipAddr;
        l_ipAddr = l_ipAddr >> (4-i-1)*8;
        uint8_t l_digit = l_ipAddr & 0xff;
        l_ipStr = l_ipStr + std::to_string(l_digit);
        if (i != 3) {
            l_ipStr += ".";
        }
    }
    return l_ipStr;
}

unsigned long long _byteOrderingEndianess(unsigned long long num, int length) {
    unsigned long long aux = 0;
    for (int i = 0; i < length; i++) {
        unsigned long long byte_index = num >> ((length - 1 - i) * 8) & 0xFF;
        aux += byte_index << (i * 8);
    }
    return aux;
}

}
}

