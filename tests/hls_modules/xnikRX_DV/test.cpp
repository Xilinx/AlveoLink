/*
 * Copyright 2019 Xilinx, Inc.
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
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include "uut_top.hpp"

int main(int argc, char** argv) {
    if (argc !=5  || (std::string(argv[1]) == "-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << argv[0] << " <number_of_pkts> <syn_control> <done_pkts> <mgr_pkts>" << std::endl;
        std::cout << "test.exe -help";
        std::cout << "    -- print out this usage:" << std::endl;
        return EXIT_FAILURE;
    }
    int l_idx = 1;
    unsigned int l_numPkts = atoi(argv[l_idx++]);
    unsigned int l_control = atoi(argv[l_idx++]);
    unsigned int l_numDonePkts = atoi(argv[l_idx++]);
    unsigned int l_mgrPkts = atoi(argv[l_idx++]);
    unsigned int l_goldenSum = 0;

    assert((l_numPkts + l_numDonePkts + l_mgrPkts) < MAX_NUMPKTS);
    assert(l_mgrPkts < l_numDonePkts);
    assert(l_numDonePkts < l_numPkts);

    unsigned int l_donePktInterval = l_numPkts / l_numDonePkts;
    unsigned int l_mgrPktInterval = l_numPkts / l_mgrPkts;

    ap_uint<AL_netDataBits> l_inPkts[MAX_NUMPKTS];
    ap_uint<AL_netDataBits> l_rxPkts[MAX_NUMPKTS];
    ap_uint<AL_netDataBits> l_xnik2nhopPkts[MAX_NUMPKTS]; 
    
    unsigned int l_pktVal = 1;
    unsigned int l_memIdx = 0;
    ap_uint<AL_netDataBits> l_dat = 0;
    //write config pkt
    l_dat(23,20) = AlveoLink::kernel::PKT_TYPE::config;
    l_dat(39,24) = 0;
    l_dat(55,40) = 2;
    l_dat(87,56) = 128;
    l_dat(119,88) = 0;
    l_inPkts[l_memIdx] = l_dat;
    l_memIdx++;
    l_dat=0;
    l_dat(23,20) = AlveoLink::kernel::PKT_TYPE::start;
    l_inPkts[l_memIdx] = l_dat;
    l_memIdx++;
    
    while (l_pktVal <= l_numPkts) {
        l_dat(23,20) = 0;
        l_dat(19,0) = l_pktVal;
        l_inPkts[l_memIdx] = l_dat;
        l_goldenSum += l_pktVal;
        l_memIdx++;
        if ((l_pktVal % l_donePktInterval) == 0) {
            ap_uint<AL_netDataBits> l_doneDat = 0;
            l_doneDat(23,20) = AlveoLink::kernel::PKT_TYPE::done;
            l_inPkts[l_memIdx] = l_doneDat;
            l_memIdx++;
        }
        if ((l_pktVal % l_mgrPktInterval) == 0) {
            ap_uint<AL_netDataBits> l_mgrDat = 0;
            l_mgrDat(23,20) = AlveoLink::kernel::PKT_TYPE::query_status;
            l_inPkts[l_memIdx] = l_mgrDat;
            l_memIdx++;
        }
        l_pktVal++;
    }
    l_dat = 0;
    l_dat(23,20) =  AlveoLink::kernel::PKT_TYPE::done;
    l_inPkts[l_memIdx] = l_dat;
    l_memIdx++;
    l_dat = 0;
    l_dat(23,20) =  AlveoLink::kernel::PKT_TYPE::terminate;
    l_inPkts[l_memIdx] = l_dat;

    uut_top(l_control,l_inPkts,l_rxPkts,l_xnik2nhopPkts);

    ap_uint<AL_netDataBits> l_rxDat = l_rxPkts[0];
    ap_uint<AL_netDataBits> l_xnik2nhopDat = l_xnik2nhopPkts[0];
    uint32_t l_numRxPkts = l_rxDat(31,0);
    uint32_t l_numNhopPkts = l_xnik2nhopDat(31,0);
    unsigned int l_sum = 0;
    std::cout << "INFO: number of rx pkts = " << l_numRxPkts << std::endl;
    for (auto i=0; i<l_numRxPkts; ++i) {
        ap_uint<AL_netDataBits> l_val = l_rxPkts[i+1];
        unsigned int l_type = l_val(23,20);
        unsigned int l_dat = l_val(15,0);
        std::cout << "INFO: rx pkt " << i << " type = " << l_type << " value = " << l_dat << std::endl;
    }
    std::cout << "INFO: number of nhop pkts = " << l_numNhopPkts << std::endl; 
    for (auto i=0; i<l_numNhopPkts; ++i) {
        ap_uint<AL_netDataBits> l_val = l_xnik2nhopPkts[i+1];
        unsigned int l_type = l_val(23,20);
        unsigned int l_dat = l_val(15,0);
        if (l_type == 0) {
            l_sum += l_dat;
        }
        std::cout << "INFO: nHop pkt " << i << " type = " << l_type << " value = " << l_dat << std::endl;
    }
    if (l_sum == l_goldenSum) {
        std::cout << "Test pass!" << std::endl;
    }
    else {
        std::cout << "ERROR: goldenSum " << l_goldenSum << " != outputSum " << l_sum << std::endl;
        std::cout << "Test failed!" << std::endl;
    }
    return EXIT_SUCCESS;
}
