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
#include "trafficManagerHost.hpp"
#include "dvNetLayer.hpp"

constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;
constexpr unsigned int t_DestBytes = AL_destBits / 8;


int main(int argc, char** argv) {
    if (argc <5  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <xclbin> <devId> <waitCycles> <syncManagerId>" << std::endl;
        std::cout << "manager.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_waitCycles = atoi(argv[l_idx++]);
    unsigned int l_syncManagerId = atoi(argv[l_idx++]);
    

    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    AlveoLink::kernel::trafficManagerHost<AL_netDataBits> l_trafficManager[2];

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

    std::cout <<"INFO: sync manager id is " << l_syncManagerId << std::endl;

    for (unsigned int i=0; i<2; ++i) {
        l_trafficManager[i].init(&l_card);
        l_trafficManager[i].createCU(i);
        std::cout << "INFO: my id is: " << l_ids[i] << " my TM id is: " <<l_ids[i] << std::endl;
        l_trafficManager[i].setConfigBuf(l_ids[i], l_ids[i], l_syncManagerId, l_waitCycles, (uint32_t)((128<<20)-1));
        l_trafficManager[i].sendBO();
        l_trafficManager[i].runCU();
    }

    for (unsigned int i=0; i<2; ++i) {
        l_trafficManager[i].finish();
        uint32_t* l_tmBuf = (uint32_t*)(l_trafficManager[i].getBufRes());
        std::cout << "INFO TM[" <<i<<"]::rdAddr = " << l_tmBuf[16] << std::endl; 
        std::cout << "INFO TM[" <<i<<"]::wrAddr = " << l_tmBuf[17] << std::endl;
    }
    std::cout << "INFO: system run finished!" << std::endl;
    return EXIT_SUCCESS;
}
