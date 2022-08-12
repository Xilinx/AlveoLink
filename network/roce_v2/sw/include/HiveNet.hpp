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
constexpr size_t retranssmitions_off = 0x38;
constexpr size_t UDPPort_off = 0x40;
constexpr size_t resetARP_off = 0x48;
constexpr size_t buffSize_off = 0x100;

class HiveNet : public AlveoLink::common::IP {
   public:
    HiveNet() = default;
    void initCU(const unsigned int p_id);

    void setLocalID(const uint16_t localID);

    void setTimeOut(const unsigned long timeout);

    void setIPSubnet(const unsigned int ipSubnet);

    void setMACSubnet(const unsigned long macSubnet);

    void setNretranssmitions(const unsigned short retranssmitions);

    void setUDPPort(const uint16_t udpAddr);

    void resetARPTable();

    unsigned long getTimeOut();

    uint32_t getIPSubnet();

    uint16_t getLocalID();

    uint64_t getMACSubnet();

    uint16_t getNretranssmitions();

    uint16_t getUDPPort();

    uint32_t getIPAddress();

    uint64_t getMACAddress();

    uint32_t getBuffSize();
};
}
}

#endif
