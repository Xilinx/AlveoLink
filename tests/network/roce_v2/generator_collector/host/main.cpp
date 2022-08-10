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
#include "popl.hpp"

using namespace popl;

int main(int argc, char** argv) {
    OptionParser op("Allowed options");
    auto help_option = op.add<Switch>("h", "help", "produce help message");
    auto devID_1 = op.add<Value<int>, Attribute::optional>("d", "dev_1", "Device 1 PCIe number", 0);
    auto devID_2 = op.add<Value<int>, Attribute::optional>("k", "dev_2", "Device 2 PCIe number", 1);
    auto xclbin = op.add<Value<std::string> >("x", "xclbin", "xclbin", "build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/binary_container_1.xclbin");
    auto sender_ID = op.add<Value<int> >("s", "sender_id", "sender id", 1);
    auto reciver_ID = op.add<Value<int> >("r", "reciver_id", "reciver id", 2);
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

    if (reciver_ID->value() == sender_ID->value()) {
        std::cout << "ERROR: Same ID for both sending and reciving" << std::endl;
        return -1;
    }

    AlveoLink::common::FPGA fpga_card0, fpga_card1;
    fpga_card0.setId(devID_1->value());
    fpga_card1.setId(devID_2->value());
    
    AlveoLink::network_roce_v2::HiveNet hivenet0, hivenet1;
    AlveoLink::network_roce_v2::KernelCMAC cmac_0, cmac_1;
    AlveoLink::common::IP ip_gen;
    AlveoLink::common::IP ip_coll;


    std::cout << "Loading xclbin to FPGA_" << devID_1->value() << std::endl;
    fpga_card0.load_xclbin(xclbin->value());
    std::cout << "Loading xclbin to FPGA_" << devID_2->value() << std::endl;
    fpga_card1.load_xclbin(xclbin->value());

    if (sender_ID->value() == 1 || sender_ID->value() == 2) {
        std::cout << "Configuring sender HiveNet on Fpga_" << 0 << std::endl;
        hivenet0.fpga(&fpga_card0);
        hivenet0.initCU((sender_ID->value()) % 2);
        hivenet0.setIPSubnet(0x0000a8c0);
        hivenet0.setMACSubnet(0x347844332211);
        hivenet0.setLocalID(sender_ID->value());

        std::cout << "Configuring Cmac on Fpga_" << 0 << std::endl;
        cmac_0.fpga(&fpga_card0);
        cmac_0.initCU((sender_ID->value()) % 2);
        ip_gen.fpga(&fpga_card0);
        ip_gen.getIP("generator_collector:{generator_collector_" + std::to_string((sender_ID->value()) % 2) + "}");

    } else {
        std::cout << "Configuring sender HiveNet on Fpga_" << 1 << std::endl;
        hivenet0.fpga(&fpga_card1);
        hivenet0.initCU((sender_ID->value()) % 2);
        hivenet0.setIPSubnet(0x0000a8c0);
        hivenet0.setMACSubnet(0x347844332211);
        hivenet0.setLocalID(sender_ID->value());

        std::cout << "Configuring Cmac on Fpga_" << 1 << std::endl;
        cmac_0.fpga(&fpga_card1);
        cmac_0.initCU((sender_ID->value()) % 2);
        ip_gen.fpga(&fpga_card1);
        ip_gen.getIP("generator_collector:{generator_collector_" + std::to_string((sender_ID->value()) % 2) + "}");
    }

    if (reciver_ID->value() == 1 || reciver_ID->value() == 2) {
        std::cout << "Configuring reciver HiveNet on Fpga_" << 0 << std::endl;
        hivenet0.fpga(&fpga_card0);
        hivenet0.initCU((reciver_ID->value()) % 2);
        hivenet0.setIPSubnet(0x0000a8c0);
        hivenet0.setMACSubnet(0x347844332211);
        hivenet0.setLocalID(reciver_ID->value());

        std::cout << "Configuring Cmac on Fpga_" << 0 << std::endl;
        cmac_1.fpga(&fpga_card0);
        cmac_1.initCU((reciver_ID->value()) % 2);
        ip_coll.fpga(&fpga_card0);
        ip_coll.getIP("generator_collector:{generator_collector_" + std::to_string((reciver_ID->value()) % 2) + "}");

    } else {
        std::cout << "Configuring reciver HiveNet on Fpga_" << 1 << std::endl;
        hivenet0.fpga(&fpga_card1);
        hivenet0.initCU((reciver_ID->value()) % 2);
        hivenet0.setIPSubnet(0x0000a8c0);
        hivenet0.setMACSubnet(0x347844332211);
        hivenet0.setLocalID(reciver_ID->value());

        std::cout << "Configuring Cmac on Fpga_" << 0 << std::endl;
        cmac_1.fpga(&fpga_card1);
        cmac_1.initCU((reciver_ID->value()) % 2);

        ip_coll.fpga(&fpga_card0);
        ip_coll.getIP("generator_collector:{generator_collector_" + std::to_string((reciver_ID->value()) % 2) + "}");
    }

    std::cout << "Configuring RS-FEC on Cmac_0" << std::endl;
    cmac_0.turnOn_RS_FEC(true);
    std::cout << "Configuring RS-FEC on Cmac_1" << std::endl;
    cmac_1.turnOn_RS_FEC(true);

    std::cout << "Waiting for links on cmacs" << std::endl;
    for (int i = 0; i < 16; ++i) {
        int link = 0;
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

    ip_gen.writeReg(0x18, 0);  // reset
    ip_coll.writeReg(0x18, 0); // reset

    ip_gen.writeReg(0x10, 1);                   // using this as generator
    ip_gen.writeReg(0x20, reciver_ID->value()); // remote ID
    ip_gen.writeReg(0x48, packet_cnt->value()); // cnt

    ip_coll.writeReg(0x10, 0); // using this as collector

    ip_coll.writeReg(0x20, reciver_ID->value()); // remote ID

    ip_coll.writeReg(0x18, 1); // start
    ip_gen.writeReg(0x18, 1);  // start
    sleep(1);

    std::cout << std::endl << std::endl << "Packet sent:" << std::dec << ip_gen.readReg(0x38) << std::endl;
    std::cout << "Packet recived:" << std::dec << ip_coll.readReg(0x38) << std::endl;
    std::cout << "Error:" << std::dec << ip_coll.readReg(0x50) << std::endl << std::endl;

    if (ip_gen.readReg(0x38) == ip_coll.readReg(0x38)) {
        std::cout << "Packet count match:  PASS" << std::endl;
    } else {
        std::cout << "Packet count missmatch:  FAIL" << std::endl;
        std::cout << "Errors:  FAIL" << std::endl;
        return -1;
    }

    if (ip_coll.readReg(0x50) == 0) {
        std::cout << "No Errors:  PASS" << std::endl;
    } else {
        std::cout << "Errors:  FAIL" << std::endl;
    }
    return 0;
}
