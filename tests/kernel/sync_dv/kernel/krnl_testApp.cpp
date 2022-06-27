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
#include "interface.hpp"
#include "types.hpp"
#include "xnikPktDefs.hpp"

constexpr unsigned int t_NumInt16s = AL_netDataBits / AL_destBits;

void recData(ap_uint<AL_netDataBits>* p_nHopDataRecPtr,
             hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
             hls::stream<ap_uint<AL_netDataBits> >& p_ctrlStr,
             unsigned int p_numTotalInts) {
    const static unsigned int t_NumInts = (AL_netDataBits/sizeof(uint32_t));
    bool l_exit = false;
    bool l_recDone = false;
    unsigned int l_totalInts = 0;
    unsigned int l_idx = 1;
    AlveoLink::kernel::HopCtrlPkt<AL_netDataBits, AL_destBits> l_ctrlPkt;
LOOP_TEST_RECDATA:
    while (!l_recDone) {
#pragma HLS PIPELINE II=1
        ap_uint<t_NumInts> l_intArr;
        l_recDone = !(l_totalInts < p_numTotalInts);
        if (!p_inStr.empty()){
            ap_uint<AL_netDataBits> l_val = p_inStr.read();
            uint32_t l_ints = 0;
            for (auto i=0; i<t_NumInts; ++i) {
                if (i==0) {
                    l_intArr[i] = 0;
                }
                else {
                    l_intArr[i] = (l_val((i+1)*32-1, i*32) !=0);
                }
            }
            for (auto i=0; i<t_NumInts; ++i) {
                l_ints = l_ints + l_intArr[i];
            }
            l_totalInts += l_ints;
            p_nHopDataRecPtr[l_idx] = l_val;
            l_idx++;
        }
    }
    ap_uint<AL_netDataBits> l_cnts = 0;
    l_cnts.range(31,0) = l_idx-1;
    p_nHopDataRecPtr[0] = l_cnts;

    l_ctrlPkt.setType(AlveoLink::kernel::PKT_TYPE::done);
    l_ctrlPkt.write(p_ctrlStr);

    while (!l_exit) {
#pragma HLS PIPELINE II=1
        l_ctrlPkt.read(p_inStr);
        if (l_ctrlPkt.isDoneWork()) {
            l_ctrlPkt.setType(AlveoLink::kernel::PKT_TYPE::terminate);
            l_ctrlPkt.write(p_ctrlStr);
            l_exit = true;
        }
        else if (l_ctrlPkt.isWorkload()) {
            l_ctrlPkt.setType(AlveoLink::kernel::PKT_TYPE::done);
            l_ctrlPkt.write(p_ctrlStr);
        }
    }
}

void transData(ap_uint<AL_netDataBits>* p_nHopDataSendPtr,
               hls::stream<ap_uint<AL_netDataBits> >& p_ctrlStr,
               hls::stream<ap_uint<AL_netDataBits> >& p_outStr,
               unsigned int p_myId,
               unsigned int p_destId,
               unsigned int p_numDevs,
               unsigned int p_numPkts,
               unsigned int p_batchPkts,
               unsigned int p_timeOutCycles) {

    AlveoLink::kernel::HopCtrlPkt<AL_netDataBits, AL_destBits> l_ctrlPkt;
    l_ctrlPkt.setSrcId(p_myId);
    l_ctrlPkt.setNumDevs(p_numDevs);
    l_ctrlPkt.setBatchPkts(p_batchPkts);
    l_ctrlPkt.setTimeOutCycles(p_timeOutCycles);
    l_ctrlPkt.setType(AlveoLink::kernel::PKT_TYPE::config);
    l_ctrlPkt.write(p_outStr);

    bool l_exit = false;
    unsigned int l_numPkts = 1;
    bool l_recDone = true;
    bool l_transDone = false;
LOOP_TEST_TRANSDATA:
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        l_transDone = (l_numPkts > p_numPkts);
        if (l_ctrlPkt.readNB(p_ctrlStr)) {
            if (l_ctrlPkt.isDoneWork()) {
                l_recDone = true;
            }
            else if (l_ctrlPkt.isTerminate()) {
                l_exit = true;
            }
        }
        else if (!l_transDone) {
            ap_uint<AL_netDataBits> l_val = p_nHopDataSendPtr[l_numPkts];
            p_outStr.write(l_val);
            l_numPkts++;
        }
        else if (l_recDone && l_transDone) {
            l_ctrlPkt.setType(AlveoLink::kernel::PKT_TYPE::done);
            l_ctrlPkt.setSrcId(p_myId);
            l_ctrlPkt.write(p_outStr);
            l_recDone = false;
        }
    }
    ap_uint<AL_netDataBits> l_cnts = 0;
    l_cnts.range(31,0) = l_numPkts-1;
    p_nHopDataSendPtr[0] = l_cnts;
}

extern "C" void krnl_testApp(ap_uint<AL_netDataBits>* p_nHopDataSendPtr,
                          ap_uint<AL_netDataBits>* p_nHopDataRecPtr,
                          hls::stream<ap_uint<AL_netDataBits> >& p_outStr,
                          hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
                          unsigned int p_myId,
                          unsigned int p_destId,
                          unsigned int p_numDevs,
                          unsigned int p_numPkts,
                          unsigned int p_batchPkts,
                          unsigned int p_timeOutCycles,
                          unsigned int p_numTotalInts) {     
    POINTER(p_nHopDataSendPtr, p_nHopDataSendPtr)
    POINTER(p_nHopDataRecPtr, p_nHopDataRecPtr)
    AXIS(p_outStr)
    AXIS(p_inStr)
    SCALAR(p_myId)
    SCALAR(p_destId)
    SCALAR(p_numDevs)
    SCALAR(p_numPkts)
    SCALAR(p_batchPkts)
    SCALAR(p_timeOutCycles)
    SCALAR(p_numTotalInts)
    SCALAR(return)

#pragma HLS DATAFLOW
    hls::stream<ap_uint<AL_netDataBits> > l_ctrlStr;
#pragma HLS STREAM variable=l_ctrlStr depth=16
    recData(p_nHopDataRecPtr, p_inStr, l_ctrlStr, p_numTotalInts);
    transData(p_nHopDataSendPtr, l_ctrlStr, p_outStr, p_myId, p_destId, p_numDevs, p_numPkts, p_batchPkts, p_timeOutCycles);
}
