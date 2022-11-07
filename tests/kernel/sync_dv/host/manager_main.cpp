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
#include "dvNetLayer.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc <5  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <waitCycles> <flushCounter> [debug]" << std::endl;
        std::cout << "manager.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_waitCycles = atoi(argv[l_idx++]);
    unsigned int l_flushCounter = atoi(argv[l_idx++]);
    std::string l_debug = "";
    if (argc == 6) {
        l_debug =  argv[l_idx++];
    }

    int l_numDevs;


    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
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
    l_numDevs = l_ids[0];

    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter);
    l_manager.sendBO();
    if (l_debug != "debug") {
        std::cout << "Please enter ctrl+c to stop the manager." << std::endl;
    }
    int l_option = 0;
    do {
        l_manager.runCU();

        if (l_debug != "debug") {
            l_manager.finish();
            std::cout << "INFO: system run finished!" << std::endl;
        }
        else {
            std::cout << "Please choose one of the following options: " << std::endl;
            std::cout << "0: stop the manager" << std::endl;
            std::cout << "1: wait until system run finishes" << std::endl;
            std::cout << "2: clearCounters" << std::endl;
            std::cout << "3: getClrCnts" << std::endl;
            std::cout << "4: getIdStatus" << std::endl;
            std::cout << "5: getLaneRxCounter" << std::endl;
            std::cout << "6: getLaneTxCounter" << std::endl;
            std::cout << "7: getLastRxPkt" << std::endl;
            std::cout << "8: getLastTxPkt" << std::endl;
            std::cin >> l_option;
            if (l_option == 0) {
                l_manager.finish();
                std::cout << "INFO: system run finished!" << std::endl;
            }
            if (l_option == 1) {
                l_manager.finish();
                std::cout << "INFO: system run finished!" << std::endl;
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
            if (l_option != 0) {
                std::cout << "INFO: Please enter \'c\' to continue..." << std::endl;
                char l_unused;
                std::cin >> l_unused;
            }
        }
    } while ((l_debug != "debug") || ((l_option != 0) && (l_debug != "debug")));
    return EXIT_SUCCESS;
}
