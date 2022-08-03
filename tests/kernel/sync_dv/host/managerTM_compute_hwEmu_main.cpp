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
#include <unistd.h>

#include "hwManagerHost.hpp"
#include "trafficManagerHost.hpp"
#include "testAppHost.hpp"
#include "dvNetLayer.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc <6  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <numPkts> <waitCycles> <flushCounter>" << std::endl;
        std::cout << "manager_compute.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_numPkts = atoi(argv[l_idx++]);
    unsigned int l_waitCycles = atoi(argv[l_idx++]);
    unsigned int l_flushCounter = atoi(argv[l_idx++]);

    unsigned int l_numDevs = 3;

 
    AlveoLink::common::FPGA l_card;
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
    AlveoLink::kernel::trafficManagerHost<AL_netDataBits> l_trafficManager;
    testAppHost<AL_netDataBits> l_testAppHost[2];

    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;

    std::cout <<"INFO: total number of compute nodes: " << l_numDevs-1 << std::endl;

    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter);
    l_manager.sendBO();

    l_trafficManager.init(&l_card);
    l_trafficManager.createCU(0);
    l_trafficManager.setConfigBuf(2, 2, l_numDevs, l_waitCycles, (uint32_t)(((1<<20)/64)-1));
    uint8_t* l_pktStore = (uint8_t*)(l_trafficManager.createPktStore(1<<20));
    l_trafficManager.sendBO();

    for (auto i=0; i<2; ++i) {
        l_testAppHost[i].init(&l_card);
        l_testAppHost[i].createCU(i);
    }

    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = (l_numPkts*4+1)*t_NetDataBytes;
    unsigned int l_numData = l_numPkts * 16;
    unsigned int l_totalNumInts[2];
    unsigned int l_totalSum[2];
    unsigned int l_destId[2];
    uint32_t* l_transBuf[2];
    uint32_t* l_resBuf[2];
    
    for (auto i=0; i<2; ++i) {
        l_totalSum[i] = 0;
    }

    for (auto i=0; i<2; ++i) {
        l_transBuf[i] = (uint32_t*)l_testAppHost[i].createTransBuf(l_transBytes);
        l_resBuf[i] = (uint32_t*)l_testAppHost[i].createRecBufs(l_recBytes);
        l_totalNumInts[i] = (l_numPkts * 15);
    
        std::memset(l_resBuf[i], 0, l_recBytes);
        std::memset(l_transBuf[i], 0, l_transBytes);
        l_destId[i] = (i + 1);
        if (l_destId[i] == (l_numDevs-1)) {
            l_destId[i] = 0;
        } 
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

    l_manager.runCU();
    l_trafficManager.runCU();
    std::cout <<"INFO: managers has started... " << std::endl;


    for (auto i=0; i<2; ++i) {//start application kernel
        l_testAppHost[i].runCU(i, l_destId[i], l_numDevs, l_numPkts, 2, l_totalNumInts[i]);
    }

    for (auto i=0; i<2; ++i) {
        l_testAppHost[i].finish();
        l_testAppHost[i].syncRes();
        l_testAppHost[i].syncTrans();
    }
    
    l_manager.finish();
    l_trafficManager.finish();
    std::cout << "INFO: system run finished!" << std::endl;

    std::cout << std::dec;

    int l_errs = 0;
    unsigned int l_totalSumRec[3];
    for (auto i=0; i<3; ++i) {
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
