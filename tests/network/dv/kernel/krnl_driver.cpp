
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
#include <cstdint>
#include "interface.hpp"
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

void transData(unsigned int p_numWords,
               ap_uint<AL_netDataBits>* p_dataPtr,
               hls::stream<ap_uint<AL_netDataBits> >& p_outStr,
               hls::stream<ap_uint<1> >& p_ctrlStr) {
    for (auto i=0; i<p_numWords; ++i) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_data = p_dataPtr[i];
        if (i==0) {
            p_ctrlStr.write(1);
        }
        p_outStr.write(l_data);
    }
}

void recData(unsigned int p_numTotalInts,
             hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
             ap_uint<AL_netDataBits>* p_recDataPtr,
             hls::stream<ap_uint<1> >& p_ctrlStr) {
    const static unsigned int t_NumInts = (AL_netDataBits/sizeof(uint32_t));
    bool l_exit = false;
    uint32_t l_totalInts = 0;
    uint32_t l_idx=1;
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        ap_uint<t_NumInts> l_intArr;
        ap_uint<AL_netDataBits> l_val;
        uint32_t l_ints = 0;
        l_val = p_inStr.read();
        if (l_idx==1) {
            p_ctrlStr.write(1);
        }
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
        p_recDataPtr[l_idx] = l_val;
        l_idx++;
        l_exit = !(l_totalInts < p_numTotalInts);
    }
    p_ctrlStr.write(1);
    p_recDataPtr[0] = l_idx-1;
}

void calcStats(hls::stream<ap_uint<1> >& p_sendCtrlStr,
               hls::stream<ap_uint<1> >& p_recCtrlStr,
               ap_uint<32>* p_stats) {

    ap_uint<32> l_latCycles = 0;
    ap_uint<32> l_totalCycles = 0;
    ap_uint<1> l_sendCtrl = p_sendCtrlStr.read();
    ap_uint<1> l_recCtrl = ~l_sendCtrl;
    while (l_recCtrl == 0) {
#pragma HLS PIPELINE II=1
        l_latCycles++;
        bool l_unused = p_recCtrlStr.read_nb(l_recCtrl);
    }
    ap_uint<1> l_pktCtrl = 0;
    while (l_pktCtrl == 0) {
#pragma HLS PIPELINE II=1
        l_totalCycles++;
        bool l_unused = p_recCtrlStr.read_nb(l_pktCtrl);
    }
    p_stats[0] = l_latCycles;
    p_stats[1] = l_totalCycles + l_latCycles; 
}                      

extern "C" void krnl_driver(unsigned int p_numWords,
                          ap_uint<AL_netDataBits>* p_dataPtr,
                          hls::stream<ap_uint<AL_netDataBits> >& p_outStr,
                          hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
                          ap_uint<AL_netDataBits>* p_recDataPtr,
                          ap_uint<32>* p_statsPtr,
                          unsigned int p_numTotalInts) { 
    POINTER(p_dataPtr, p_dataPtr)
    POINTER(p_recDataPtr, p_recDataPtr)
    POINTER(p_statsPtr, p_statsPtr)
    AXIS(p_outStr)
    AXIS(p_inStr)
    SCALAR(p_numWords)
    SCALAR(p_numTotalInts)
    SCALAR(return)

    hls::stream<ap_uint<1> > l_sendCtrlStr, l_recCtrlStr;
#pragma HLS stream variable = l_sendCtrlStr depth = 4
#pragma HLS stream variable = l_recCtrlStr depth = 4

#pragma HLS DATAFLOW

    transData(p_numWords, p_dataPtr, p_outStr, l_sendCtrlStr);
    recData(p_numTotalInts, p_inStr, p_recDataPtr, l_recCtrlStr);
    calcStats(l_sendCtrlStr, l_recCtrlStr, p_statsPtr);
}
