
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

void transData(unsigned int p_numWords,
               ap_uint<AL_netDataBits>* p_dataPtr,
               ap_uint<AL_destBits>* p_destPtr,
               hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr) {
    for (auto i=0; i<p_numWords; ++i) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_data = p_dataPtr[i];
        ap_uint<AL_destBits> l_dest = p_destPtr[i];
        ap_axiu<AL_netDataBits, 0, 0, AL_destBits> l_val;
        l_val.data = l_data;
        l_val.dest = l_dest;
        p_outStr.write(l_val);
    }
}

void recData(unsigned int p_numWords,
             hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
             ap_uint<AL_netDataBits>* p_recDataPtr,
             ap_uint<AL_destBits>* p_recDestPtr) {
    for (auto i=0; i<p_numWords; ++i) {
#pragma HLS PIPELINE II=1
        ap_axiu<AL_netDataBits, 0, 0, AL_destBits> l_val;
        l_val = p_inStr.read();
        p_recDataPtr[i] = l_val.data;
        p_recDestPtr[i] = l_val.dest;
    }
}
                        

extern "C" void krnl_driver(unsigned int p_numWords,
                          ap_uint<AL_netDataBits>* p_dataPtr,
                          ap_uint<AL_destBits>* p_destPtr,
                          hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr,
                          hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
                          ap_uint<AL_netDataBits>* p_recDataPtr,
                          ap_uint<AL_destBits>* p_recDestPtr) { 
    POINTER(p_dataPtr, p_dataPtr)
    POINTER(p_destPtr, p_destPtr)
    POINTER(p_recDataPtr, p_recDataPtr)
    POINTER(p_recDestPtr, p_recDestPtr)
    AXIS(p_outStr)
    AXIS(p_inStr)
    SCALAR(p_numWords)
    SCALAR(return)

#pragma HLS DATAFLOW
    transData(p_numWords, p_dataPtr, p_destPtr, p_outStr);
    recData(p_numWords, p_inStr, p_recDataPtr, p_recDestPtr);
}
