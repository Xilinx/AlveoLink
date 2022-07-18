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
//#include "xnik_dv.hpp"
#include "cntPktsHost.hpp"
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
    if (argc > 6) {
        l_mode =  atoi(argv[l_idx++]);
    }

    unsigned int l_numDevs = 3;
    unsigned int l_batchPkts = 128;
    unsigned int l_timeOutCycles = 0;


    std::cout << "INFO: batchPkts = " << l_batchPkts << std::endl;
    std::cout << "INFO: timeOutCycles = " <<  l_timeOutCycles << std::endl;
    std::cout << "INFO: communication mode (0:ring, 1:congestion) = " << l_mode << std::endl;
 
    AlveoLink::common::FPGA l_card;
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
    testAppHost<AL_netDataBits> l_testAppHost[3];
    cntPktsHost<2> l_cntPktsHost;

    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;

    std::cout <<"INFO: total number of compute nodes: " << l_numDevs << std::endl;
    unsigned int l_numNetPkts[2], l_numRxUsrPkts;
    l_cntPktsHost.init(&l_card);
    l_cntPktsHost.createCU(0, 0);

    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter);
    l_manager.sendBO();

    for (auto i=0; i<3; ++i) {
        l_testAppHost[i].init(&l_card);
        l_testAppHost[i].createCU(i);
    }

    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = (l_mode == 0)?(l_numPkts*4+1)*t_NetDataBytes: (l_numPkts*8+1)*t_NetDataBytes;
    unsigned int l_numData = l_numPkts * 16;
    unsigned int l_totalNumInts[3];
    unsigned int l_totalSum[3];
    unsigned int l_destId[3];
    uint32_t* l_transBuf[3];
    uint32_t* l_resBuf[3];
    
    for (auto i=0; i<3; ++i) {
        l_totalSum[i] = 0;
    }

    for (auto i=0; i<3; ++i) {
        l_transBuf[i] = (uint32_t*)l_testAppHost[i].createTransBuf(l_transBytes);
        l_resBuf[i] = (uint32_t*)l_testAppHost[i].createRecBufs(l_recBytes);
        if (l_mode == 0) {
            l_totalNumInts[i] = (l_numPkts * 15);
        }
        else {
            l_totalNumInts[i] = (i==1)? (l_numPkts * 15*2): (i==2)?(l_numPkts * 15): 0;
        }
    
        std::memset(l_resBuf[i], 0, l_recBytes);
        std::memset(l_transBuf[i], 0, l_transBytes);
        l_destId[i] = (i + 1);
        if (l_destId[i] == l_numDevs) {
            l_destId[i] = (l_mode == 0)? 0: i - 1;
        } 
        for (auto j=0; j<l_numData; ++j) {
            if (j % 16 == 0) {
                l_transBuf[i][j+16] = (1<<31) + l_destId[i];
            }
            else {
                l_transBuf[i][j+16] = j;
                if ((l_mode == 0) || (i==2))  {
                    l_totalSum[i] += j;
                }
                else if (l_mode == 1) {
                    if (i==0) {
                        l_totalSum[i] += 0;
                    }
                    else if (i==1) {
                        l_totalSum[i] += j*2;
                    }
                }
            }
        }
    }
    
    for (auto i=0; i<3; ++i) {//start application kernel
        l_testAppHost[i].sendBO();
    }

    for (auto runId = 0; runId < 2; ++ runId) {
        l_manager.runCU();
        l_cntPktsHost.runCU(l_numNetPkts, l_numRxUsrPkts);
        std::cout <<"INFO: manager has started... " << std::endl;


        for (auto i=0; i<3; ++i) {//start application kernel
            l_testAppHost[i].runCU(i, l_destId[i], l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles, l_totalNumInts[i]);
        }

        for (auto i=0; i<3; ++i) {
            l_testAppHost[i].finish();
            l_testAppHost[i].syncRes();
            l_testAppHost[i].syncTrans();
        }
        
        l_manager.finish();
        l_cntPktsHost.finish();
    }
    std::cout << "INFO: system run finished!" << std::endl;

    l_cntPktsHost.getNumTxNetPkts(0, l_numNetPkts[0]);
    l_cntPktsHost.getNumRxNetPkts(0, l_numNetPkts[1]);
    l_cntPktsHost.getNumRxUsrPkts(l_numRxUsrPkts);
    std::cout <<"INFO: testApp0+XNIK has transmitted " << l_numNetPkts[0] << " net pkts." << std::endl;
    std::cout <<"INFO: testApp0+XNIK has received " << l_numNetPkts[1] << " net pkts." << std::endl;
    std::cout <<"INFO: testApp0 has received " << l_numRxUsrPkts << " usr pkts from XNIK." << std::endl;

    std::cout << std::dec;

    int l_errs = 0;
    unsigned int l_totalSumRec[3];
    for (auto i=0; i<3; ++i) {
        l_totalSumRec[i] = 0;
    }

    std::cout << std::dec << std::endl;
    

    if (l_errs != 0) {
        return EXIT_FAILURE;
    }
    std::cout <<"Test Pass!" << std::endl;
    for (auto i=0; i<3; ++i) {
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
    for (auto i=0; i<3; ++i) {
        if (l_totalSumRec[i] != l_totalSum[i]) {
            std::cout << "ERROR: kernel " << i << " has data errors!" << std::endl;
            std::cout << "    INFO: total sum of data sent " << l_totalSum[i]<< " != " << l_totalSumRec[i] << " the sum of receivedintegers"  << std::endl;
            l_errs++;
        }
    }

    return EXIT_SUCCESS;
}
