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
#include "whole_func.h"
//#ifdef _WIN32
//#include <Windows.h>
//#else
#include <unistd.h>
#include <stdlib.h>
//#endif
void gain_info(xrt::ip& ip_hivenet,
               xrt::ip& ip_cmac,
               vector<xrt::xclbin::arg>& args_hivenet,
               vector<xrt::xclbin::arg>& args_cmac,
               bool& end_f);

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "There is no enough arguments to run kernels! argc = " << argc << endl;
        return 1;
    }

    static unsigned short local_ID_1 = 0x1;
    static unsigned short local_ID_gen = 0x2;
    static unsigned long timeoutInNs_1 = 0xFFFF;
    static unsigned int myIp_1 = 0x0000a8c0;
    static unsigned long myMac_1 = 0x000025282927;
    static unsigned short nRetransmissions_1 = 3;
    static unsigned short udp_port_1 = 4791;
    static bool arp_clear_n_1 = 1;

    string binaryFile = argv[1];
    int device_index = atoi(argv[3]);

    cout << "Open the device " << device_index << endl;
    auto device = xrt::device(device_index);
    cout << device.get_info<xrt::info::device::name>() << " is opened\n";

    cout << "Load the xclbin " << binaryFile << endl;
    auto uuid = device.load_xclbin(binaryFile);
    cout << "Already loaded\n";

    auto ip_1 = xrt::ip(device, uuid, "HiveNet_kernel_0");
    auto ip_gen_coll = xrt::ip(device, uuid, "krnl_latency");
    auto ip_cmac1 = xrt::ip(device, uuid, "cmac_0");

    vector<xrt::xclbin::ip> cu_1;
    vector<xrt::xclbin::ip> cu_gen_coll;
    vector<xrt::xclbin::ip> cu_cmac1;

    auto xclbin = xrt::xclbin(binaryFile);

    for (auto& kernel : xclbin.get_kernels()) {
        if (kernel.get_name() == "HiveNet_kernel_0") {
            cu_1 = kernel.get_cus();
        }

        else if (kernel.get_name() == "krnl_latency") {
            cu_gen_coll = kernel.get_cus();
        }

        else if (kernel.get_name() == "cmac_0") {
            cu_cmac1 = kernel.get_cus();
        }
    }

    if (cu_1.empty()) throw runtime_error("IP HiveNet_kernel_0 not found in the provided xclbin");
    if (cu_gen_coll.empty()) throw runtime_error("IP krnl_latency not found in the provided xclbin");
    if (cu_cmac1.empty()) throw runtime_error("IP cmac_0 not found in the provided xclbin");

    cout << "Setting IP's arguments" << endl;

    auto args_1 = cu_1[0].get_args();
    auto args_gen_coll1 = cu_gen_coll[0].get_args();
    auto args_cmac1 = cu_cmac1[0].get_args();

    cout << "Starting initialization from file\n";
    ifstream initfile_1;
    string file_1 = argv[2];
    initfile_1.open(file_1);
    if (initfile_1.is_open()) {
        string temp_line;
        istringstream transit_stream;
        bool first_line = true;
        int ind = -1;
        string val_name;
        while (!initfile_1.eof()) {
            getline(initfile_1, temp_line);

            if (temp_line.size() != 0) {
                transit_stream.clear();
                transit_stream.str(temp_line);

                if (first_line) {
                    transit_stream >> ind;
                    cout << "\n" << ind << endl;
                    first_line = !first_line;
                }

                else {
                    unsigned char hex8u_num;
                    unsigned int hex32u_num;
                    unsigned short hex16u_num;
                    unsigned long hex64u_num;

                    switch (ind) {
                        case 0:
                            transit_stream >> hex >> hex16u_num;
                            local_ID_1 = hex16u_num;
                            cout << "Value for local_ID_1 " << hex << showbase << local_ID_1 << endl;
                            break;
                        case 1:
                            transit_stream >> hex >> hex64u_num;
                            timeoutInNs_1 = hex64u_num;
                            cout << "Value for timeoutInNs_1 " << hex << showbase << timeoutInNs_1 << endl;
                            break;
                        case 2:
                            transit_stream >> hex >> hex32u_num;
                            myIp_1 = hex32u_num;
                            cout << "Value for myIp_1 " << hex << showbase << myIp_1 << endl;
                            break;
                        case 3:
                            transit_stream >> hex >> hex64u_num;
                            myMac_1 = hex64u_num;
                            cout << "Value for myMac_1 " << hex << showbase << myMac_1 << endl;
                            break;
                        case 4:
                            transit_stream >> hex >> hex16u_num;
                            nRetransmissions_1 = hex16u_num;
                            cout << "Value for nRetransmissions_1 " << hex << showbase << nRetransmissions_1 << endl;
                            break;
                        case 5:
                            transit_stream >> hex >> hex16u_num;
                            udp_port_1 = hex16u_num;
                            cout << "Value for udp_port_1 " << hex << showbase << udp_port_1 << endl;
                            break;
                        case 6:
                            transit_stream >> hex >> hex8u_num;
                            arp_clear_n_1 = hex8u_num;
                            cout << "Value for arp_clear_n_1 " << hex << showbase << arp_clear_n_1 << endl;
                            break;
                        case 7:
                            transit_stream >> hex >> hex16u_num;
                            local_ID_gen = hex16u_num;
                            cout << "Value for local_ID_gen " << hex << showbase << local_ID_gen << endl;
                    }

                    first_line = !first_line;
                }
            }
        }
    }

    else {
        cout << "Can't open file! Hard coded values will be used." << endl;
    }

    cout << "Setting local_ID_1 \n";
    ip_1.write_register(args_1[0].get_offset(), local_ID_1);

    cout << "Setting timeoutInNs_1 \n";
    ip_1.write_register(args_1[1].get_offset(), timeoutInNs_1);
    ip_1.write_register(args_1[1].get_offset() + 4, timeoutInNs_1 >> 32);

    cout << "Setting myIp_1 \n";
    ip_1.write_register(args_1[2].get_offset(), myIp_1);

    cout << "Setting myMac_1 \n";
    ip_1.write_register(args_1[3].get_offset(), myMac_1);
    ip_1.write_register(args_1[3].get_offset() + 4, myMac_1 >> 32);

    cout << "Setting nRetransmissions_1 \n";
    ip_1.write_register(args_1[4].get_offset(), nRetransmissions_1);

    cout << "Setting udp_port_1 \n";
    ip_1.write_register(args_1[5].get_offset(), udp_port_1);

    cout << "Setting arp_clear_n_1 \n";
    ip_1.write_register(args_1[6].get_offset(), arp_clear_n_1);
    arp_clear_n_1 = 1;

    cout << "Setting local_ID for generator\n";
    ip_gen_coll.write_register(args_gen_coll1[4].get_offset(), local_ID_gen);

    // cmac
    // cout<<"Let's see CMAC status\n";
    // cmacLinkStatus(ip_1, args_1);
    cout << "Do you want to enable RS-FEC? (answer 0/1)" << endl;
    bool enable_fec;
    cin >> enable_fec;
    cout << endl;
    turnON_fec(ip_cmac1, args_cmac1, enable_fec);

    // this will be our ap_start
    bool end_run = true;
    bool turn_gen;

    cout << "Do you want to turn on generator? (answer 0/1)" << endl;
    cin >> turn_gen;
    cout << endl;

    if (turn_gen) {
        cout << "Good, generator will generate packets with " << hex << showbase << local_ID_gen << " id" << endl;
        ip_gen_coll.write_register(args_gen_coll1[1].get_offset(), 1);

        ip_gen_coll.write_register(args_gen_coll1[0].get_offset(), 1);

        cout << "Generating packets until you want to stop!\n";

        for (int bps = 0; bps < 1000; bps++) {
            uint32_t latency = ip_gen_coll.read_register(args_gen_coll1[5].get_offset()) - 5;

            cout << "Latency in clock cycles = " << dec << latency << endl;
            cout << "Latency in ns = " << dec << latency * 3 << endl;
            cout << "Packets in buffer: " << dec << ip_1.read_register(args_1[9].get_offset()) << endl << endl;

            sleep(1);
        }

        // gain_info(ip_1, ip_cmac1, args_1, args_cmac1, end_run);

        if (end_run) {
            cout << "Packet generation has been stopped!\nEnd run" << endl;
            return 0;
        }

    }

    else {
        // cout<<"Well, collector will work for this node"<<endl;
        ip_gen_coll.write_register(args_gen_coll1[1].get_offset(), 0);
        // cout<<"We are ready. Let's start!\n";

        end_run = false;

        while (!end_run) {
            cout << "Packet collection is running!\n" << endl;
            cout << "Packets in buffer: " << dec << ip_1.read_register(args_1[9].get_offset()) << endl;

            sleep(3);
        }
    }

    return 0;
}

void gain_info(xrt::ip& ip_hivenet,
               xrt::ip& ip_cmac,
               vector<xrt::xclbin::arg>& args_hivenet,
               vector<xrt::xclbin::arg>& args_cmac,
               bool& end_f) {
    bool answer;
    int entered_step;
    cout << "Do you want to give/gain some information about the system state?(answer 0/1)" << endl;
    cin >> answer;
    cout << endl;

    if (answer) {
        cout << "To give or gain specific information, please, type corresponding number:\n";
        cout << "Change Local ID: 0\n";
        cout << "Check CMAC status: 1\n";
        cout << "Check Buffer size: 2\n";
        cout << "End run process: 3\n" << endl;
        cin >> entered_step;

        switch (entered_step) {
            case 0:
                unsigned short temp_local_ID;
                temp_local_ID = 1;
                cout << " Please, write local ID in hexadecimal and little-endian byte order" << endl;
                cin >> hex >> temp_local_ID;
                updateID(ip_hivenet, args_hivenet, temp_local_ID);
                break;

            case 1:
                cmacLinkStatus(ip_cmac, args_cmac);
                break;
            case 2:
                getBufSize(ip_hivenet, args_hivenet);
                break;
            case 3:
                end_f = true;
                break;
            default:
                cout << "Your answer doesn't correspond to any option mentioned above\n";
                break;
        }
    }
}
