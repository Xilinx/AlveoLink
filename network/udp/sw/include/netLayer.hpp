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

#ifndef ALVEOLINK_NETWORKLAYER_HPP
#define ALVEOLINK_NETWORKLAYER_HPP

#include "cmac.hpp"
#include "udp_ip.hpp"
#include "netException.hpp"

namespace AlveoLink {
namespace network_udp {

template <unsigned int t_numInfs, unsigned int t_MaxNumSockets=16>
class NetLayer {
   public:
    NetLayer(){};
    void init(AlveoLink::common::FPGA* p_fpga = nullptr) {
        for (unsigned int i = 0; i < t_numInfs; ++i) {
            m_cmacs[i].fpga(p_fpga);
            m_cmacs[i].initCU(i);
            m_udpIPs[i].fpga(p_fpga);
            m_udpIPs[i].initCU(i);
        }
    }

    std::map<std::string, bool> linkStatus(const unsigned int p_id) { return m_cmacs[p_id].linkStatus(); }

    void arpDiscovery(const unsigned int p_id) { m_udpIPs[p_id].arpDiscovery(); }

    std::map<int, std::pair<std::string, std::string> > readARPTable(const unsigned int p_id, int num_entries = 256) {
        if (num_entries < 0) {
            throw NetInvalidValue("num_entries is smaller than zero");
        } else if (num_entries > 256) {
            throw NetInvalidValue("num_entries is greater than 256");
        }
        return m_udpIPs[p_id].readARPTable(num_entries);
    }

    void configureSocket(const unsigned int p_id,
                         int index,
                         std::string p_theirIP,
                         uint16_t p_theirPort,
                         uint16_t p_myPort,
                         bool p_valid) {
        m_udpIPs[p_id].configureSocket(index, p_theirIP, p_theirPort, p_myPort, p_valid);
    }

    SocketType getHostSocket(const unsigned int p_id, int index) { return m_udpIPs[p_id].getHostSocket(index); }

    std::map<int, AlveoLink::network_udp::SocketType> populateSocketTable(const unsigned int p_id) {
        return m_udpIPs[p_id].populateSocketTable();
    }
    void updateIpAddress(const unsigned int p_id, const uint32_t p_ipAddr) {
        m_udpIPs[p_id].setIPaddr(p_ipAddr);
    }

    void populateSocketTable(const unsigned int p_id, std::vector<SocketType>& p_socketTable) {
        m_udpIPs[p_id].populateSocketTable(p_socketTable);
        m_udpIPs[p_id].printSocketTable(p_socketTable.size());
    }

    uint32_t getUDPinPkts(const uint16_t p_infId) { return (m_udpIPs[p_infId].getUDPinPkts()); }
    uint32_t getUDPoutPkts(const uint16_t p_infId) { return (m_udpIPs[p_infId].getUDPoutPkts()); }
    uint32_t getUDPappInPkts(const uint16_t p_infId) { return (m_udpIPs[p_infId].getUDPappInPkts()); }
    uint32_t getUDPappOutPkts(const uint16_t p_infId) { return (m_udpIPs[p_infId].getUDPappOutPkts()); }

   protected:
    CMAC m_cmacs[t_numInfs];
    UdpIp<t_MaxNumSockets> m_udpIPs[t_numInfs];
};
}
}

#endif
