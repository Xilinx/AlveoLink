/*
 * Copyright 2019-2021 Xilinx, Inc.
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

// This file is required for OpenCL C++ wrapper APIs
#include "netLayer.hpp"

int main(int argc, char** argv) {
    if (argc != 3 || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <XCLBIN File> <device id>" << std::endl;
        std::cout << "arpDisc.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }

    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    int l_devId = 0;
    l_devId = atoi(argv[l_idx++]);
    int l_infId = 0;

    AlveoLink::common::FPGA l_card;
    AlveoLink::network_udp::NetLayer<AL_numInf, AL_maxNumSockets> l_netLayer;
    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;
    l_netLayer.init(&l_card);

    l_netLayer.arpDiscovery(l_infId);
    std::map<int, std::pair<std::string, std::string> > table = l_netLayer.readARPTable(l_infId);
    std::cout << "INFO: ARP table for device " << l_devId << " is " << std::endl;
    for (auto& item : table) {
        std::cout << item.first << ": {MAC address: " << item.second.first << ", IP address: " << item.second.second
                  << "}" << std::endl;
    }

    return EXIT_SUCCESS;
}
