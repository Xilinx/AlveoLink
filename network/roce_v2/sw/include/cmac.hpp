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

#ifndef ROCEV2_CMAC_HPP
#define ROCEV2_CMAC_HPP

#include <map>
#include <string>
#include "xNativeFPGA.hpp"

namespace AlveoLink {
namespace network_roce_v2 {
constexpr size_t rsfec_config_enable = 0x107C;
constexpr size_t stat_tx_status = 0x0200;
constexpr size_t stat_rx_status = 0x0204;

class KernelCMAC : public AlveoLink::common::IP {
   public:
    KernelCMAC() = default;
    void initCU(const unsigned int p_id);
    std::map<std::string, bool> linkStatus();
    void turnOn_RS_FEC(bool enable_fec);
    bool get_RS_FEC_enable();
};
}
}

#endif
