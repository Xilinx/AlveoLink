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
#include "HiveNet.hpp"
#include <map>
#include <string>

namespace AlveoLink {
namespace network_roce_v2 {
void HiveNet::initCU(const unsigned int p_id) {
    std::string l_cuName = "HiveNet_kernel_" + std::to_string(p_id) /*+ ":{HiveNet_kernel_" + std::to_string(p_id) + "}"*/;
    this->getIP(l_cuName);
    resetARPTable();
    setTimeOut(0xffff);
    setNretransmissions(3);
    setUDPPort(0x12b7);
    this->writeReg(ecnTimeout_off,50);

}

void HiveNet::setLocalID(const uint16_t localID){
    this->writeReg(localID_off,localID);
}
uint16_t HiveNet::getLocalID(){
    uint16_t localID = this->readReg(localID_off);
    return localID;
}
void HiveNet::setTimeOut(const unsigned long timeout){
    this->writeReg(timeout_off,timeout);
    this->writeReg(timeout_off+4,timeout>>32);
}

void HiveNet::setIPSubnet(const unsigned int ipSubnet){
    this->writeReg(IPsubnet_off,ipSubnet);
}
uint32_t HiveNet::getIPSubnet(){
    uint32_t ipSubnet = this->readReg(IPsubnet_off);
    return ipSubnet;
}
void HiveNet::setMACSubnet(const unsigned long macSubnet){
    this->writeReg(MACSubnet_off,macSubnet);
    this->writeReg(MACSubnet_off+4,macSubnet>>32);
}
uint64_t HiveNet::getMACSubnet(){
    uint64_t macSubnet = this->readReg(MACSubnet_off+4);
    macSubnet = macSubnet<<32;
    macSubnet += this->readReg(MACSubnet_off);
    macSubnet&=0xffffffffffffe000;
    return macSubnet;
}
void HiveNet::setNretransmissions(const unsigned short retransmissions){
    this->writeReg(retransmissions_off,retransmissions);  
}
uint16_t HiveNet::getNretransmissions(){
    uint16_t retransmissions = this->readReg(retransmissions_off);  
    return retransmissions;
}
void HiveNet::setUDPPort(const uint16_t udpAddr){
    this->writeReg(UDPPort_off,udpAddr);
}
void HiveNet::set_VLAN(uint8_t turnON_VLAN, uint32_t pfc_code, uint32_t vlan_id){
    if(turnON_VLAN){
        this->writeReg(0x210, 1);//turning on the Vlan tag in frame
        this->writeReg(0x218, pfc_code);
        this->writeReg(0x220, vlan_id);
    }
    else{
        this->writeReg(0x210, 0);
    }
}
void HiveNet::resetARPTable(){
    //this->writeReg(resetARP_off,0);
    this->writeReg(resetARP_off,1);
}
uint32_t HiveNet::getIPAddress(){
    uint16_t id=this->readReg(localID_off);
    uint32_t subnet=this->readReg(IPsubnet_off);
    uint32_t subnet_little = ((subnet >> 24) & 0xff) | ((subnet << 8) & 0xff0000) | ((subnet >> 8) & 0xff00) | ((subnet << 24) & 0xff000000);
    uint32_t ip_little=subnet_little + id;
    uint32_t ip_big=((ip_little>>24)&0xff) | ((ip_little<<8)&0xff0000) | ((ip_little>>8)&0xff00) | ((ip_little<<24)&0xff000000);
    return ip_big;
}
uint64_t HiveNet::getMACAddress(){
    uint64_t macSubAddress = this->readReg(MACSubnet_off + 4);
    macSubAddress = macSubAddress << 32;
    macSubAddress += this->readReg(MACSubnet_off);
    uint16_t id = this->readReg(localID_off);
    uint64_t macAddress = macSubAddress;
    macAddress &= 0xffffffffffffe000;
    macAddress = macAddress + id;
    return macAddress;
}
uint16_t HiveNet::getUDPPort(){
    uint16_t udpAddr = this->readReg(UDPPort_off);
    return udpAddr;
}

uint32_t HiveNet::getBuffSize(){
    return this->readReg(buffSize_off);
}
uint64_t HiveNet::getTimeOut(){
    uint64_t timeout = this->readReg(timeout_off + 4);
    timeout = timeout << 32;
    timeout += this->readReg(timeout_off);
    return timeout; 
}

}
}
