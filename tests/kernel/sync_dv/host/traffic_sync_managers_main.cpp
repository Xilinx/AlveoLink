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
        std::cout << argv[0] << " <xclbin> <devId> <waitCycles> <flushCounter> [startId]" << std::endl;
        std::cout << "manager.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    
    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    unsigned int l_devId = atoi(argv[l_idx++]);
    unsigned int l_waitCycles = atoi(argv[l_idx++]);
    unsigned int l_flushCounter = atoi(argv[l_idx++]);
    unsigned int l_startId = 0;
    
    if (argc > 5) {
        l_startId = atoi(argv[l_idx++]);
    }

    unsigned int l_numDevs = 0;
    unsigned int l_syncManagerId = 0;


    AlveoLink::common::FPGA l_card;
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
    AlveoLink::kernel::trafficManagerHost<AL_netDataBits> l_trafficManager;

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
    l_syncManagerId = l_ids[0];
    l_numDevs = l_syncManagerId - l_startId;

    std::cout <<"INFO: sync manager id is " << l_syncManagerId << std::endl;
    std::cout <<"INFO: number of devices is " << l_numDevs << std::endl;

    l_manager.init(&l_card);
    l_manager.createCU(0);
    l_manager.setConfigBuf(l_numDevs, l_waitCycles, l_flushCounter, l_startId);
    l_manager.sendBO();
    int l_option = 0;
    l_manager.runCU();

    l_trafficManager.init(&l_card);
    l_trafficManager.createCU(0);
    l_trafficManager.setConfigBuf(l_ids[1], l_ids[1], l_syncManagerId, l_waitCycles, (uint32_t)((256<<20)-1));
//    size_t l_pktBytes = 1;
  //  l_pktBytes = l_pktBytes << 30;
    //uint8_t* l_pktStore = (uint8_t*)(l_trafficManager.createPktStore(l_pktBytes));
    l_trafficManager.sendBO();
    l_trafficManager.runCU();

    l_manager.finish();
    l_trafficManager.finish();
    uint32_t* l_tmBuf = (uint32_t*)(l_trafficManager.getBufRes());
    std::cout << "INFO TM::rdAddr = " << l_tmBuf[0] << std::endl; 
    std::cout << "INFO TM::wrAddr = " << l_tmBuf[1] << std::endl;
    std::cout << "INFO: system run finished!" << std::endl;
    return EXIT_SUCCESS;
}
