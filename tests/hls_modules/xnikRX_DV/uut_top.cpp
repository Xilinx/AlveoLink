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
#include "uut_top.hpp"

void loadPkts(uint32_t p_control,
              ap_uint<AL_netDataBits>* p_inPkts,
              hls::stream<ap_uint<AL_netDataBits> >& p_netStr,
              hls::stream<ap_uint<AL_netDataBits> >& p_txStr) {
    ap_uint<32> l_control = p_control;
    bool l_syncTX = l_control[0]; //true: send pkt to tx and net stream in same cycle
    bool l_syncMgr = l_control[1];
    bool l_exit = false;
    unsigned int l_idx = 0;
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_val = p_inPkts[l_idx];
        if (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::done) {
            p_txStr.write(l_val);
            if (l_syncTX) {
                ap_uint<AL_netDataBits> l_dat = l_val;
                if (l_syncMgr) {
                    l_dat(23,20) =  AlveoLink::kernel::PKT_TYPE::query_status;
                }
                else {
                    l_dat(23,20) =  AlveoLink::kernel::PKT_TYPE::workload;
                }
                p_netStr.write(l_dat);
            }
        }
        else if (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::config) {
            p_txStr.write(l_val);
        }
        else {
            p_netStr.write(l_val);
        }
        l_idx++;
        l_exit = (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::terminate);
    }
}

void storePkt(const AlveoLink::kernel::PKT_TYPE p_lastPktType,
              hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
              ap_uint<AL_netDataBits>* p_recPkts){

    uint32_t l_numPkts=1;
    bool l_exit = false;
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_val = p_inStr.read();
        p_recPkts[l_numPkts] = l_val;
        l_numPkts++;
        l_exit = (l_val(23,20) == p_lastPktType);
    }
    ap_uint<AL_netDataBits> l_dat0 = 0;
    l_dat0(31,0) = (l_numPkts-1);
    p_recPkts[0] = l_dat0;
}

void uut_top(uint32_t p_control,
             ap_uint<AL_netDataBits> p_inPkts[MAX_NUMPKTS],
             ap_uint<AL_netDataBits> p_rxPkts[MAX_NUMPKTS],
             ap_uint<AL_netDataBits> p_xnik2nhopPkts[MAX_NUMPKTS]) { 
    
    AlveoLink::kernel::xnikSync_RX<AL_netDataBits, AL_destBits> l_xnikSyncRX;
    hls::stream<ap_uint<AL_netDataBits> > l_inStr;
    hls::stream<ap_uint<AL_netDataBits> > l_txStr;
    hls::stream<ap_uint<AL_netDataBits> > l_rxStr;
    hls::stream<ap_uint<AL_netDataBits> > l_xnik2nhopStr;
#pragma HLS DATAFLOW
    loadPkts(p_control, p_inPkts, l_inStr, l_txStr);
    l_xnikSyncRX.process(l_inStr, l_txStr, l_rxStr, l_xnik2nhopStr);
    storePkt(AlveoLink::kernel::PKT_TYPE::terminate, l_rxStr, p_rxPkts);
    storePkt(AlveoLink::kernel::PKT_TYPE::done, l_xnik2nhopStr, p_xnik2nhopPkts);
}
