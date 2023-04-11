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
constexpr size_t rsfec_config_enable =                        0x107C;
constexpr size_t stat_tx_status =                             0x0200;
constexpr size_t stat_rx_status =                             0x0204;
constexpr size_t CONFIGURATION_RX_FLOW_CONTROL_CONTROL_REG1 = 0x0084;
constexpr size_t CONFIGURATION_RX_FLOW_CONTROL_CONTROL_REG2 = 0x0088;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_QUANTA_REG1 =  0x0048;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_QUANTA_REG2 =  0x004C;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_QUANTA_REG3 =  0x0050;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_QUANTA_REG4 =  0x0054;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_QUANTA_REG5 =  0x0058;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_REFRESH_REG1 = 0x0034;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_REFRESH_REG2 = 0x0038;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_REFRESH_REG3 = 0x003C;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_REFRESH_REG4 = 0x0040;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_REFRESH_REG5 = 0x0044;
constexpr size_t CONFIGURATION_TX_FLOW_CONTROL_CONTROL_REG1 = 0x0030;

constexpr size_t flow_control_block_off =                     0x2000;

class KernelCMAC : public AlveoLink::common::IP {
   public:
    KernelCMAC() = default;
    void initCU(const unsigned int p_id);
    std::map<std::string, bool> linkStatus();
    void turnOn_RS_FEC(bool enable_fec);
    bool get_RS_FEC_enable();
    void set_FlowControlType(uint32_t pfc_code);
};
}
}

#endif
