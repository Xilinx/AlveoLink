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
#include <cassert>
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

#include "dvNetLayer.hpp"
#include "testVerifyHost.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc < 4  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbinfile> <devId> <numPkts> [debug]" << std::endl;
        std::cout << "test.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    int l_devId = atoi(argv[l_idx++]);
    unsigned int l_numPkts = atoi(argv[l_idx++]);
    std::string l_debug = "";
    if (argc >4 ) {
        l_debug = argv[l_idx++];
    }
    unsigned int l_batchPkts = 128; //atoi(argv[l_idx++]) - 1;
    unsigned int l_timeOutCycles = 0; //atoi(argv[l_idx++]) - 1;
    unsigned int l_waitCycles = 2 << 30; //200; //atoi(argv[l_idx++]);

    if (argc > 5) {
        l_batchPkts = atoi(argv[l_idx++]);
    }
    if (argc > 6) {
        l_timeOutCycles = atoi(argv[l_idx++]);
    }
    

    uint32_t l_myID[AL_numInfs];
    int l_numDevs = AL_numInfs; 
    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    testVerifyHost<AL_netDataBits> l_testVerifyHost[AL_numInfs];
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
            l_myID[i] = l_ids[i];
        }
        else {
            std::cout << "INFO: port " << i << " link down." << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (auto i=0; i<AL_numInfs; ++i) {
        l_testVerifyHost[i].init(&l_card);
        l_testVerifyHost[i].createCU(i);
    }
    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = l_transBytes;
    assert(l_recBytes > 0);
    uint8_t* l_transBuf[AL_numInfs];
    uint8_t* l_resBuf[AL_numInfs];
    uint8_t* l_verifySentDestBuf[AL_numInfs];
    uint8_t* l_verifyRecDestBuf[AL_numInfs];

    for (auto i=0; i<AL_numInfs; ++i) {
        l_transBuf[i] = (uint8_t*)l_testVerifyHost[i].createTransBuf(l_transBytes);
        l_testVerifyHost[i].createRecBufs(l_recBytes);
    }

    for (auto k=0; k<AL_numInfs; ++k) {
        for (unsigned int i=1; i<(l_numPkts+1); ++i) {
            std::vector<uint8_t> l_pkt(t_NetDataBytes);
            l_pkt[1] = 0;
            l_pkt[0] = (l_myID[k] + 1) % l_numDevs;
            l_pkt[2] = AlveoLink::kernel::PKT_TYPE::workload;
            l_pkt[2] = l_pkt[2] << 4;
            std::memcpy(l_pkt.data()+3, (uint8_t*)(&i), 4);
            for (unsigned int j=7; j<t_NetDataBytes; ++j) {
                l_pkt[j] = 0;
            }
            std::memcpy(l_transBuf[k] + i*t_NetDataBytes, l_pkt.data(), t_NetDataBytes); 
        }
        l_testVerifyHost[k].sendBO();
    }
   
    for (auto i=0; i<AL_numInfs; ++i) { 
        l_testVerifyHost[i].runCU(l_myID[i], l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles, l_waitCycles);
    }

    for (auto i=0; i<AL_numInfs; ++i) {
        l_resBuf[i] = (uint8_t*)(l_testVerifyHost[i].getRes());
        l_transBuf[i] = (uint8_t*)(l_testVerifyHost[i].getTransBuf());
        l_verifySentDestBuf[i] = (uint8_t*)(l_testVerifyHost[i].getVerifySentDestBuf());
        l_verifyRecDestBuf[i] = (uint8_t*)(l_testVerifyHost[i].getVerifyRecDestBuf());
    }

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
    unsigned int l_numTxPkts[AL_numInfs];
    unsigned int l_numRxPkts[AL_numInfs];
    unsigned int l_numVerifyTxPkts[AL_numInfs];
    unsigned int l_numVerifyRxPkts[AL_numInfs];
    for (auto k=0; k<AL_numInfs; ++k) {
        l_numTxPkts[k] = (l_transBuf[k][3]<<24);
        l_numTxPkts[k]  += (l_transBuf[k][2]<<16);
        l_numTxPkts[k]  += (l_transBuf[k][1]<<8);
        l_numTxPkts[k]  += l_transBuf[k][0];

        l_numRxPkts[k] = (l_resBuf[k][3]<<24);
        l_numRxPkts[k]  += (l_resBuf[k][2]<<16);
        l_numRxPkts[k]  += (l_resBuf[k][1]<<8);
        l_numRxPkts[k]  += l_resBuf[k][0];
        
        l_numVerifyTxPkts[k] = (l_verifySentDestBuf[k][3]<<24);
        l_numVerifyTxPkts[k]  += (l_verifySentDestBuf[k][2]<<16);
        l_numVerifyTxPkts[k]  += (l_verifySentDestBuf[k][1]<<8);
        l_numVerifyTxPkts[k]  += l_verifySentDestBuf[k][0];

        l_numVerifyRxPkts[k] = (l_verifyRecDestBuf[k][3]<<24);
        l_numVerifyRxPkts[k]  += (l_verifyRecDestBuf[k][2]<<16);
        l_numVerifyRxPkts[k]  += (l_verifyRecDestBuf[k][1]<<8);
        l_numVerifyRxPkts[k]  += l_verifyRecDestBuf[k][0];

        std::cout << "INFO: kernel " << k << " num of transmitted pkts is: " << l_numTxPkts[k] << std::endl;
        std::cout << "INFO: kernel " << k << " num of received pkts is: " << l_numRxPkts[k] << std::endl;
        std::cout << "INFO: verify kernel " << k << " registered num of transmitted pkts is: " << l_numVerifyTxPkts[k] << std::endl;
        std::cout << "INFO: verify kernel " << k << " registered num of received pkts is: " << l_numVerifyRxPkts[k] << std::endl;
    }
    for (auto k=0; k<AL_numInfs; ++k) {
        if ((l_numTxPkts[k] != l_numPkts) || (l_numRxPkts[k] != l_numPkts)) {
            std::cout << "ERROR: pkts transmitted or pkts received != " <<l_numPkts << std::endl;
            return EXIT_FAILURE;
        }
    }
    int l_dataErr = 0;
    for (auto k=0; k<AL_numInfs; ++k) {
        for (unsigned int i=1; i<(l_numPkts+1); ++i) {
            unsigned int l_res = (l_resBuf[k][i*t_NetDataBytes+6]<<24);
            l_res += l_resBuf[k][i*t_NetDataBytes+5]<<16;
            l_res += l_resBuf[k][i*t_NetDataBytes+4]<<8;
            l_res += l_resBuf[k][i*t_NetDataBytes+3];
            if (l_res != i) {
                std::cout << "ERROR: Kernel " << k << " Pkt " << i << " received " << l_res << std::endl;
                l_dataErr++;
            }
        }
    }
    if (l_dataErr != 0) {
        std::cout << "ERROR: data error!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout <<"Test Pass!" << std::endl;
    return EXIT_SUCCESS;

}
