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

#include "hwManagerHost.hpp"
#include "testAppHost.hpp"
#include "dvNetLayer.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc <7  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <numDevs> <numPkts> <waitCycles> <flushCounter> [debug]" << std::endl;
        std::cout << "manager_compute.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_numDevs = atoi(argv[l_idx++]);
    unsigned int l_numPkts = atoi(argv[l_idx++]);
    unsigned int l_waitCycles = atoi(argv[l_idx++]);
    unsigned int l_flushCounter = atoi(argv[l_idx++]);
    std::string l_debug = "";
    if (argc == 8) {
        l_debug =  argv[l_idx++];
    }

    unsigned int l_batchPkts = 128;
    unsigned int l_timeOutCycles = 0;
    unsigned int l_computeWaitCycles = 2 << 30;

    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
    testAppHost<AL_netDataBits> l_testAppHost;
    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;

    l_dvNetLayer.init(&l_card);
    uint16_t* l_ids = l_dvNetLayer.getIds();
    std::bitset<AL_numInfs> l_linkUp = l_dvNetLayer.getLinkStatus();
    for (auto i=0; i<AL_numInfs; ++i) {
        std::cout << "INFO: port " << i << " has id " << std::dec << l_ids[i]  << std::endl;
        if (l_linkUp[i]) {
            std::cout << "INFO: port " << i << " link up." << std::endl;
        }
        else {
            std::cout << "INFO: port " << i << " link down." << std::endl;
            return EXIT_FAILURE;
        }
    }

    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter);
    l_manager.sendBO();
    l_manager.runCU();

    l_testAppHost.init(&l_card);
    l_testAppHost.createCU(0);

    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = l_transBytes;
    uint8_t* l_transBuf;
    uint8_t* l_resBuf;
    l_transBuf = (uint8_t*)l_testAppHost.createTransBuf(l_transBytes);
    l_testAppHost.createRecBufs(l_recBytes);
    
    for (unsigned int i=1; i<(l_numPkts+1); ++i) {
        std::vector<uint8_t> l_pkt(t_NetDataBytes);
        l_pkt[1] = 0;
        l_pkt[0] = (l_ids[0] + 1) % l_numDevs;
        l_pkt[2] = AlveoLink::kernel::PKT_TYPE::workload;
        l_pkt[2] = l_pkt[2] << 4;
        std::memcpy(l_pkt.data()+3, (uint8_t*)(&i), 4);
        for (unsigned int j=7; j<t_NetDataBytes; ++j) {
            l_pkt[j] = 0;
        }
        std::memcpy(l_transBuf + i*t_NetDataBytes, l_pkt.data(), t_NetDataBytes); 
    }
    l_testAppHost.sendBO();
    l_testAppHost.runCU(l_ids[0], l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles, l_computeWaitCycles);
    l_resBuf = (uint8_t*)(l_testAppHost.getRes());
    l_transBuf = (uint8_t*)(l_testAppHost.getTransBuf());
     
    if (l_debug == "debug") {
        std::vector<uint64_t> l_pktTxCnts = l_dvNetLayer.getLaneTxPktsCnt();
        for (auto i=0; i<AL_numInfs; ++i) {
            std::cout << "INFO: port " << i << std::endl;
            for (auto j=0; j<4; ++j) {
                std::cout << "    lane " << j << " has transmitted " << l_pktTxCnts[i*4+j] << " packets." << std::endl;
            }
        }
        std::vector<uint64_t> l_pktCnts = l_dvNetLayer.getLaneRxPktsCnt();
        for (auto i=0; i<AL_numInfs; ++i) {
            std::cout << "INFO: port " << i << std::endl;
            for (auto j=0; j<4; ++j) {
                std::cout << "    lane " << j << " has received " << l_pktCnts[i*4+j] << " packets." << std::endl;
            }
        }
        for (auto i=0; i<AL_numInfs; ++i) {
            for (auto j=0; j<4; ++j) {
                std::vector<uint32_t> l_lastRxPkt = l_dvNetLayer.getLastTxPkt(i, j);
                std::cout << "INFO: last tx pkt for port " << i << " lane " << j << " is: " << std::endl;
                for (auto k=0; k<4; ++k) {
                    std::cout << "    w" << k << " = 0x" << std::hex << l_lastRxPkt[k];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        for (auto i=0; i<AL_numInfs; ++i) {
            for (auto j=0; j<4; ++j) {
                std::vector<uint32_t> l_lastRxPkt = l_dvNetLayer.getLastRxPkt(i, j);
                std::cout << "INFO: last rx pkt for port " << i << " lane " << j << " is: " << std::endl;
                for (auto k=0; k<4; ++k) {
                    std::cout << "    w" << k << " = 0x" << std::hex << l_lastRxPkt[k];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::dec;
    unsigned int l_numTxPkts;
    unsigned int l_numRxPkts;
    l_numTxPkts = (l_transBuf[3]<<24);
    l_numTxPkts  += (l_transBuf[2]<<16);
    l_numTxPkts  += (l_transBuf[1]<<8);
    l_numTxPkts  += l_transBuf[0];

    l_numRxPkts = (l_resBuf[3]<<24);
    l_numRxPkts  += (l_resBuf[2]<<16);
    l_numRxPkts  += (l_resBuf[1]<<8);
    l_numRxPkts  += l_resBuf[0];
    std::cout << "INFO: kernel " << l_ids[0] << " num of transmitted pkts is: " << l_numTxPkts << std::endl;
    std::cout << "INFO: kernel " << l_ids[0] << " num of received pkts is: " << l_numRxPkts << std::endl;

    l_manager.finish();
    std::cout << "INFO: system run finished!" << std::endl;

    if ((l_numTxPkts != l_numPkts) || (l_numRxPkts != l_numPkts)) {
        std::cout << "ERROR: pkts transmitted or pkts received != " <<l_numPkts << std::endl;
        return EXIT_FAILURE;
    }

    int l_dataErr = 0;
    for (unsigned int i=1; i<(l_numPkts+1); ++i) {
        unsigned int l_res = (l_resBuf[i*t_NetDataBytes+6]<<24);
        l_res += l_resBuf[i*t_NetDataBytes+5]<<16;
        l_res += l_resBuf[i*t_NetDataBytes+4]<<8;
        l_res += l_resBuf[i*t_NetDataBytes+3];
        if (l_res != i) {
            std::cout << "ERROR: Kernel " << l_ids[0] << " Pkt " << i << " received " << l_res << std::endl;
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
