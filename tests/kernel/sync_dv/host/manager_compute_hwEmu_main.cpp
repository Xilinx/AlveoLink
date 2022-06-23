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
#include "xnik_dv.hpp"
#include "testAppHost.hpp"
#include "dvNetLayer.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc <6  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <numPkts> <waitCycles> <flushCounter> [mode]" << std::endl;
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
    unsigned int l_mode = 0; //0: ring, 1: congestion
    if (argc > 7) {
        l_mode =  atoi(argv[l_idx++]);
    }

    unsigned int l_numDevs = 3;
    unsigned int l_batchPkts = 128;
    unsigned int l_timeOutCycles = 0;
    unsigned int l_computeWaitSeconds = 2;


    std::cout << "INFO: batchPkts = " << l_batchPkts << std::endl;
    std::cout << "INFO: timeOutCycles = " <<  l_timeOutCycles << std::endl;
    std::cout << "INFO: computeWaitSeconds = " << l_computeWaitSeconds << std::endl;
    std::cout << "INFO: communication mode (0:ring, 1:congestion) = " << l_mode << std::endl;
 
    AlveoLink::common::FPGA l_card;
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
    testAppHost<AL_netDataBits> l_testAppHost[3];
    AlveoLink::kernel::xnikTX<3> l_xnikTX[3];
    AlveoLink::kernel::xnikRX<3> l_xnikRX[3];

    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;

    std::cout <<"INFO: total number of compute nodes: " << l_numDevs << std::endl;
    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter);
    l_manager.sendBO();
    l_manager.runCU();
    std::cout <<"INFO: manager has started... " << std::endl;

    for (auto i=0; i<3; ++i) {
        l_xnikTX[i].init(&l_card);
        l_xnikRX[i].init(&l_card);
        l_testAppHost[i].init(&l_card);
        
        l_xnikTX[i].createCU(i);
        l_xnikRX[i].createCU(i);
        l_testAppHost[i].createCU(i);
    }

    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = (l_mode == 0)? l_transBytes: l_transBytes*2;
    unsigned int l_statsBytes = 128 * sizeof(uint32_t);
    uint8_t* l_transBuf[3];
    uint8_t* l_resBuf[3];
    uint32_t* l_txStats[3];
    uint32_t* l_rxStats[3];

    for (auto i=0; i<3; ++i) {
        l_transBuf[i] = (uint8_t*)l_testAppHost[i].createTransBuf(l_transBytes);
        l_resBuf[i] = (uint8_t*)l_testAppHost[i].createRecBufs(l_recBytes);
        l_txStats[i] = (uint32_t*)l_xnikTX[i].createStatsBuf(l_statsBytes);
        l_rxStats[i] = (uint32_t*)l_xnikRX[i].createStatsBuf(l_statsBytes);
    
        std::memset(l_transBuf[i], 0, l_transBytes);
        std::memset(l_resBuf[i], 0, l_recBytes);
        std::memset(l_txStats[i], 0, l_statsBytes);
        std::memset(l_rxStats[i], 0, l_statsBytes);
        l_testAppHost[i].sendBO();
        l_xnikTX[i].sendBO();
        l_xnikRX[i].sendBO();
    }
    for (auto i=0; i<3; ++i) {//run all TX and RX
        l_xnikTX[i].runCU();
        l_xnikRX[i].runCU();
    }
    
    for (auto i=0; i<3; ++i) {//start application kernel
        unsigned int l_destId = (i + 1);
        if (l_destId == l_numDevs) {
            l_destId = (l_mode == 0)? 0: i - 1;
        } 
        l_testAppHost[i].runCU(i, l_destId, l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles);
    }

    for (auto i=0; i<3; ++i) {
        l_xnikTX[i].finish();
        l_xnikRX[i].finish();
        l_testAppHost[i].finish();
        l_xnikTX[i].syncBO();
        l_xnikRX[i].syncBO();
        l_testAppHost[i].syncRes();
        l_testAppHost[i].syncTrans();
    }

    l_manager.finish();
    std::cout << "INFO: system run finished!" << std::endl;

    std::cout << std::dec;

    int l_dataErr = 0;
    unsigned int l_numTxPkts[3];
    unsigned int l_numRxPkts[3];
    for (auto i=0; i<3; ++i) {
        l_numTxPkts[i] = (l_transBuf[i][3]<<24);
        l_numTxPkts[i]  += (l_transBuf[i][2]<<16);
        l_numTxPkts[i]  += (l_transBuf[i][1]<<8);
        l_numTxPkts[i]  += l_transBuf[i][0];

        l_numRxPkts[i] = (l_resBuf[i][3]<<24);
        l_numRxPkts[i]  += (l_resBuf[i][2]<<16);
        l_numRxPkts[i]  += (l_resBuf[i][1]<<8);
        l_numRxPkts[i]  += l_resBuf[i][0];
        std::cout << "INFO: kernel " << i << " num of transmitted pkts is: " << l_numTxPkts[i] << std::endl;
        std::cout << "INFO: kernel " << i << " num of received pkts is: " << l_numRxPkts[i] << std::endl;
        
        if (l_numTxPkts[i] != l_numPkts) {
            l_dataErr++;
            std::cout << "ERROR: kernel " << i << " pkts transmitted != " <<l_numPkts << std::endl;
        }
    }
        
    if (l_dataErr != 0) {
        return EXIT_FAILURE;
    }


    unsigned int l_numRecPkts[3];
    for (auto k=0; k<3; ++k) {
        if (l_mode == 0) {
            l_numRecPkts[k] = l_numPkts+1;
        }
        else {
            if ((k == (l_numDevs -2)) && (l_numDevs > 2)) {
                l_numRecPkts[k] = 2*l_numPkts+1;
            }
            else if ((k == 0) && (l_numDevs > 2)) {
                l_numRecPkts[k] = 0;
            }
            else {
                l_numRecPkts[k] = l_numPkts+1;
            }
        }
        std::vector<unsigned int> l_expArr(l_numPkts+1, 0);

        for (unsigned int i=1; i<(l_numRecPkts[k]); ++i) {
            unsigned int l_res = (l_resBuf[k][i*t_NetDataBytes+7]<<24);
            l_res += l_resBuf[k][i*t_NetDataBytes+6]<<16;
            l_res += l_resBuf[k][i*t_NetDataBytes+5]<<8;
            l_res += l_resBuf[k][i*t_NetDataBytes+4];
            if ((l_res > l_numPkts) || (l_res < 1)) {
                std::cout << "ERROR: Kernel " << k << " Pkt " << i << " received " << l_res << std::endl;
                l_dataErr++;
            }
            else {
                l_expArr[l_res] = l_expArr[l_res] + 1;
            }
        }

        for (auto i=1; i<l_numPkts+1; ++i) {
            if ((l_numRecPkts[k] > (l_numPkts+1)) && (l_expArr[i] != 2)) {
                std::cout << "ERROR: Kernel " << k << " received value " << i << " " << l_expArr[i] << " times" << std::endl;
                l_dataErr++; 
            }
            else if ((l_numRecPkts[k] == (l_numPkts+1)) && (l_expArr[i] != 1)) {
                std::cout << "ERROR: Kernel " << k << " received value " << i << " " << l_expArr[i] << " times" << std::endl;
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
