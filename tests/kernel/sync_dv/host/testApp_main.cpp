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
#include <unistd.h>

#include "dvNetLayer.hpp"
#include "testAppHost.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc < 5  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbinfile> <devId> <hostId> <netfile> <numPkts> [debug]" << std::endl;
        std::cout << "test.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_hostId = atoi(argv[l_idx++]); 
    std::string l_netfilename = argv[l_idx++];
    unsigned int l_numPkts = atoi(argv[l_idx++]);
    std::string l_debug = "";
    if (argc >6 ) {
        l_debug = argv[l_idx++];
    }
   
    unsigned int l_syncManagerId = 0;
    unsigned int l_tmId = 0;
    unsigned int l_kernelStartId = 0;

    unsigned int l_numHosts = 0;
    unsigned int l_portIds[2];
    std::fstream l_netfile;
    l_netfile.open(l_netfilename);
    l_netfile >> l_tmId >> l_syncManagerId >> l_kernelStartId >> l_numHosts;
    for (unsigned int i=0; i<l_numHosts; ++i) {
        unsigned int ll_hostId, l_numCards;
        l_netfile >> ll_hostId >> l_numCards;
        for (unsigned int j=0; j< l_numCards; ++j) {
            unsigned int l_cardId, l_port0Id, l_port1Id;
            l_netfile >> l_cardId >> l_port0Id >> l_port1Id;
            if ((l_cardId == l_devId) && (ll_hostId == l_hostId)){
                l_portIds[0] = l_port0Id;
                l_portIds[1] = l_port1Id;
            }
        }
    }
    l_netfile.close();

    std::cout <<"INFO: syncManagerId is " << l_syncManagerId << std::endl;
    std::cout <<"INFO: tmId is " << l_tmId << std::endl;
    std::cout <<"INFO: kernelStartId is " << l_kernelStartId << std::endl;

    uint32_t l_myID[AL_numInfs];
    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    testAppHost<AL_netDataBits> l_testAppHost[AL_numInfs];
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
    l_dvNetLayer.clearCounters();

    for (auto i=0; i<AL_numInfs; ++i) {
        l_testAppHost[i].init(&l_card);
        l_testAppHost[i].createCU(i);
    }
    std::cout << "INFO: successfully created CUs" << std::endl;

    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = (l_numPkts*4+1)*t_NetDataBytes;
    assert(l_recBytes > 0);
    unsigned int l_numData = l_numPkts * 16;
    uint32_t* l_transBuf[AL_numInfs];
    uint32_t* l_resBuf[AL_numInfs];
    unsigned int l_totalNumInts[2];
    unsigned int l_totalSum[2];
    unsigned int l_destId[2];

    for (auto i=0; i<AL_numInfs; ++i) {
        l_transBuf[i] = (uint32_t*)l_testAppHost[i].createTransBuf(l_transBytes);
        l_resBuf[i] = (uint32_t*)l_testAppHost[i].createRecBufs(l_recBytes);
    }

    for (auto i=0; i<2; ++i) {
        l_totalSum[i] = 0;
    }

    for (auto i=0; i<2; ++i) {
        l_totalNumInts[i] = (l_numPkts * 15);
    
        std::memset(l_resBuf[i], 0, l_recBytes);
        std::memset(l_transBuf[i], 0, l_transBytes);
        l_destId[i] = l_myID[1-i];
        for (auto j=0; j<l_numData; ++j) {
            if (j % 16 == 0) {
                l_transBuf[i][j+16] = (1<<31) + l_destId[i];
            }
            else {
                l_transBuf[i][j+16] = i+j;
                l_totalSum[i] = l_totalSum[i]+i+j;
            }
        }
    }
    
    for (auto i=0; i<2; ++i) {//start application kernel
        l_testAppHost[i].sendBO();
    }
    for (auto i=0; i<2; ++i) {//start application kernel
        l_testAppHost[i].runCU(l_myID[i], l_destId[i], l_syncManagerId, l_numPkts, l_tmId, l_totalNumInts[i]);
        std::cout << "INFO: CU " << i << " started." << std::endl;
    }

    for (auto i=0; i<2; ++i) {
        l_testAppHost[i].finish();
        l_testAppHost[i].syncRes();
        l_testAppHost[i].syncTrans();
        std::cout << "INFO: CU " << i << " finished." << std::endl;
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

    int l_errs = 0;
    unsigned int l_totalSumRec[2];
    for (auto i=0; i<2; ++i) {
        l_totalSumRec[i] = 0;
    }

    std::cout << std::dec << std::endl;
    

    for (auto i=0; i<2; ++i) {
        unsigned int l_recWideWords = l_resBuf[i][0];
        unsigned int l_totalRecInts = l_recWideWords * 16;
        std::cout << "INFO: kernel " << i << " received " << l_recWideWords << " 512-bit data" << std::endl;
        for (auto j=0; j<l_totalRecInts; ++j) {
            uint32_t l_val = l_resBuf[i][j+16];
            if (((l_val >> 31) & 0x00000001) != 1) {
                l_totalSumRec[i] += l_val;
            }
        }
    }
    for (auto i=0; i<2; ++i) {
        if (l_totalSumRec[i] != l_totalSum[1-i]) {
            std::cout << "ERROR: kernel " << i << " has data errors!" << std::endl;
            std::cout << "    INFO: total sum of data sent " << l_totalSum[1-i]<< " != " << l_totalSumRec[i] << " the sum of receivedintegers"  << std::endl;
            l_errs++;
        }
    }

    if (l_errs != 0) {
        return EXIT_FAILURE;
    }

    std::cout <<"Test Pass!" << std::endl;
    return EXIT_SUCCESS;

}
