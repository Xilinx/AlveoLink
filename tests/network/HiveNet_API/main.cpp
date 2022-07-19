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
#include <stdint.h>

#include "HiveNet.hpp"
#include "cmac.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "There is no enough arguments to run kernels! argc = " << argc << std::endl;
        return 1;
    }
    std::string binaryFile = argv[1];
    AlveoLink::common::FPGA fpga_card0, fpga_card1;
    fpga_card0.setId(0);
    fpga_card1.setId(1);
    std::cout << "Loading xclbin to FPGA_0" << std::endl;
    fpga_card1.load_xclbin(binaryFile);
    std::cout << "Loading xclbin to FPGA_1" << std::endl;
    fpga_card0.load_xclbin(binaryFile);

    AlveoLink::network_roce_v2::HiveNet hivenet0, hivenet1;
    std::cout << "Configuring HiveNet_0 on Fpga_0" << std::endl;

    hivenet0.fpga(&fpga_card0);
    hivenet0.initCU(0);
    hivenet0.setLocalID(1);

    std::cout << "Configuring HiveNet_0 on Fpga_1" << std::endl;
    hivenet1.fpga(&fpga_card1);
    hivenet1.initCU(0);
    hivenet1.setLocalID(20);

    AlveoLink::network_roce_v2::KernelCMAC cmac_0, cmac_1;
    cmac_0.fpga(&fpga_card0);
    cmac_1.fpga(&fpga_card1);
    cmac_0.initCU(0);
    cmac_1.initCU(0);

    std::cout << "Configuring RS-FEC on Fpga_0 and Fpga_1" << std::endl;
    cmac_0.turnOn_RS_FEC(true);
    cmac_1.turnOn_RS_FEC(true);

    std::cout << "Waiting for links on cmacs" << std::endl;
    for (int i = 0; i < 16; ++i) {
        int link = 0;
        ;
        auto cmac_0_link_status = cmac_0.linkStatus();

        auto it_0 = cmac_0_link_status.find("rx_aligned");
        if (it_0 != cmac_0_link_status.end()) {
            std::cout << "Cmac_0 rx_aligned: " << it_0->second << std::endl;
            if (it_0->second) {
                link++;
            }
        }
        auto cmac_1_link_status = cmac_1.linkStatus();

        auto it_1 = cmac_1_link_status.find("rx_aligned");
        if (it_1 != cmac_1_link_status.end()) {
            std::cout << "Cmac_1 rx_aligned: " << it_1->second << std::endl;
            if (it_1->second) {
                link++;
            }
        }
        if (link == 2) {
            break;
        }
        std::cout << "\n";
        sleep(1);
    }

    AlveoLink::common::IP ip_gen_0; 
    AlveoLink::common::IP ip_coll_0;
    
    ip_gen_0.fpga(&fpga_card0);
    ip_coll_0.fpga(&fpga_card1);
    
    ip_gen_0.getIP("generator_user_latency");
    ip_coll_0.getIP("generator_user_latency");
    
    ip_gen_0.writeReg(0x18, 1);
    ip_gen_0.writeReg(0x20, 20);

    ip_coll_0.writeReg(0x18, 0);
    ip_coll_0.writeReg(0x20, 1);

    ip_coll_0.writeReg(0x10, 1);
    ip_gen_0.writeReg(0x10, 1);

    sleep(2);
    ip_gen_0.readReg(0x28);
    std::cout << "Avarage latency:" << std::dec << ip_gen_0.readReg(0x28) * 3 << " ns" << std::endl;
    return 0;
}
