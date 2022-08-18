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

#include <iostream>
#include <signal.h>
#include <thread>

#include "HiveNet.hpp"
#include "cmac.hpp"
#include "popl.hpp"

using namespace popl;

xrt::ip* ip_gen[2] = {nullptr, nullptr}; // for handling sigterm
static void exit_handler(int signum) {
    std::cout << std::endl;
    if (ip_gen != nullptr) {
        if (ip_gen[0] != nullptr) {
            std::cout << "Sttoping generator_0." << std::endl;
            ip_gen[0]->write_register(0x18, 0); // reset
        }
        if (ip_gen[1] != nullptr) {
            std::cout << "Sttoping generator_1." << std::endl;
            ip_gen[1]->write_register(0x18, 0); // reset
        }
    }
    std::cout << "Exitting" << std::endl;
    exit(0);
}
void exit_programm_after_seconds(uint64_t seconds) {
    sleep(seconds);
    exit_handler(0);
}

int main(int argc, char** argv) {
    signal(SIGINT, exit_handler);

    OptionParser op("Allowed options");
    auto help_option = op.add<Switch>("h", "help", "produce help message");
    auto devID_1 = op.add<Value<int>, Attribute::optional>("d", "dev_1", "Device 1 PCIe number", 0);
    auto devID_2 = op.add<Value<int>, Attribute::optional>("k", "dev_2", "Device 2 PCIe number", 1);
    auto xclbin = op.add<Value<std::string> >("x", "xclbin", "xclbin", "binary_container_1.xclbin");

    auto sender_ID = op.add<Value<int> >("s", "sender_device_id", "sender id", 0);

    auto packet_cnt = op.add<Value<uint64_t> >("c", "cnt", "packet count", 32);
    auto max_runtime = op.add<Value<uint64_t> >("t", "time", "MAX runtime in seconds", 3600);

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

    if (sender_ID->value() < 0 || sender_ID->value() > 1) {
        std::cout << "Error: Sender ID can be either 0 or 1." << std::endl;
        return -1;
    }

    AlveoLink::common::FPGA fpga_card0;
    AlveoLink::common::FPGA fpga_card1;
    AlveoLink::network_roce_v2::HiveNet hivenet_s[2], hivenet_r[2];
    AlveoLink::network_roce_v2::KernelCMAC cmac_s[2], cmac_r[2];

    xrt::ip* ip_coll[2];
    fpga_card0.setId(sender_ID->value() == 0 ? devID_1->value() : devID_2->value());
    fpga_card1.setId(sender_ID->value() == 0 ? devID_2->value() : devID_1->value());
    std::cout << "Loading xclbin to FPGA_" << devID_1->value() << std::endl;
    fpga_card0.load_xclbin(xclbin->value());
    std::cout << "Loading xclbin to FPGA_" << devID_2->value() << std::endl;
    fpga_card1.load_xclbin(xclbin->value());

    int senderID[2];
    int receiverID[2];

    senderID[0] = sender_ID->value() == 0 ? 1 : 3;
    senderID[1] = sender_ID->value() == 0 ? 2 : 4;

    receiverID[0] = sender_ID->value() == 0 ? 3 : 1;
    receiverID[1] = sender_ID->value() == 0 ? 4 : 2;

    std::cout << "Configuring sender HiveNets on Fpga_" << sender_ID->value() << std::endl;
    hivenet_s[0].fpga(&fpga_card0);
    hivenet_s[0].initCU(0);
    hivenet_s[0].setIPSubnet(0x0000a8c0);
    hivenet_s[0].setMACSubnet(0x347844332211);
    hivenet_s[0].setLocalID(senderID[0]);

    hivenet_s[1].fpga(&fpga_card0);
    hivenet_s[1].initCU(1);
    hivenet_s[1].setIPSubnet(0x0000a8c0);
    hivenet_s[1].setMACSubnet(0x347844332211);
    hivenet_s[1].setLocalID(senderID[1]);

    std::cout << "Configuring Cmac_0 on Fpga_" << 0 << std::endl;

    cmac_s[0].fpga(&fpga_card0);
    cmac_s[0].initCU(0);

    cmac_s[1].fpga(&fpga_card0);
    cmac_s[1].initCU(1);

    ip_gen[0] = new xrt::ip(fpga_card0.getDevice(), fpga_card0.getUUID(),
                            "generator_collector:{generator_collector_" + std::to_string(0) + "}");
    ip_gen[1] = new xrt::ip(fpga_card0.getDevice(), fpga_card0.getUUID(),
                            "generator_collector:{generator_collector_" + std::to_string(1) + "}");

    std::cout << "Configuring receiver HiveNets on Fpga_" << 1 - sender_ID->value() << std::endl;
    hivenet_r[0].fpga(&fpga_card1);
    hivenet_r[0].initCU(0);
    hivenet_r[0].setIPSubnet(0x0000a8c0);
    hivenet_r[0].setMACSubnet(0x347844332211);
    hivenet_r[0].setLocalID(receiverID[0]);

    hivenet_r[1].fpga(&fpga_card1);
    hivenet_r[1].initCU(1);
    hivenet_r[1].setIPSubnet(0x0000a8c0);
    hivenet_r[1].setMACSubnet(0x347844332211);
    hivenet_r[1].setLocalID(receiverID[1]);

    std::cout << "Configuring Cmac_0 on Fpga_" << 0 << std::endl;

    cmac_r[0].fpga(&fpga_card1);
    cmac_r[0].initCU(0);

    cmac_r[1].fpga(&fpga_card1);
    cmac_r[1].initCU(1);

    ip_coll[0] = new xrt::ip(fpga_card1.getDevice(), fpga_card1.getUUID(),
                             "generator_collector:{generator_collector_" + std::to_string(0) + "}");
    ip_coll[1] = new xrt::ip(fpga_card1.getDevice(), fpga_card1.getUUID(),
                             "generator_collector:{generator_collector_" + std::to_string(1) + "}");

    std::cout << "Configuring RS-FEC on sender Cmacs" << std::endl;
    cmac_s[0].turnOn_RS_FEC(true);
    cmac_s[1].turnOn_RS_FEC(true);
    std::cout << "Configuring RS-FEC on receiver Cmacs" << std::endl;
    cmac_r[0].turnOn_RS_FEC(true);
    cmac_r[1].turnOn_RS_FEC(true);

    std::cout << "Waiting for links on cmacs" << std::endl << std::endl;
    for (int i = 0; i < 16; ++i) {
        int link = 0;
        ;
        auto cmac_0_link_status = cmac_s[0].linkStatus();

        auto it_0 = cmac_0_link_status.find("rx_aligned");
        if (it_0 != cmac_0_link_status.end()) {
            std::cout << "Cmac_1 rx_aligned: " << it_0->second << std::endl;
            if (it_0->second) {
                link++;
            }
        }

        auto cmac_1_link_status = cmac_s[1].linkStatus();

        it_0 = cmac_1_link_status.find("rx_aligned");
        if (it_0 != cmac_1_link_status.end()) {
            std::cout << "Cmac_2 rx_aligned: " << it_0->second << std::endl;
            if (it_0->second) {
                link++;
            }
        }

        auto cmac_2_link_status = cmac_r[0].linkStatus();

        it_0 = cmac_2_link_status.find("rx_aligned");
        if (it_0 != cmac_2_link_status.end()) {
            std::cout << "Cmac_3 rx_aligned: " << it_0->second << std::endl;
            if (it_0->second) {
                link++;
            }
        }

        auto cmac_3_link_status = cmac_r[1].linkStatus();

        it_0 = cmac_3_link_status.find("rx_aligned");
        if (it_0 != cmac_3_link_status.end()) {
            std::cout << "Cmac_4 rx_aligned: " << it_0->second << std::endl;
            if (it_0->second) {
                link++;
            }
        }
        if (link == 4) {
            break;
        }
        std::cout << "\n";
        sleep(1);
    }

    std::thread wait_thread(exit_programm_after_seconds, max_runtime->value());

    //////////////////////////////////////////////////////////////
    // Packet Generation///////////////////////////////////////////
    //////////////////////////////////////////////////////////////

    ip_gen[0]->write_register(0x18, 0);             // reset
    ip_coll[0]->write_register(0x18, 0);            // reset
    ip_gen[0]->write_register(0x10, 1);             // using this as generator
    ip_gen[0]->write_register(0x20, receiverID[0]); // remote ID
    ip_gen[0]->write_register(0x48, 1);             // cnt

    ip_coll[0]->write_register(0x10, 0); // using this as collector

    ip_coll[0]->write_register(0x20, receiverID[0]); // remote ID

    ip_coll[0]->write_register(0x18, 1); // start
    ip_gen[0]->write_register(0x18, 1);  // start

    bool error = false;
    sleep(1);
    error |= ip_gen[0]->read_register(0x38) != ip_coll[0]->read_register(0x38);

    if (!error) {
        std::cout << std::endl
                  << std::endl
                  << "Packets sent 0 :" << std::dec << ip_gen[0]->read_register(0x38) << std::endl;
    } else {
        std::cout << "Packets count missmatch:  FAIL" << std::endl;
        std::cout << "Errors:  FAIL" << std::endl;
        return -1;
    }

    error |= ip_coll[0]->read_register(0x50) != 0;
    if (error) {
        std::cout << "Errors:  FAIL" << std::endl;
        return -1;
    } else {
        std::cout << "No Errors." << std::endl;
    }

    ip_gen[1]->write_register(0x18, 0);             // reset
    ip_coll[1]->write_register(0x18, 0);            // reset
    ip_gen[1]->write_register(0x10, 1);             // using this as generator
    ip_gen[1]->write_register(0x20, receiverID[1]); // remote ID
    ip_gen[1]->write_register(0x48, 1);             // cnt

    ip_coll[1]->write_register(0x10, 0); // using this as collector

    ip_coll[1]->write_register(0x20, receiverID[1]); // remote ID

    ip_coll[1]->write_register(0x18, 1); // start
    ip_gen[1]->write_register(0x18, 1);  // start

    sleep(1);
    error |= ip_gen[1]->read_register(0x38) != ip_coll[1]->read_register(0x38);

    if (!error) {
        std::cout << std::endl
                  << std::endl
                  << "Packets sent 1 :" << std::dec << ip_gen[1]->read_register(0x38) << std::endl;
    } else {
        std::cout << "Packets count missmatch:  FAIL" << std::endl;
        std::cout << "Errors:  FAIL" << std::endl;
        return -1;
    }

    error |= ip_coll[1]->read_register(0x50) != 0;
    if (error) {
        std::cout << "Errors:  FAIL" << std::endl;
        return -1;
    } else {
        std::cout << "No Errors." << std::endl;
    }

    uint64_t full_32_bit_cnt = (packet_cnt->value()) / ((uint32_t)(0xffffffff));
    uint64_t packet_sent[2] = {0};
    std::cout << std::endl << "Packet generation started." << std::endl;
    for (uint64_t i = 0; i < full_32_bit_cnt && !error; i++) {
        ip_gen[0]->write_register(0x18, 0);             // reset
        ip_coll[0]->write_register(0x18, 0);            // reset
        ip_gen[0]->write_register(0x10, 1);             // using this as generator
        ip_gen[0]->write_register(0x20, receiverID[0]); // remote ID
        ip_gen[0]->write_register(0x48, 0xffffffff);    // cnt

        ip_coll[0]->write_register(0x10, 0); // using this as collector

        ip_coll[0]->write_register(0x20, receiverID[0]); // remote ID

        ip_coll[0]->write_register(0x18, 1); // start
        ip_gen[0]->write_register(0x18, 1);  // start

        ip_gen[1]->write_register(0x18, 0);             // reset
        ip_coll[1]->write_register(0x18, 0);            // reset
        ip_gen[1]->write_register(0x10, 1);             // using this as generator
        ip_gen[1]->write_register(0x20, receiverID[1]); // remote ID
        ip_gen[1]->write_register(0x48, 0xffffffff);    // cnt

        ip_coll[1]->write_register(0x10, 0); // using this as collector

        ip_coll[1]->write_register(0x20, receiverID[1]); // remote ID

        ip_coll[1]->write_register(0x18, 1); // start
        ip_gen[1]->write_register(0x18, 1);  // start

        int j = 0;
        while (ip_gen[0]->read_register(0x38) != 0xffffffff || ip_gen[1]->read_register(0x38) != 0xffffffff) {
            sleep(1);
            j++;
            if (j % 10 == 0) {
                std::cout << "Packets sent 0 :" << std::dec << packet_sent[0] + ip_gen[0]->read_register(0x38)
                          << std::endl;
                std::cout << "Packets sent 1 :" << std::dec << packet_sent[1] + ip_gen[1]->read_register(0x38)
                          << std::endl;
            }
        }

        sleep(1);
        error |= ip_gen[0]->read_register(0x38) != ip_coll[0]->read_register(0x38);
        error |= ip_gen[1]->read_register(0x38) != ip_coll[1]->read_register(0x38);

        if (!error) {
            packet_sent[0] += ip_gen[0]->read_register(0x38);
            std::cout << std::endl << std::endl << "Packets sent 0 :" << std::dec << packet_sent[0] << std::endl;
            packet_sent[1] += ip_gen[1]->read_register(0x38);
            std::cout << std::endl << std::endl << "Packets sent 1 :" << std::dec << packet_sent[1] << std::endl;
        } else {
            std::cout << "Packets count missmatch:  FAIL" << std::endl;
            std::cout << "Errors:  FAIL" << std::endl;
            return -1;
        }

        error |= ip_coll[0]->read_register(0x50) != 0;
        error |= ip_coll[1]->read_register(0x50) != 0;
        if (error) {
            std::cout << "Errors:  FAIL" << std::endl;
            return -1;
        } else {
            std::cout << "No Errors." << std::endl << std::endl;
        }
    }

    uint32_t remainder = packet_cnt->value() % (0xffffffff);

    if (remainder != 0) {
        ip_gen[0]->write_register(0x18, 0);             // reset
        ip_coll[0]->write_register(0x18, 0);            // reset
        ip_gen[0]->write_register(0x10, 1);             // using this as generator
        ip_gen[0]->write_register(0x20, receiverID[0]); // remote ID
        ip_gen[0]->write_register(0x48, remainder);     // cnt

        ip_coll[0]->write_register(0x10, 0); // using this as collector

        ip_coll[0]->write_register(0x20, receiverID[0]); // remote ID

        ip_coll[0]->write_register(0x18, 1); // start
        ip_gen[0]->write_register(0x18, 1);  // start

        ip_gen[1]->write_register(0x18, 0);             // reset
        ip_coll[1]->write_register(0x18, 0);            // reset
        ip_gen[1]->write_register(0x10, 1);             // using this as generator
        ip_gen[1]->write_register(0x20, receiverID[1]); // remote ID
        ip_gen[1]->write_register(0x48, remainder);     // cnt

        ip_coll[1]->write_register(0x10, 0); // using this as collector

        ip_coll[1]->write_register(0x20, receiverID[1]); // remote ID

        ip_coll[1]->write_register(0x18, 1); // start
        ip_gen[1]->write_register(0x18, 1);  // start

        int j = 0;
        while (ip_gen[0]->read_register(0x38) != remainder || ip_gen[1]->read_register(0x38) != remainder) {
            sleep(1);
            j++;
            if (j % 10 == 0) {
                std::cout << "Packets sent 0 :" << std::dec << packet_sent[0] + ip_gen[0]->read_register(0x38)
                          << std::endl;
                std::cout << "Packets sent 1 :" << std::dec << packet_sent[1] + ip_gen[1]->read_register(0x38)
                          << std::endl;
            }
        }

        sleep(1);
        error |= ip_gen[0]->read_register(0x38) != ip_coll[0]->read_register(0x38);
        error |= ip_gen[1]->read_register(0x38) != ip_coll[1]->read_register(0x38);

        if (!error) {
            packet_sent[0] += ip_gen[0]->read_register(0x38);
            std::cout << std::endl << std::endl << "Packets sent 0 :" << std::dec << packet_sent[0] << std::endl;
            packet_sent[1] += ip_gen[1]->read_register(0x38);
            std::cout << "Packets sent 1 :" << std::dec << packet_sent[1] << std::endl;
        } else {
            std::cout << "Packets count missmatch:  FAIL" << std::endl;
            std::cout << "Errors:  FAIL" << std::endl;
            return -1;
        }

        error |= ip_coll[0]->read_register(0x50) != 0;
        error |= ip_coll[1]->read_register(0x50) != 0;
        if (error) {
            std::cout << "Errors:  FAIL" << std::endl;
            return -1;
        } else {
            std::cout << "No Errors." << std::endl << std::endl;
        }
    }

    if (!error) {
        std::cout << "No Errors:  PASS" << std::endl;
    } else {
        std::cout << "Errors:  FAIL" << std::endl;
        return -1;
    }

    if (!error) {
        std::cout << "Packets match:  PASS" << std::endl;
    } else {
        std::cout << "Packets missmatch:  FAIL" << std::endl;
        return -1;
    }
    wait_thread.detach();
    return 0;
}
