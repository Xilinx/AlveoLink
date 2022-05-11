
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
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

constexpr unsigned int t_PLRAM_Size = 128 * 1024;
void transData(unsigned int p_numWords,
               ap_uint<AL_netDataBits>* p_dataPtr,
               ap_uint<AL_destBits>* p_destPtr,
               hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr,
               hls::stream<ap_uint<1> >& p_ctrlStr) {
    p_ctrlStr.write(1);
    for (auto i=0; i<p_numWords; ++i) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_data = p_dataPtr[i];
        ap_uint<AL_destBits> l_dest = p_destPtr[i % t_PLRAM_Size];
        ap_axiu<AL_netDataBits, 0, 0, AL_destBits> l_val;
        l_val.data = l_data;
        l_val.dest = l_dest;
        p_outStr.write(l_val);
    }
}

void recData(unsigned int p_numWords,
             hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
             ap_uint<AL_netDataBits>* p_recDataPtr,
             hls::stream<ap_uint<1> >& p_ctrlStr) {
    for (auto i=0; i<p_numWords; ++i) {
#pragma HLS PIPELINE II=1
        ap_axiu<AL_netDataBits, 0, 0, AL_destBits> l_val;
        l_val = p_inStr.read();
        if (i==0) {
            p_ctrlStr.write(1);
        }
        p_recDataPtr[i] = l_val.data;
    }
     p_ctrlStr.write(1);
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
        l_totalCycles++;
        bool l_unused = p_recCtrlStr.read_nb(l_recCtrl);
    }
    l_recCtrl = 0;
    while (l_recCtrl == 0) {
#pragma HLS PIPELINE II=1
        l_totalCycles++;
        bool l_unused = p_recCtrlStr.read_nb(l_recCtrl);
    }
    p_stats[0] = l_latCycles;
    p_stats[1] = l_totalCycles; 
}                      

extern "C" void krnl_driver(unsigned int p_numWords,
                          ap_uint<AL_netDataBits>* p_dataPtr,
                          ap_uint<AL_destBits>* p_destPtr,
                          hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr,
                          hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
                          ap_uint<AL_netDataBits>* p_recDataPtr,
                          ap_uint<32>* p_statsPtr) { 
    POINTER(p_dataPtr, p_dataPtr)
    POINTER(p_destPtr, p_destPtr)
    POINTER(p_recDataPtr, p_recDataPtr)
    POINTER(p_statsPtr, p_statsPtr)
    AXIS(p_outStr)
    AXIS(p_inStr)
    SCALAR(p_numWords)
    SCALAR(return)

    hls::stream<ap_uint<1> > l_sendCtrlStr, l_recCtrlStr;
#pragma HLS stream variable = l_sendCtrlStr depth = 4
#pragma HLS stream variable = l_recCtrlStr depth = 4

#pragma HLS DATAFLOW

    transData(p_numWords, p_dataPtr, p_destPtr, p_outStr, l_sendCtrlStr);
    recData(p_numWords, p_inStr, p_recDataPtr, l_recCtrlStr);
    calcStats(l_sendCtrlStr, l_recCtrlStr, p_statsPtr);
}
