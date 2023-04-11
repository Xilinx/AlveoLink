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

#ifndef ROCEV2_HIVENET_HPP
#define ROCEV2_HIVENET_HPP

#include "xNativeFPGA.hpp"
#include <map>
#include <string>
namespace AlveoLink {
namespace network_roce_v2 {

constexpr size_t localID_off = 0x10;
constexpr size_t timeout_off = 0x18;
constexpr size_t IPsubnet_off = 0x24;
constexpr size_t MACSubnet_off = 0x2c;
constexpr size_t retransmissions_off = 0x38;
constexpr size_t UDPPort_off = 0x40;
constexpr size_t resetARP_off = 0x48;
constexpr size_t buffSize_off = 0x100;
constexpr size_t ecnTimeout_off = 0x180;

//--------------------------------------------------------------------------------//
// HiveNet Class
//--------------------------------------------------------------------------------//
class HiveNet : public AlveoLink::common::IP {
   public:
    HiveNet() = default;

    /**
    * @brief setting the network interface HiveNet IP is connected to
    *
    * @param p_id network interface ID (0/1 for Alveo U55C card)
    *
    */
    void initCU(const unsigned int p_id);

    /**
    * @brief setting setting the HiveNet ID. Each HiveNet should have unique ID under the same subnet. 
    *
    * @param localID Local HiveNet ID up to 8192
    *
    */
    void setLocalID(const uint16_t localID);

    /**
    * @brief setting the timeout parameter.
    *
    * @param timeout Timeout in nanoseconds for each packet in case of packet loss in network, default value 65535 ns.
    *
    */
    void setTimeOut(const unsigned long timeout);

    /**
    * @brief setting the ipSubnet, the IP subnet should be the same for all HiveNet IPs.
    *
    * @param ipSubnet The IP subnet the HiveNets are working in.
    *
    */
    void setIPSubnet(const unsigned int ipSubnet);

    /**
    * @brief setting the macSubnet, the MAC subnet should be the same for all HiveNet IPs.
    *
    * @param macSubnet The MAC subnet the HiveNets are working in.
    *
    */
    void setMACSubnet(const unsigned long macSubnet);

    /**
    * @brief setting the retransmissions.
    *
    * @param retransmissions number of retransmissions in case of packet loss in network, default value 3.
    *
    */
    void setNretransmissions(const unsigned short retransmissions);

    /**
    * @brief setting the udpPort, the UDP port should be the same on all HiveNet IPs.
    *
    * @param udpPort the UDP port number, default value 4971.
    *
    */
    void setUDPPort(const uint16_t udpAddr);

    /**
    * @brief resetting ARP table.
    */

   void set_VLAN(uint8_t turnON_VLAN, uint32_t pfc_code, uint32_t vlan_id);

   /**
    * @brief turing on VLAN with the parameters
    * 
    * @param turnON_VLAN enable the VLAN
    * 
    * @param pfc_code the pfc id for the vlan if the turnON_VLAN is true
    * 
    * @param vlan_id the vlan id for the vlan if the turnON_VLAN is true
    */
    void resetARPTable();

    /**
    * @brief readout timeout setting.
    */
    unsigned long getTimeOut();

    /**
    * @brief getting IPSubnet. 
    */
    uint32_t getIPSubnet();

    /**
    * @brief getting localID. 
    */
    uint16_t getLocalID();

    /**
    * @brief getting MACSubnet.
    */
    uint64_t getMACSubnet();

    /**
    * @brief getting number of retransmissions. 
    */
    uint16_t getNretransmissions();

    /**
    * @brief getting UDPPort number. 
    */
    uint16_t getUDPPort();

    /**
    * @brief getting IPAddress, each HiveNet IP has an IP address calculated from IPSubnet and localID. 
    */
    uint32_t getIPAddress();

    /**
    * @brief getting MACAddress, each HiveNet IP has an Mac address calculated from MACsubnet and localID.
    */
    uint64_t getMACAddress();

    /**
    * @brief return number of  packets in the HiveNet buffer waiting for their acknowledgment.
    */
    uint32_t getBuffSize();
};
}
}

#endif
