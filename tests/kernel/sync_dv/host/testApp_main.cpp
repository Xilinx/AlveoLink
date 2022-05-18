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
#include "testAppHost.hpp"
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
    if (argc == 5) {
        l_debug = argv[l_idx++];
    }
    unsigned int l_batchPkts = 128; //atoi(argv[l_idx++]) - 1;
    unsigned int l_timeOutCycles = 0; //atoi(argv[l_idx++]) - 1;
    unsigned int l_waitCycles = 200; //atoi(argv[l_idx++]);
    

    uint32_t l_myID[AL_numInfs];
    int l_numDevs = AL_numInfs; 
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

    for (auto i=0; i<AL_numInfs; ++i) {
        l_testAppHost[i].init(&l_card);
        l_testAppHost[i].createCU(i);
    }
    unsigned int l_transBytes = t_NetDataBytes * l_numPkts;
    unsigned int l_recBytes = l_transBytes;
    assert(l_recBytes > 0);
    uint8_t* l_transBuf[AL_numInfs];
    uint8_t* l_resBuf[AL_numInfs];

    for (auto i=0; i<AL_numInfs; ++i) {
        l_transBuf[i] = (uint8_t*)l_testAppHost[i].createTransBuf(l_transBytes);
        l_testAppHost[i].createRecBufs(l_recBytes);
    }

    for (auto k=0; k<AL_numInfs; ++k) {
        for (unsigned int i=0; i<l_numPkts; ++i) {
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
        l_testAppHost[k].sendBO();
    }
   
    for (auto i=0; i<AL_numInfs; ++i) { 
        l_testAppHost[i].runCU(l_myID[i], l_numDevs, l_numPkts, l_batchPkts, l_timeOutCycles, l_waitCycles);
    }
    if (l_debug != "debug") {
        for (auto i=0; i<AL_numInfs; ++i) {
            l_resBuf[i] = (uint8_t*)(l_testAppHost[i].getRes());
        }

        int l_dataErr = 0;
        for (auto k=0; k<AL_numInfs; ++k) {
            for (unsigned int i=0; i<l_numPkts; ++i) {
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

    std::cout << "Please choose one of the following options: " << std::endl;
    std::cout << "0: quit" << std::endl;
    std::cout << "1: wait until kernel finishes" << std::endl;
    std::cout << "2: clearCounters" << std::endl;
    std::cout << "3: getClrCnts" << std::endl;
    std::cout << "4: getIdStatus" << std::endl;
    std::cout << "5: getLaneRxCounter" << std::endl;
    std::cout << "6: getLaneTxCounter" << std::endl;
    std::cout << "7: getLastRxPkt" << std::endl;
    std::cout << "8: getLastTxPkt" << std::endl;
    int l_option;
    std::cin >> l_option;
    while (l_option != 0) {
         if (l_option == 1) {
            for (auto i=0; i<AL_numInfs; ++i) {
                l_resBuf[i] = (uint8_t*)(l_testAppHost[i].getRes());
            }

            int l_dataErr = 0;
            for (auto k=0; k<AL_numInfs; ++k) {
                for (unsigned int i=0; i<l_numPkts; ++i) {
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
        if (l_option == 2) {
            l_dvNetLayer.clearCounters();
            std::cout << "INFO: reset counters completed." << std::endl;
        }
        if (l_option == 3) {
            std::vector<unsigned int> l_clrCnts = l_dvNetLayer.getClrCnts();
            for (auto i=0; i<AL_numInfs; ++i) {
                std::cout << "INFO: port " << i << " ClrCnts value is: 0x" << std::hex << l_clrCnts[i] << std::endl;
            }
        }
        if (l_option == 4) {
            uint16_t* l_ids = l_dvNetLayer.getIds();
            std::bitset<AL_numInfs> l_linkUp = l_dvNetLayer.getLinkStatus();
            for (auto i=0; i<AL_numInfs; ++i) {
                std::cout << "INFO: port " << i << " has id " << std::dec << l_ids[i]  << std::endl;
                if (l_linkUp[i]) {
                    std::cout << "INFO: port " << i << " link up." << std::endl;
                }
                else {
                    std::cout << "INFO: port " << i << " link down." << std::endl;
                }
            }
        }
        if (l_option == 5) {
            std::vector<uint64_t> l_pktCnts = l_dvNetLayer.getLaneRxPktsCnt();
            for (auto i=0; i<AL_numInfs; ++i) {
                std::cout << "INFO: port " << i << std::endl;
                for (auto j=0; j<4; ++j) {
                    std::cout << "    lane " << j << " has received " << l_pktCnts[i*4+j] << " packets." << std::endl;
                }
            }
        }
        if (l_option == 6) {
            std::vector<uint64_t> l_pktCnts = l_dvNetLayer.getLaneTxPktsCnt();
            for (auto i=0; i<AL_numInfs; ++i) {
                std::cout << "INFO: port " << i << std::endl;
                for (auto j=0; j<4; ++j) {
                    std::cout << "    lane " << j << " has transmitted " << l_pktCnts[i*4+j] << " packets." << std::endl;
                }
            }
        }
        if (l_option == 7) {
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
        if (l_option == 8) {
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
        }
        std::cout << std::endl;
        std::cout << "INFO: Please enter \'c\' to continue..." << std::endl;
        char l_unused;
        std::cin >> l_unused;
        std::cout << "Please choose one of the following options: " << std::endl;
        std::cout << "0: quit" << std::endl;
        std::cout << "1: wait until kernel finishes" << std::endl;
        std::cout << "2: clearCounters" << std::endl;
        std::cout << "3: getClrCnts" << std::endl;
        std::cout << "4: getIdStatus" << std::endl;
        std::cout << "5: getLaneRxCounter" << std::endl;
        std::cout << "6: getLaneTxCounter" << std::endl;
        std::cout << "7: getLastRxPkt" << std::endl;
        std::cout << "8: getLastTxPkt" << std::endl;
        std::cin >> l_option;
    }
    return EXIT_SUCCESS;
}
