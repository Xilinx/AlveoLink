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
static int readPSN;
static int writePSN;
static unsigned int buffSize;
static unsigned int stat_rx_status_cmac;
static unsigned int stat_tx_status_cmac;

static unsigned int stat_rx_aligned_err_mask = 0x00000008;
static unsigned int stat_rx_local_fault_mask = 0x00000040;
static unsigned int stat_rx_remote_fault_mask = 0x00000020;
static unsigned int stat_rx_status_mask = 0x00000001;
static unsigned int stat_tx_local_fault_mask = 0x00000001;

static bool stat_rx_aligned_err;
static bool stat_rx_local_fault;
static bool stat_rx_remote_fault;
static bool stat_rx_status;
static bool stat_tx_local_fault;

void getBufSize(xrt::ip& ip, vector<xrt::xclbin::arg>& args) {
    cout << "Info about Buffer size\n\n";
    buffSize = ip.read_register(args[9].get_offset());
    readPSN = ip.read_register(args[7].get_offset());
    writePSN = ip.read_register(args[8].get_offset());
    cout << "Read pointer is " << hex << showbase << readPSN << endl;
    cout << "Write pointer is " << hex << showbase << writePSN << endl;
    cout << "Buffer size is " << hex << showbase << buffSize << endl;
    // cout<<"Round-trip latency is "<<hex<<showbase<<(clck_ns*buffSize*2)<<" ns\n"<<endl;
}

void cmacLinkStatus(xrt::ip& ip, vector<xrt::xclbin::arg>& args) {
    cout << "Info about CMAC\n\n";
    stat_rx_status_cmac = ip.read_register(args[9].get_offset());

    stat_rx_aligned_err = stat_rx_aligned_err_mask & stat_rx_status_cmac;

    cout << "stat_rx_aligned_err = " << stat_rx_aligned_err << endl;

    stat_rx_local_fault = stat_rx_local_fault_mask & stat_rx_status_cmac;
    cout << "stat_rx_local_fault = " << stat_rx_local_fault << endl;

    stat_rx_remote_fault = stat_rx_remote_fault_mask & stat_rx_status_cmac;
    cout << "stat_rx_remote_fault = " << stat_rx_remote_fault << endl;

    stat_rx_status = stat_rx_status_mask & stat_rx_status_cmac;
    cout << "stat_rx_status = " << stat_rx_status << endl;

    stat_tx_status_cmac = ip.read_register(args[8].get_offset());

    stat_tx_local_fault = stat_tx_local_fault_mask & stat_tx_status_cmac;
    cout << "stat_tx_local_fault = " << stat_tx_local_fault << "\n\n";
}

void updateID(xrt::ip& ip, vector<xrt::xclbin::arg>& args, unsigned short localID) {
    cout << "Changing local ID to " << localID << endl;
    ip.write_register(args[0].get_offset(), localID);

    // cout<<"Sorry, you need to do this via file for now..\nP.S. After change in file, you'll need to xbutil reset --d
    // <BDF>.\n";
}

void update_genID(xrt::ip& gen_ip, vector<xrt::xclbin::arg>& gen_args, unsigned short genID) {
    cout << "Changing local ID to " << hex << genID << endl;
    gen_ip.write_register(gen_args[2].get_offset(), genID);
}

void turnON_fec(xrt::ip& ip, vector<xrt::xclbin::arg>& args, bool enable_fec) {
    switch (enable_fec) {
        case 0:
            cout << "Disabling RS-FEC\n";
            ip.write_register(args[67].get_offset(), 0);
            break;
        case 1:
            cout << "Enabling RS-FEC\n";
            ip.write_register(args[67].get_offset(), rs_fecON);
    }
}

// void gain_info(xrt::ip& ip_hivenet, xrt::ip& ip_cmac, vector<xrt::xclbin::arg>& args_hivenet,
// vector<xrt::xclbin::arg>& args_cmac, bool& end_f)
//{
//	bool answer;
//	int entered_step;
//	cout<<"Do you want to give or gain some information?(answer 0/1)"<<endl;
//	cin>>answer;
//	cout<<endl;
//
//	if(answer)
//	{
//		cout<<"To give or gain specific information, please, type corresponding number:\n";
//		cout<<"Change Local ID: 0\n";
//		cout<<"Check CMAC status: 1\n";
//		cout<<"Check Buffer sizes: 2\n";
//		cout<<"End run process: 3\n"<<endl;
//		cin>>entered_step;
//
//		switch(entered_step)
//		{
//		case 0:
//			unsigned short temp_local_ID;
//			cout<<" Please, write local ID in little-endian byte order"<<endl;
//			cin>>temp_local_ID;
//			updateID(ip_hivenet, args_hivenet,temp_local_ID);
//			break;
//
//		case 1:
//			cmacLinkStatus(ip_cmac, args_cmac);
//			break;
//		case 2:
//			getBufSize(ip_hivenet, args_hivenet);
//			break;
//		case 3:
//			end_f= true;
//			break;
//		default:
//			cout<<"Your answer doesn't correspond to any option mentioned above\n";
//			break;
//		}
//
//	}
//
//}
