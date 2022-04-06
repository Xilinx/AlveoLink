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
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "basicHost.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc !=6  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <socket_file> <ip_file> <numPkts> <batchPkts> <timeOutCycles>" << std::endl;
        std::cout << "xnikSyncTest.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_sockFileName = argv[l_idx++];
    std::string l_ipFileName = argv[l_idx++];
    unsigned int l_numPkts = atoi(argv[l_idx++]);
    unsigned int l_batchPkts = atoi(argv[l_idx++]) - 1;
    unsigned int l_timeOutCycles = atoi(argv[l_idx++]) - 1;

    
    std::string l_hostName;
    std::string l_xclbinName;
    int l_devId = 0;
    std::string l_myIPstr;
    //uint32_t l_myIP = 0;
    uint32_t l_myID = 0;
    std::vector<std::string> l_ipTable;

    std::ifstream l_sockFile(l_sockFileName);
    std::string l_myHostName;
    std::string l_devIdStr;
    if (l_sockFile.is_open()) {
        std::string l_lineStr;
        if (getline(l_sockFile, l_lineStr)) {
            std::istringstream l_strStream(l_lineStr);
            l_strStream >> l_hostName >> l_myIPstr >> l_xclbinName >> l_devIdStr;
            l_devId = std::stoi(l_devIdStr);
            std::cout << "INFO: My ip address is: " << l_myIPstr << std::endl;
        }
        while (getline(l_sockFile, l_lineStr)) {
            std::istringstream l_strStream(l_lineStr);
            std::string l_sockIDstr, l_srcIPstr, l_desIPstr;
            l_strStream >> l_sockIDstr >> l_srcIPstr >> l_desIPstr;
            std::cout << "Sockets INFO: " << l_sockIDstr << " " << l_srcIPstr << " " << l_desIPstr << std::endl;
        }
        l_sockFile.close();
    }

    std::ifstream l_ipFile(l_ipFileName);
    if (l_ipFile.is_open()) {
        std::string l_ipStr;
        while (getline(l_ipFile, l_ipStr)) {
            if (l_ipStr == l_myIPstr) {
                l_myID = l_ipTable.size();
            }
            l_ipTable.push_back(l_ipStr);
        }
        l_ipFile.close();
    }
    
    int l_numDevs = l_ipTable.size()-1;

    int l_infID = 0;

    AlveoLink::common::FPGA l_card;
    basicHost<AL_netDataBits> l_basicHost;
    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;
    l_basicHost.init(&l_card);
    
    
    unsigned int l_transBytes = t_NetDataBytes * l_numPkts;
    unsigned int l_recBytes = l_transBytes;
    l_basicHost.createCU(0);
    uint8_t* l_transBuf = (uint8_t*)l_basicHost.createTransBuf(l_transBytes);
    if (l_recBytes > 0) {
        l_basicHost.createRecBufs(l_recBytes);
    }

    for (unsigned int i=0; i<l_numPkts; ++i) {
        std::vector<uint8_t> l_pkt(t_NetDataBytes);
        l_pkt[1] = 0;
        l_pkt[0] = (l_myID + 1) % l_numDevs;
        l_pkt[2] = AlveoLink::kernel::PKT_TYPE::workload;
        l_pkt[2] = l_pkt[2] << 4;
        std::memcpy(l_pkt.data()+3, (uint8_t*)(&i), 4);
        for (unsigned int j=7; j<t_NetDataBytes; ++j) {
            l_pkt[j] = 0;
        }
        std::memcpy(l_transBuf + i*t_NetDataBytes, l_pkt.data(), t_NetDataBytes); 
    }
    l_basicHost.sendBO();
    l_basicHost.runCU(l_myID, l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles);
    uint8_t* l_resBuf = (uint8_t*)(l_basicHost.getRes());

    int l_dataErr = 0;
    for (unsigned int i=0; i<l_numPkts; ++i) {
        unsigned int l_res = (l_resBuf[i*t_NetDataBytes+6]<<24);
        l_res += l_resBuf[i*t_NetDataBytes+5]<<16;
        l_res += l_resBuf[i*t_NetDataBytes+4]<<8;
        l_res += l_resBuf[i*t_NetDataBytes+3];
        if (l_res != i) {
            std::cout << "ERROR: Pkt " << i << " received " << l_res << std::endl;
            l_dataErr++;
        }
    }
    if (l_dataErr != 0) {
        std::cout << "ERROR: data error!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout <<"Test Pass!" << std::endl;
    return EXIT_SUCCESS;
}
