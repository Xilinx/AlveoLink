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

#include "netLayer.hpp"
#include "genColHost.hpp"
#include "popl.hpp"

using namespace popl;

int main(int argc, char** argv) {
    OptionParser op("Allowed options");
    auto help_option = op.add<Switch>("h", "help", "produce help message");
    auto devID_1 = op.add<Value<int>, Attribute::optional>("d", "dev_1", "Device 1 PCIe number", 0);
    auto devID_2 = op.add<Value<int>, Attribute::optional>("k", "dev_2", "Device 2 PCIe number", 1);
    auto xclbin = op.add<Value<std::string> >("x", "xclbin", "xclbin", "build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/binary_container_1.xclbin");
    auto sender_ID = op.add<Value<int> >("s", "sender_id", "sender id", 1); //sender device id
    auto packet_cnt = op.add<Value<int> >("c", "cnt", "packet count", 32);

    op.parse(argc, argv);

    if (help_option->count() == 1) {
        std::cout << op << "\n";
        return 0;
    } else if (help_option->count() == 2) {
        std::cout << op.help(Attribute::advanced) << "\n";
        return 0;

    } else if (help_option->count() > 2) {
        std::cout << op.help(Attribute::expert) << "\n";
        return 0;
    }

    AlveoLink::common::FPGA fpga_card[2];
    fpga_card[0].setId(devID_1->value());
    fpga_card[1].setId(devID_2->value());
   
    AlveoLink::network_roce_v2::NetLayer<2> l_netLayer[2];
 
    std::cout << "Loading xclbin to FPGA_" << devID_1->value() << std::endl;
    fpga_card[0].load_xclbin(xclbin->value());
    std::cout << "Loading xclbin to FPGA_" << devID_2->value() << std::endl;
    fpga_card[1].load_xclbin(xclbin->value());

    for (auto i=0; i<2; ++i) {
        l_netLayer[i].init(&(fpga_card[i]));
        for (auto j=0; j<2; ++j) {
            l_netLayer[i].setIPSubnet(j, 0x0000a8c0);
            l_netLayer[i].setMACSubnet(j, 0x347844332211);
            l_netLayer[i].setID(j, i*2+j);
        }
    }
    for (auto i=0; i<2; ++i) {
        for (auto j=0; j<2; ++j) {
            std::cout <<"INFO: turn on RS_FEC for device " << i << " port " <<j << std::endl;
            l_netLayer[i].turnOn_RS_FEC(j, true);
        }
    }
    unsigned int l_totalDevLinksUp = 0;
    while (l_totalDevLinksUp < 2) {
        std::cout << "INFO: Waiting for links up on device " << l_totalDevLinksUp << std::endl;
        if (l_netLayer[l_totalDevLinksUp].linksUp()) {
            l_totalDevLinksUp++;
        }
    }

    genColHost<2> l_genColHost[2];
    unsigned int l_genCol[2][2];
    unsigned int l_id[2][2];
    unsigned int l_numPkts[2][2];
    for (auto i=0; i<2; ++i) {
        for (auto j=0; j<2; ++j) {
            l_id[i][j] = i*2+j;
            if (sender_ID->value() == i) {
                l_genCol[i][j] = 1;
            }
            else {
                l_genCol[i][j] = 0;
            }
            l_numPkts[i][j] = packet_cnt->value();
        }
    }
    for (auto i=0; i<2; ++i) {
        l_genColHost[i].init(&(fpga_card[i]));
        if (l_genCol[i][0] == 0) {
            std::cout << "INFO: start collectors on device " << i << std::endl;
            l_genColHost[i].runCollectors(l_genCol[i], l_id[i], l_numPkts[i]);
        }
    }
    for (auto i=0; i<2; ++i) {
        if (l_genCol[i][0] == 1) {
            std::cout << "INFO: start generators on device " << i << std::endl;
            l_genColHost[i].runGenerators(l_genCol[i], l_id[1-i], l_numPkts[i]);
        }
    }
    for (auto i=0; i<2; ++i) {
        l_genColHost[i].finish();
    }

    unsigned int l_errors[2];
    for (auto i=0; i<2; ++i) {
        if (l_genCol[i][0] == 0) {
            l_genColHost[i].getErrors(l_errors);
            for (auto j=0; j<2; ++j) {
                if (l_errors[j] != 0) {
                    std::cout << "ERROR: device " << i << " port " <<j <<" has collector errors." << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }
    std::cout <<"Test pass!" << std::endl;
    return EXIT_SUCCESS;
}
