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
#include "testAppHost.hpp"
#include "dvNetLayer.hpp"
#include "graphPktDefs.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc <7  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <numDevs> <numPkts> <waitCycles> <flushCounter> [debug] [batchPkts] [timeOutCycles] [waitSeconds] [mode]" << std::endl;
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
    if (argc > 7) {
        l_debug =  argv[l_idx++];
    }

    unsigned int l_batchPkts = 128;
    unsigned int l_timeOutCycles = 0;
    unsigned int l_computeWaitSeconds = 2;
    unsigned int l_mode = 0; //0: ring, 1: congestion

    if (argc > 8) {
        l_batchPkts = atoi(argv[l_idx++]);
    }
    if (argc > 9) {
        l_timeOutCycles = atoi(argv[l_idx++]);
    }
    if (argc > 10) {
        l_computeWaitSeconds = atoi(argv[l_idx++]);
    }
    if (argc > 11) {
        l_mode = atoi(argv[l_idx++]);
    }

    std::cout << "INFO: batchPkts = " << l_batchPkts << std::endl;
    std::cout << "INFO: timeOutCycles = " <<  l_timeOutCycles << std::endl;
    std::cout << "INFO: computeWaitSeconds = " << l_computeWaitSeconds << std::endl;
    std::cout << "INFO: communication mode (0:ring, 1:congestion) = " << l_mode << std::endl;
 
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
    l_dvNetLayer.clearCounters();

    std::cout <<"INFO: total number of compute nodes: " << l_numDevs << std::endl;
    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter);
    l_manager.sendBO();
    l_manager.runCU();
    std::cout <<"INFO: manager has started... " << std::endl;

    l_testAppHost.init(&l_card);
    l_testAppHost.createCU(0);

    unsigned int l_transBytes = t_NetDataBytes * (l_numPkts+1);
    unsigned int l_recBytes = (l_mode == 0)? l_transBytes: l_transBytes*2;
    uint8_t* l_transBuf;
    uint8_t* l_resBuf;
    l_transBuf = (uint8_t*)l_testAppHost.createTransBuf(l_transBytes);
    l_resBuf = (uint8_t*)l_testAppHost.createRecBufs(l_recBytes);
    
    std::memset(l_transBuf, 0, l_transBytes);
    std::memset(l_resBuf, 0, l_recBytes);    
    l_testAppHost.sendBO();

    unsigned int l_destId = (l_ids[0] + 1);
    if (l_destId == l_numDevs) {
        l_destId = (l_mode == 0)? 0: l_ids[0] - 1;
    } 
    l_testAppHost.runCU(l_ids[0], l_destId, l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles);

     
    if (l_debug == "debug") {
        sleep(l_computeWaitSeconds);
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
   
    if (l_debug != "debug") {
        l_testAppHost.finish();
    } 
    l_testAppHost.syncRes();
    l_testAppHost.syncTrans();

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
        
    unsigned int l_wt0, l_wt1, l_wt2;
    unsigned int l_wr0, l_wr1, l_wr2;
    if (l_numTxPkts == 0) {
        for (auto i=1; i<l_numPkts+1; ++i) {
            l_wt0 = (l_transBuf[i*t_NetDataBytes+3]<<24);
            l_wt0 += l_transBuf[i*t_NetDataBytes+2]<<16;
            l_wt0 += l_transBuf[i*t_NetDataBytes+1]<<8;
            l_wt0 += l_transBuf[i*t_NetDataBytes];
            
            l_wt1 = (l_transBuf[i*t_NetDataBytes+7]<<24);
            l_wt1 += l_transBuf[i*t_NetDataBytes+6]<<16;
            l_wt1 += l_transBuf[i*t_NetDataBytes+5]<<8;
            l_wt1 += l_transBuf[i*t_NetDataBytes+4];
            
            l_wt2 = (l_transBuf[i*t_NetDataBytes+11]<<24);
            l_wt2 += l_transBuf[i*t_NetDataBytes+10]<<16;
            l_wt2 += l_transBuf[i*t_NetDataBytes+9]<<8;
            l_wt2 += l_transBuf[i*t_NetDataBytes+8];

            if ((l_wt0 == 0) && (l_wt1 == 0) && (l_wt2 == 0)) {
                l_numTxPkts = i-1;
                break;
            }
        }
    }

    if (l_numRxPkts == 0) {
        for (auto i=1; i<l_numPkts+1; ++i) {
            l_wr0 = (l_resBuf[i*t_NetDataBytes+3]<<24);
            l_wr0 += l_resBuf[i*t_NetDataBytes+2]<<16;
            l_wr0 += l_resBuf[i*t_NetDataBytes+1]<<8;
            l_wr0 += l_resBuf[i*t_NetDataBytes];
            
            l_wr1 = (l_resBuf[i*t_NetDataBytes+7]<<24);
            l_wr1 += l_resBuf[i*t_NetDataBytes+6]<<16;
            l_wr1 += l_resBuf[i*t_NetDataBytes+5]<<8;
            l_wr1 += l_resBuf[i*t_NetDataBytes+4];
            
            l_wr2 = (l_resBuf[i*t_NetDataBytes+11]<<24);
            l_wr2 += l_resBuf[i*t_NetDataBytes+10]<<16;
            l_wr2 += l_resBuf[i*t_NetDataBytes+9]<<8;
            l_wr2 += l_resBuf[i*t_NetDataBytes+8];

            if ((l_wr0 == 0) && (l_wr1 == 0) && (l_wr2 == 0)) {
                l_numRxPkts = i-1;
                break;
            }
        }
    }

    std::cout << "INFO: kernel " << l_ids[0] << " num of transmitted pkts is: " << l_numTxPkts << std::endl;
    std::cout << "INFO: kernel " << l_ids[0] << " num of received pkts is: " << l_numRxPkts << std::endl;

    int l_dataErr = 0;
    if (l_numTxPkts != l_numPkts) {
        std::cout << "ERROR: pkts transmitted != " <<l_numPkts << std::endl;
        l_dataErr++;
    }
    else if (l_debug == "debug")  {
        l_testAppHost.finish();
    }
    if (l_dataErr != 0) {
        return EXIT_FAILURE;
    }

    l_manager.finish();
    std::cout << "INFO: system run finished!" << std::endl;

    unsigned int l_numRecPkts = 0;
    if ((l_ids[0] == (l_numDevs -2)) && (l_numDevs > 2)) {
        l_numRecPkts = 2*l_numPkts+1;
    }
    else if ((l_ids[0] == 0) && (l_numDevs > 2)) {
        l_numRecPkts = 0;
    }
    else {
        l_numRecPkts = l_numPkts + 1;
    }

    std::vector<unsigned int> l_expArr(l_numPkts+1, 0);    
    for (unsigned int i=1; i<(l_numRecPkts); ++i) {
        unsigned int l_res = (l_resBuf[i*t_NetDataBytes+7]<<24);
        l_res += l_resBuf[i*t_NetDataBytes+6]<<16;
        l_res += l_resBuf[i*t_NetDataBytes+5]<<8;
        l_res += l_resBuf[i*t_NetDataBytes+4];
        if ((l_res > l_numPkts) || (l_res < 1)) {
            std::cout << "ERROR: Kernel " << l_ids[0] << " Pkt " << i << " received " << l_res << std::endl;
            l_dataErr++;
        }
        else {
            l_expArr[l_res] = l_expArr[l_res] + 1;
        }
    }
    for (auto i=1; i<l_numPkts+1; ++i) {
        if ((l_numRecPkts > (l_numPkts+1)) && (l_expArr[i] != 2)) {
            std::cout << "ERROR: Kernel " << l_ids[0]  << " received value " << i << " " << l_expArr[i] << " times" << std::endl;
            l_dataErr++;
        }
        else if ((l_numRecPkts == (l_numPkts+1)) && (l_expArr[i] != 1)) {
            std::cout << "ERROR: Kernel " << l_ids[0]  << " received value " << i << " " << l_expArr[i] << " times" << std::endl;
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
