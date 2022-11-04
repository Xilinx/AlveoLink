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
#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "manager.hpp"


constexpr unsigned int t_NetDataBytes = AL_netDataBits / 8;

int main(int argc, char** argv) {
    if (argc <5  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <ip_file> <flushCounter> <microSeconds4query> <microSeconds4retran>" << std::endl;
        std::cout << "manager.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    int l_idx = 1;
    std::string l_ipFileName = argv[l_idx++];
    int l_flushCounter = atoi(argv[l_idx++]);
    int l_microSeconds4Query = atoi(argv[l_idx++]);
    int l_microSec4retran = atoi(argv[l_idx++]);
    AlveoLink::kernel::Manager<AL_maxConnections, AL_mtuBytes, t_NetDataBytes> l_manager(l_ipFileName, l_microSec4retran);
    std::cout << "Please enter ctrl+c to stop the manager." << std::endl;
    do {
        std::cout << "system started..."<< std::endl;
        double l_timeSec = l_manager.process(l_flushCounter, l_microSeconds4Query);
        std::cout << "INFO: system run time = " << l_timeSec << " seconds" << std::endl;
    } while (1);
    return EXIT_SUCCESS;
}
