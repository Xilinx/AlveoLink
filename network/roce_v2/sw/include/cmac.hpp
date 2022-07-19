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
    void initCU(const unsigned int p_id) {
        std::string l_cuName = "cmac_" + std::to_string(p_id);
        this->getIP(l_cuName);
    }
    std::map<std::string, bool> linkStatus() {
        std::map<std::string, bool> status_dict;
        uint32_t l_rxStatus = this->readReg(stat_rx_status);
        std::bitset<32> l_rxBits(l_rxStatus);
        uint32_t l_txStatus = this->readReg(stat_tx_status);
        std::bitset<32> l_txBits(l_txStatus);
        status_dict.insert({"rx_status", l_rxBits.test(0)});
        status_dict.insert({"rx_aligned", l_rxBits.test(1)});
        status_dict.insert({"rx_misaligned", l_rxBits.test(2)});
        status_dict.insert({"rx_aligned_err", l_rxBits.test(3)});
        status_dict.insert({"rx_hi_ber", l_rxBits.test(4)});
        status_dict.insert({"rx_remote_fault", l_rxBits.test(5)});
        status_dict.insert({"rx_local_fault", l_rxBits.test(6)});
        status_dict.insert({"rx_got_signal_os", l_rxBits.test(14)});
        status_dict.insert({"tx_local_fault", l_txBits.test(0)});
        return status_dict;
    }
    void turnOn_RS_FEC(bool enable_fec) {
        if (enable_fec) {
            this->writeReg(rsfec_config_enable, 0x3);
        } else {
            this->writeReg(rsfec_config_enable, 0);
        }
    }
    bool get_RS_FEC_enable() {
        unsigned short enable_fec_status = this->readReg(rsfec_config_enable);
        if (enable_fec_status == 0x3)
            return true;
        else
            return false;
    }
};
}
}

#endif
