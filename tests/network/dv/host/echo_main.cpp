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

#include "dvAdapter.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;


int main(int argc, char** argv) {
    if (argc !=3  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId>" << std::endl;
        std::cout << "host.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);

    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvAdapter<AL_maxConnections, AL_destBits> l_dvAdapter;
    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;

    l_dvAdapter.fpga(&l_card);
    l_dvAdapter.createCU(0);
    l_dvAdapter.init();
    std::cout << "Please choose one of the following options: " << std::endl;
    std::cout << "0: quit" << std::endl;
    std::cout << "1: get valid dests" << std::endl;
    std::cout << "2: clearCounters" << std::endl;
    std::cout << "3: getClrCnts" << std::endl;
    std::cout << "4: getIdStatus" << std::endl;
    std::cout << "5: getLaneRxCounter" << std::endl;
    std::cout << "6: getLaneTxCounter" << std::endl;
    int l_option;
    std::cin >> l_option;
    while (l_option != 0) {
        if (l_option == 1) {
            uint16_t l_numDests = l_dvAdapter.getNumDests();
            std::bitset<AL_maxConnections> l_dests = l_dvAdapter.getDestMap();
            std::cout << "INFO: Network Interface 0 has " << l_numDests << " dests." << std::endl;
            for (auto i=0; i<l_numDests; ++i) {
                if (l_dests[i]) {
                     std::cout << "INFO: dest " << i << " is valid." << std::endl;
                }
            }
        }
        if (l_option == 2) {
            l_dvAdapter.clearCounters();
            std::cout << "INFO: reset counters completed." << std::endl;
        }
        if (l_option == 3) {
            unsigned int l_clrCnts = l_dvAdapter.getClrCnts();
            std::cout << "INFO: read out ClrCnts value is: 0x" << std::hex << l_clrCnts << std::endl;
        }
        if (l_option == 4) {
            uint16_t l_id = l_dvAdapter.getMyId();
            bool l_linkUp = l_dvAdapter.isLinkUp();
            std::cout << "INFO: Network Interface 0 has id " << l_id  << std::endl;
            if (l_linkUp) {
                std::cout << "INFO: Network Interface 0 link is up." << std::endl;
            }
            else {
                std::bitset<4> l_laneStatus = l_dvAdapter.getLaneStatus();
                for (auto i=0; i<4; ++i) {
                    if (l_laneStatus[i]) {
                        std::cout << "INFO: lane " << i << " is up." << std::endl;
                    }
                    else {
                        std::cout << "INFO: lane " << i << " is down." << std::endl;
                    }
                }
            }
        }
        if (l_option == 5) {
            std::vector<uint64_t> l_pktCnts = l_dvAdapter.getLaneRxCounter();
            for (auto i=0; i<4; ++i) {
                std::cout << "INFO: lane " << i << " has sent " << l_pktCnts[i] << " packets." << std::endl;
            }
        }
        if (l_option == 6) {
            std::vector<uint64_t> l_pktCnts= l_dvAdapter.getLaneTxCounter();
            for (auto i=0; i<4; ++i) {
                std::cout << "INFO: lane " << i << " has received " << l_pktCnts[i] << " packets." << std::endl;
            }
        }
        std::cout << std::endl;
        std::cout << "INFO: Please enter \'c\' to continue..." << std::endl;
        char l_unused;
        std::cin >> l_unused;
        std::cout << "Please choose one of the following options: " << std::endl;
        std::cout << "0: quit" << std::endl;
        std::cout << "1: check network interface status" << std::endl;
        std::cout << "2: clearCounters" << std::endl;
        std::cout << "3: getClrCnts" << std::endl;
        std::cout << "4: getLaneStatus" << std::endl;
        std::cout << "5: getLaneRxCounter" << std::endl;
        std::cout << "6: getLaneTxCounter" << std::endl;
        std::cin >> l_option;
    }
    return EXIT_SUCCESS;
}
