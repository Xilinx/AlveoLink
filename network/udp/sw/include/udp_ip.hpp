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


#ifndef ALVEOLINK_NETWORK_UDPIP_HPP
#define ALVEOLINK_NETWORK_UDPIP_HPP

#include <vector>
#include <map>
#include <cmath>
#include "netException.hpp"
#include "xNativeFPGA.hpp"
#include "utils.hpp"

namespace AlveoLink {
namespace network_udp {

constexpr size_t mac_address = 0x0010;
constexpr size_t ip_address = 0x0018;
constexpr size_t gateway = 0x001C;
constexpr size_t arp_discovery = 0x3010;
constexpr size_t arp_mac_addr_offset = 0x3800;
constexpr size_t arp_ip_addr_offset = 0x3400;
constexpr size_t arp_valid_offset = 0x3100;
constexpr size_t udp_theirIP_offset = 0x2010;
constexpr size_t udp_theirPort_offset = 0x2090;
constexpr size_t udp_myPort_offset = 0x2110;
constexpr size_t udp_valid_offset = 0x2190;
constexpr size_t udp_number_sockets = 0x2210;
constexpr size_t udp_in_packets = 0x10D0;
constexpr size_t udp_out_packets = 0x1100;
constexpr size_t udp_app_in_packets = 0x10E8;
constexpr size_t udp_app_out_packets = 0x1118;

constexpr size_t udp_in_bytes = 0x10C8;
constexpr size_t udp_out_bytes = 0x10F8;
constexpr size_t udp_app_in_bytes = 0x10E0;
constexpr size_t udp_app_out_bytes = 0x1110;

constexpr size_t ethhi_out_bytes = 0x1098;
constexpr size_t eth_out_bytes = 0x10b0;

struct SocketType {
    std::string theirIP;
    uint32_t theirIPint;
    // uint16_t theirPort = 38746; //for cpu
    uint16_t theirPort = 62781;
    uint16_t myPort = 62781;
    bool valid = false;
};

template <unsigned int t_MaxNumSockets=16>
class UdpIp : public AlveoLink::common::IP {
   public:
    UdpIp() { m_numSockets = 0; };
   public:
    void initCU(const unsigned int p_id) {
        std::string l_cuName = "networklayer:{networklayer_" + std::to_string(p_id) + "}";
        getIP(l_cuName);
    }
    void setIPaddr(const uint32_t p_ipAddr) { 
        m_ip = p_ipAddr; 
        updateIPAddress(m_ip);
    }
    uint32_t getIpAddr() { return m_ip; }
    unsigned int getNumSockets() { return m_numSockets; }
    uint32_t updateIPAddress(const std::string p_ipAddrStr) {
        uint32_t p_ipAddr = getIPint(p_ipAddrStr);
        writeReg(ip_address, p_ipAddr);
        uint32_t l_gatewayAddr = (p_ipAddr & 0xFFFFFF00) + 1;
        writeReg(gateway, l_gatewayAddr);
        uint32_t l_curMacAddr;
        l_curMacAddr = readReg(mac_address);
        uint32_t l_newMacAddr = (l_curMacAddr & 0xFFFFFFFFFF00) + (p_ipAddr & 0xFF);
        writeReg(mac_address, l_newMacAddr);
        return p_ipAddr;
    }
    void updateIPAddress(const uint32_t p_ipAddr) {
        writeReg(ip_address, p_ipAddr);
        uint32_t l_gatewayAddr = (p_ipAddr & 0xFFFFFF00) + 1;
        writeReg(gateway, l_gatewayAddr);
        uint32_t l_curMacAddr;
        l_curMacAddr = readReg(mac_address);
        uint32_t l_newMacAddr = (l_curMacAddr & 0xFFFFFFFFFF00) + (p_ipAddr & 0xFF);
        writeReg(mac_address, l_newMacAddr);
    }
    void arpDiscovery() {
        writeReg(arp_discovery, 0);
        writeReg(arp_discovery, 1);
        writeReg(arp_discovery, 0);
    }

    std::map<int, std::pair<std::string, std::string> > readARPTable(int num_entries) {
        uint32_t mac_addr_offset = arp_mac_addr_offset;
        uint32_t ip_addr_offset = arp_ip_addr_offset;
        uint32_t valid_addr_offset = arp_valid_offset;
        std::map<int, std::pair<std::string, std::string> > table;
        unsigned long long valid_entry;

        for (int i = 0; i < num_entries; i++) {
            if ((i % 4) == 0) {
                valid_entry = readReg(valid_addr_offset + (i / 4) * 4);
            }
            unsigned long long isvalid = (valid_entry >> ((i % 4) * 8)) & 0x1;
            if (isvalid) {
                unsigned long long mac_lsb = readReg(mac_addr_offset + (i * 2 * 4));
                unsigned long long mac_msb = readReg(mac_addr_offset + ((i * 2 + 1) * 4));
                unsigned long long ip_addr = readReg(ip_addr_offset + (i * 4));
                unsigned long long mac_addr = pow(2, 32) * mac_msb + mac_lsb;
                unsigned long long mac_hex = _byteOrderingEndianess(mac_addr, 6);
                std::stringstream mac_hex_stringstream;
                mac_hex_stringstream << std::hex << mac_hex << std::dec;
                std::string mac_hex_string = mac_hex_stringstream.str();
                mac_hex_string = std::string(12 - mac_hex_string.length(), '0') + mac_hex_string;
                std::string mac_str = "";
                for (int j = 0; j < (int)mac_hex_string.length(); j++) {
                    mac_str = mac_str + mac_hex_string.at(j);
                    if ((j % 2 != 0) && (j != (int)mac_hex_string.length() - 1)) {
                        mac_str = mac_str + ":";
                    }
                }
                unsigned long long ip_addr_print = _byteOrderingEndianess(ip_addr);
                unsigned char ipBytes[4];
                ipBytes[0] = ip_addr_print & 0xFF;
                ipBytes[1] = (ip_addr_print >> 8) & 0xFF;
                ipBytes[2] = (ip_addr_print >> 16) & 0xFF;
                ipBytes[3] = (ip_addr_print >> 24) & 0xFF;
                std::stringstream ip_addr_printstream;
                ip_addr_printstream << int(ipBytes[3]) << "." << int(ipBytes[2]) << "." << int(ipBytes[1]) << "."
                                    << int(ipBytes[0]);
                std::string ip_addr_print_string = ip_addr_printstream.str();
                table.insert({i, {mac_str, ip_addr_print_string}});
            }
        }
        return table;
    }

    void configureSocket(int index, std::string p_theirIP, uint16_t p_theirPort, uint16_t p_myPort, bool p_valid) {
        SocketType l_socket = {p_theirIP, getIPint(p_theirIP), p_theirPort, p_myPort, p_valid};
        m_sockets[index] = l_socket;
    }

    SocketType getHostSocket(int index) {
        return m_sockets[index];
    }
    void populateSocketTable(std::vector<SocketType>& p_socketTable) {
        uint32_t theirIP_offset = udp_theirIP_offset;
        uint16_t theirPort_offset = udp_theirPort_offset;
        uint16_t myPort_offset = udp_myPort_offset;
        uint16_t valid_offset = udp_valid_offset;

        int numSocketsHW = readReg(udp_number_sockets);

        int l_socketTBsize = p_socketTable.size();
        if (numSocketsHW < l_socketTBsize) {
            std::string errMsg = "Socket list length " + std::to_string(l_socketTBsize) +
                                 " is bigger than the number of sockets in hardware " + std::to_string(numSocketsHW);
            throw NetInvalidValue(errMsg);
        }

        for (int i = 0; i < l_socketTBsize; i++) {
            uint32_t ti_offset = theirIP_offset + i * 8;
            uint32_t tp_offset = theirPort_offset + i * 8;
            uint32_t mp_offset = myPort_offset + i * 8;
            uint32_t v_offset = valid_offset + i * 8;

            uint32_t theirIP = p_socketTable[i].theirIPint;
            writeReg(ti_offset, theirIP);
            writeReg(tp_offset, p_socketTable[i].theirPort);
            writeReg(mp_offset, p_socketTable[i].myPort);
            writeReg(v_offset, p_socketTable[i].valid);
        }
    }
    
    void printSocketTable(const unsigned int p_numSockets) {
        uint32_t theirIP_offset = udp_theirIP_offset;
        uint16_t theirPort_offset = udp_theirPort_offset;
        uint16_t myPort_offset = udp_myPort_offset;
        uint16_t valid_offset = udp_valid_offset;
        for (unsigned int i=0; i<p_numSockets; ++i) {
            uint32_t ti_offset = theirIP_offset + i * 8;
            uint32_t tp_offset = theirPort_offset + i * 8;
            uint32_t mp_offset = myPort_offset + i * 8;
            uint32_t v_offset = valid_offset + i * 8;
            uint32_t isValid = readReg(v_offset);
            if (isValid == 0) {
                throw NetInvalidValue("Socket not set properly.");
            }
            else {
                uint32_t ti = readReg(ti_offset);
                uint32_t tp = readReg(tp_offset);
                uint32_t mp = readReg(mp_offset);

                unsigned char ipBytes[4];
                ipBytes[0] = ti & 0xFF;
                ipBytes[1] = (ti >> 8) & 0xFF;
                ipBytes[2] = (ti >> 16) & 0xFF;
                ipBytes[3] = (ti >> 24) & 0xFF;
                std::stringstream ti_printstream;
                ti_printstream << int(ipBytes[3]) << "." << int(ipBytes[2]) << "." << int(ipBytes[1]) << "."
                               << int(ipBytes[0]);
                std::cout << "Socket " << i << ": ";
                std::cout << " theirIP = " << ti_printstream.str();
                std::cout << " theirPort = " << tp;
                std::cout <<" myPort = " << mp << std::endl;
            }
            
        }
    }
public: //methods for debugging and testing
    uint32_t getUDPinPkts() {
        uint32_t l_res = readReg(udp_in_packets);
        uint32_t l_bytes = readReg(udp_in_bytes);
        std::cout << "udp in bytes = " << l_bytes << std::endl;
        return l_res;
    }
    uint32_t getUDPoutPkts() {
        uint32_t l_res = readReg(udp_out_packets);
        uint32_t l_bytes = readReg(udp_out_bytes);
        std::cout << "udp out bytes = " << l_bytes << std::endl;
        l_bytes = readReg(ethhi_out_bytes);
        std::cout << "ethhi_out_bytes = " << l_bytes << std::endl;
        l_bytes = readReg(eth_out_bytes);
        std::cout << "eth_out_bytes = " << l_bytes << std::endl;
        return l_res;
    }
    uint32_t getUDPappInPkts() {
        uint32_t l_res = readReg(udp_app_in_packets);
        uint32_t l_bytes = readReg(udp_app_in_bytes);
        std::cout << "udp app in bytes = " << l_bytes << std::endl;
        return l_res;
    }
    uint32_t getUDPappOutPkts() {
        uint32_t l_res = readReg(udp_app_out_packets);
        uint32_t l_bytes = readReg(udp_app_out_bytes);
        std::cout << "udp app out bytes = " << l_bytes << std::endl;
        return l_res;
    }

   private:
    SocketType m_sockets[t_MaxNumSockets];
    uint32_t m_ip;
    uint16_t m_numSockets;
};
}
}

#endif
