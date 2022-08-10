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
    void initCU(const unsigned int p_id) {
        std::string l_cuName =
            "HiveNet_kernel_" + std::to_string(p_id) /*+ ":{HiveNet_kernel_" + std::to_string(p_id) + "}"*/;
        this->getIP(l_cuName);
        resetARPTable();
        setTimeOut(0xffff);
        setNretranssmitions(0x3);
        setUDPPort(0x12b7);
    }

    void setLocalID(const uint16_t localID) { this->writeReg(localID_off, localID); }
    uint16_t getLocalID() {
        uint16_t localID = this->readReg(localID_off);
        return localID;
    }
    void setTimeOut(const unsigned long timeout) {
        this->writeReg(timeout_off, timeout);
        this->writeReg(timeout_off + 4, timeout >> 32);
    }

    void setIPSubnet(const unsigned int ipSubnet) { this->writeReg(IPsubnet_off, ipSubnet); }
    uint32_t getIPSubnet() {
        uint32_t ipSubnet = this->readReg(IPsubnet_off);
        return ipSubnet;
    }
    void setMACSubnet(const unsigned long macSubnet) {
        this->writeReg(MACSubnet_off, macSubnet);
        this->writeReg(MACSubnet_off + 4, macSubnet >> 32);
    }
    uint64_t getMACSubnet() {
        uint64_t macSubnet = this->readReg(MACSubnet_off + 4);
        macSubnet = macSubnet << 32;
        macSubnet += this->readReg(MACSubnet_off);
        macSubnet &= 0xffffffffffffe000;
        return macSubnet;
    }
    void setNretranssmitions(const unsigned short retranssmitions) {
        this->writeReg(retranssmitions_off, retranssmitions);
    }
    uint16_t getNretranssmitions() {
        uint16_t retranssmitions = this->readReg(retranssmitions_off);
        return retranssmitions;
    }
    void setUDPPort(const uint16_t udpAddr) { this->writeReg(UDPPort_off, udpAddr); }

    void resetARPTable() {
        this->writeReg(resetARP_off, 0);
        this->writeReg(resetARP_off, 1);
    }
    uint32_t getIPAddress() {
        uint16_t id = this->readReg(localID_off);
        uint32_t subnet = this->readReg(IPsubnet_off);
        uint32_t subnet_little = ((subnet >> 24) & 0xff) | ((subnet << 8) & 0xff0000) | ((subnet >> 8) & 0xff00) |
                                 ((subnet << 24) & 0xff000000);
        uint32_t ip_little = subnet_little + id;
        uint32_t ip_big = ((ip_little >> 24) & 0xff) | ((ip_little << 8) & 0xff0000) | ((ip_little >> 8) & 0xff00) |
                          ((ip_little << 24) & 0xff000000);
        return ip_big;
    }
    uint64_t getMACAddress() {
        uint64_t macSubAddress = this->readReg(MACSubnet_off + 4);
        macSubAddress = macSubAddress << 32;
        macSubAddress += this->readReg(MACSubnet_off);
        uint16_t id = this->readReg(localID_off);
        uint64_t macAddress = macSubAddress;
        macAddress &= 0xffffffffffffe000;
        macAddress = macAddress + id;
        return macAddress;
    }
    uint16_t getUDPPort() {
        uint16_t udpAddr = this->readReg(UDPPort_off);
        return udpAddr;
    }

    uint32_t getBuffSize() { return this->readReg(buffSize_off); }
    uint64_t getTimeOut() {
        uint64_t timeout = this->readReg(timeout_off + 4);
        timeout = timeout << 32;
        timeout += this->readReg(timeout_off);
        return timeout;
    }
};
}
}

#endif
