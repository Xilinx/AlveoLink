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

#ifndef ROCEV2_NETLAYER_HPP
#define ROCEV2_NETLAYER_HPP

#include "cmac.hpp"
#include "HiveNet.hpp"

namespace AlveoLink {
namespace network_roce_v2 {

template <unsigned int t_NumPorts>
class NetLayer {
    public:
        NetLayer() {}
        void init(AlveoLink::common::FPGA* p_fpga=nullptr) {
            for (unsigned int i = 0; i < t_NumPorts; ++i) {
                m_cmac[i].fpga(p_fpga);
                m_cmac[i].initCU(i);
                m_hiveNet[i].fpga(p_fpga);
                m_hiveNet[i].initCU(i);
            }
        }
        void setID(const unsigned int p_portId, const unsigned int p_netId) {
            m_hiveNet[p_portId].setLocalID(p_netId);
        }
        void setMACSubnet(const unsigned int p_portId, const unsigned long p_macSubnet) {
            m_hiveNet[p_portId].setMACSubnet(p_macSubnet);
        }
        void setIPSubnet(const unsigned int p_portId, const unsigned int p_ipSubnet) {
            m_hiveNet[p_portId].setIPSubnet(p_ipSubnet);
        }
        void setVlanPFC(const unsigned int p_portId, uint8_t turnON_VLAN, uint32_t pfc_code, uint32_t vlan_id){
            m_hiveNet[p_portId].set_VLAN(turnON_VLAN, pfc_code, vlan_id);
        }
        void setVlanPFC(const unsigned int p_portId, uint8_t turnON_VLAN){
            m_hiveNet[p_portId].set_VLAN(turnON_VLAN, 0, 0);
        }
        void turnOn_RS_FEC(const unsigned int p_portId, const bool p_setFEC) {
            m_cmac[p_portId].turnOn_RS_FEC(p_setFEC);
        }
        void turnOn_flow_control(const unsigned int p_portId,const bool global){
            if(global){
                m_cmac[p_portId].set_FlowControlType(0x100);
            }
        }
        void turnOn_flow_control(const unsigned int p_portId,const bool global,const short pfc_code){
            m_cmac[p_portId].set_FlowControlType(pfc_code);
        }

        bool linksUp() {
            std::map<std::string, bool> l_linkStatus[t_NumPorts];
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                l_linkStatus[i] = m_cmac[i].linkStatus();
            }
            unsigned int l_numLinksUp = 0;
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                auto it = l_linkStatus[i].find("rx_aligned");
                if (it != l_linkStatus[i].end()) {
                    std::cout << "INFO: cmac " << i << " rx_aligned." << std::endl;
                    l_numLinksUp++;
                }
            }
            if (l_numLinksUp == t_NumPorts) {
                return true;
            }
            else {
                return false;
            }
        }
    private:
        KernelCMAC m_cmac[t_NumPorts];
        HiveNet m_hiveNet[t_NumPorts];       
};

}
}
#endif
