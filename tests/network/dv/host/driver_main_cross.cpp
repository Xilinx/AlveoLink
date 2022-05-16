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
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <ctime>
#include <algorithm>    // std::sort


#include "dvNetLayer.hpp"
#include "basicHost.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_ClockPeriod = 5; //clock period in ns
constexpr unsigned int t_VitisOverhead = 8; //clock cycles caused by xnik_dv converter

int main(int argc, char** argv) {
    if (argc != 4  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <numWidePkts>" << std::endl;
        std::cout << "host.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_numWidePkts = atoi(argv[l_idx++]);
    if (l_numWidePkts <= 1) {
        std::cout << "ERROR: number of pkts must be > 1!" << std::endl;
        return EXIT_FAILURE;
    }

    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    basicHost<AL_netDataBits, AL_destBits, AL_maxConnections> l_basicHost[AL_numInfs];
    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;

    l_dvNetLayer.init(&l_card);
    std::bitset<AL_numInfs> l_linkUp = l_dvNetLayer.getLinkStatus();
    if (!l_linkUp.all()) {
        std::cout << "ERROR: QSFP link is not ready!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "INFO: all links are up." << std::endl;
    uint16_t l_numDests = l_dvNetLayer.getNumDests();
    uint16_t* l_ids = l_dvNetLayer.getIds();
    for (auto i=0; i<AL_numInfs; ++i) {
        std::cout << "INFO: port " << i << " has id " << l_ids[i] << std::endl;
    }

    l_dvNetLayer.clearCounters(); // clear all counters

    uint32_t* l_dataSendBuf[AL_numInfs];
    uint32_t* l_dataRecBuf[AL_numInfs];
    uint16_t* l_destSendBuf[AL_numInfs];//keep is used for dest
    uint32_t* l_statsBuf[AL_numInfs];
    unsigned int l_numData = l_numWidePkts * 16;
    unsigned int l_numDest = l_numWidePkts;
    for (auto i=0; i<AL_numInfs; ++i) {
        l_basicHost[i].init(&l_card);
        l_basicHost[i].createCU(i);
        l_basicHost[i].createTxBufs(l_numWidePkts);
        l_basicHost[i].createRxBufs(l_numWidePkts);
        l_dataSendBuf[i] = (uint32_t*)(l_basicHost[i].getTxDataPtr());
        l_destSendBuf[i] = (uint16_t*)(l_basicHost[i].getTxDestPtr());
        for (auto j=0; j<l_numData; ++j) {
            l_dataSendBuf[i][j] = j;
        } 
        for (auto j=0; j<l_numDest; ++j) {
            l_destSendBuf[i][j] = (j % 2 + AL_numInfs) % l_numDests;
        }
    }
    for (auto i=0; i<AL_numInfs; ++i) {
        l_basicHost[i].sendBO();
        l_basicHost[i].runCU(l_numWidePkts);
    }
    for (auto i=0; i<AL_numInfs; ++i) {
        l_dataRecBuf[i] = (uint32_t*)l_basicHost[i].getRecData();
        l_statsBuf[i] = (uint32_t*)l_basicHost[i].getRecStats();
    }
    std::vector<uint64_t> l_pktRxCnts = l_dvNetLayer.getLaneRxPktsCnt();
    std::vector<uint64_t> l_pktTxCnts = l_dvNetLayer.getLaneTxPktsCnt();
    //check results
    int l_dataErrs[AL_numInfs];
    int l_errs = 0;
    std::cout << std::endl;
    for (auto i=0; i<AL_numInfs; ++i) {
        l_dataErrs[i] = 0;
        std::cout << "INFO: port " << i << " has sent " << l_pktTxCnts[i] << " pkts." << std::endl;
        std::cout << "INFO: port " << i << " has received " << l_pktRxCnts[i] << " pkts." << std::endl;
    }
    std::cout << std::endl;
    for (auto i=0; i<AL_numInfs; ++i) {
        std::sort(l_dataRecBuf[i], l_dataRecBuf[i]+l_numData);
        for (auto j=0; j<l_numData; ++j) {
            if (l_dataSendBuf[i][j] != l_dataRecBuf[i][j]) {
                l_dataErrs[i]++;
                l_errs++;
            }
        }
    }    
    for (auto i=0; i<AL_numInfs; ++i) {
        if (l_dataErrs[i] != 0) {
            std::cout << "ERROR: port " << i << " has " << std::dec << l_dataErrs[i] << " data errors!" << std::endl;
            for (auto j=0; j<4; ++j) {
                std::vector<uint32_t> l_lastRxPkt = l_dvNetLayer.getLastRxPkt(i, j);
                std::cout << "    INFO: last rx pkt for port " << i << " lane " << j << " is: " << std::endl;
                for (auto k=0; k<4; ++k) {
                    std::cout << "    w" << k << " = 0x" << std::hex << l_lastRxPkt[k];
                }
                std::cout << std::endl;
            }
            for (auto j=0; j<4; ++j) {
                std::vector<uint32_t> l_lastTxPkt = l_dvNetLayer.getLastTxPkt(i, j);
                std::cout << "    INFO: last tx pkt for port " << i << " lane " << j << " is: " << std::endl;
                for (auto k=0; k<4; ++k) {
                    std::cout << "    w" << k << " = 0x" << std::hex << l_lastTxPkt[k];
                }
                std::cout << std::endl;
            }
            std::cout << "    INFO: last uint32_t in dataSendBuf[" << i << "] = " << l_dataSendBuf[i][l_numData-1]  << std::endl;
            std::cout << "    INFO: last uint32_t in dataRecBuf[" << i << "] = " << l_dataRecBuf[i][l_numData-1]  << std::endl;
        }
    }
    if (l_errs != 0) {
        std::cout << "ERROR: total " << std::dec <<l_errs << " mismatches!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "INFO: Test Pass!" << std::endl;

    //dump performance data
    //check if performance file exists
    std::string l_perfFileName = "./perf.txt";
    struct stat l_buffer;
    std::ofstream l_outFile;
    if (stat (l_perfFileName.c_str(), &l_buffer) == 0) {//file exists
        l_outFile.open(l_perfFileName, std::ios_base::app); 
    }
    else { //file doesn't exit
        l_outFile.open(l_perfFileName);
        l_outFile << "port_no, " << "num_64B_pkts, " << "num_bytes, ";
        l_outFile << "lat_cycles, " << "total_cycles, " << "lat_time(ns), " << "total_time(ns), ";
        l_outFile << "bandwidth(GB/Sec)" << std::endl;
    }
    for (auto i=0; i<AL_numInfs; ++i) {
        l_statsBuf[i][0] = l_statsBuf[i][0] - 2; //latency counter added 2 extra cycles;
        l_statsBuf[i][1] = l_statsBuf[i][1] + 2; //total cycle counter is 2 cycles less than the real value
        unsigned int l_latTime = l_statsBuf[i][0] * t_ClockPeriod;
        unsigned int l_totalTime =  l_statsBuf[i][1] * t_ClockPeriod;
        double l_bandwidth = (l_numWidePkts * 64.0) / l_totalTime;
        l_outFile << i << ", " << std::dec << l_numWidePkts <<", " << l_numWidePkts * 64 << ", ";
        l_outFile << l_statsBuf[i][0] << ", " << l_statsBuf[i][1] << ", "; 
        l_outFile << l_latTime << ", " << l_totalTime << ", ";
        l_outFile << std::fixed << l_bandwidth << std::endl;
    }
    l_outFile.close();
    return EXIT_SUCCESS;
}
