/*
 * Copyright 2019-2022 Xilinx, Inc.
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


#ifndef ALVEOLINK_NETWORK_UTILS_HPP
#define ALVEOLINK_NETWORK_UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "netException.hpp"

namespace AlveoLink {
namespace network_udp {

uint32_t getIPint(const std::string p_ipAddrStr);
std::string getIPstr(const uint32_t p_ipAddr);
unsigned long long _byteOrderingEndianess(unsigned long long num, int length = 4);

}
}

#endif
