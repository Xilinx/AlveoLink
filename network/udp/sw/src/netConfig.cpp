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
    if (argc != 5 || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <XCLBIN File> <device id> <all_ip.txt> <my_ip>" << std::endl;
        std::cout << "netConfig.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }

    int l_idx = 1;
    std::string l_xclbinName = argv[l_idx++];
    int l_devId = 0;
    l_devId = atoi(argv[l_idx++]);
    std::string all_ip_file = argv[l_idx++];
    std::string l_myIP_string = argv[l_idx++];
    int l_infId = 0;

    // read all IP
    std::ifstream all_ip(all_ip_file);
    std::vector<std::string> ip_list;
    if (all_ip.is_open()) {
        std::string l_ip;
        while (getline(all_ip, l_ip)) {
            ip_list.push_back(l_ip);
        }
        all_ip.close();
    }


    char localHostName[256];
    gethostname(localHostName, 256);
    std::string l_table_file = std::string(localHostName) + "_" + std::to_string(l_devId) + "_sockets.txt";

    AlveoLink::common::FPGA l_card;
    AlveoLink::network_udp::NetLayer<AL_numInf, AL_maxNumSockets> l_netLayer;
    l_card.setId(l_devId);
    l_card.load_xclbin(l_xclbinName);
    std::cout << "INFO: loading xclbin successfully!" << std::endl;
    l_netLayer.init(&l_card);

    uint32_t l_myIP = AlveoLink::network_udp::getIPint(l_myIP_string);
    std::map<std::string, bool> linkStatus;
    bool l_linkUp=false;
    do {
        linkStatus = l_netLayer.linkStatus(l_infId);
        l_linkUp = linkStatus["rx_status"] && !linkStatus["tx_local_fault"] && !linkStatus["rx_local_fault"] && !linkStatus["rx_aligned_err"];
    } while (!l_linkUp);
    std::cout << "Link " << l_infId << " is up." << std::endl;
    for (auto& item : linkStatus) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    l_netLayer.updateIpAddress(l_infId, l_myIP);
    std::cout << "My ip address is: " << AlveoLink::network_udp::getIPstr(l_myIP) << std::endl;

    std::vector<AlveoLink::network_udp::SocketType> l_socketTable;
    std::ofstream l_table(l_table_file);
    l_table << std::string(localHostName) << " ";
    l_table << l_myIP_string << " ";
    l_table << l_xclbinName << " ";
    l_table << std::to_string(l_devId) << " " <<std::endl;

    int i = 0;
    for (auto& l_ip : ip_list) {
//        if (l_ip != l_myIP_string) {
            AlveoLink::network_udp::SocketType l_socket;
            l_socket.theirIP = l_ip;
            l_socket.theirIPint = AlveoLink::network_udp::getIPint(l_ip);
            l_socket.valid = true;
            l_socketTable.push_back(l_socket);
            l_table << i << " " << l_myIP_string << " " << l_ip << std::endl;
            i++;
//        }
    }
    l_table.close();

    l_netLayer.populateSocketTable(l_infId, l_socketTable);
    return EXIT_SUCCESS;
}
