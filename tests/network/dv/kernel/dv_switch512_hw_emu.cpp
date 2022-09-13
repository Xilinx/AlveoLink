
/*
 * Copyright 2019-2021 Xilinx, Inc.
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

void splitAxis2Str(hls::stream<ap_axiu<512, 0, 0, 16> >& p_axis,
                  hls::stream<ap_uint<512> > p_outStr[4]) {
    while (true) {
#pragma HLS pipeline style=frp
#pragma HLS PIPELINE II=1
        if (!p_axis.empty()) {
            ap_axiu<512, 0, 0, 16> l_inVal = p_axis.read();
            ap_uint<2> l_dest = l_inVal.dest;
            p_outStr[l_dest].write(l_inVal.data);
        }
    }
}

void mergeStr(hls::stream<ap_uint<512> > p_inStr[4][4],
              hls::stream<ap_uint<528> > p_outStr[4]) {
    while (true) {
#pragma HLS pipeline style=frp
#pragma HLS PIPELINE II=1
        for (auto b=0; b<4; ++b) {
            ap_uint<2> l_src = 0;
            for (auto n=0; n<4; ++n) {
                if (!p_inStr[n][b].empty()) {
                    l_src = n;
                    break;
                } 
            }
            
            ap_uint<512> l_val;
            ap_uint<528> l_outVal;
            if (p_inStr[l_src][b].read_nb(l_val)) {
                l_outVal(527,16) = l_val;
                l_outVal(15,0) = l_src;
                p_outStr[b].write(l_outVal);
            }
        }
    }
}

void strArr2Axis(hls::stream<ap_uint<528> > p_inStr[4],
                 hls::stream<ap_axiu<512, 0, 0, 16> >& rx0_axis, 
                 hls::stream<ap_axiu<512, 0, 0, 16> >& rx1_axis, 
                 hls::stream<ap_axiu<512, 0, 0, 16> >& rx2_axis, 
                 hls::stream<ap_axiu<512, 0, 0, 16> >& rx3_axis) {
    while (true) {
#pragma HLS pipeline style=frp
#pragma HLS PIPELINE II=1
        ap_uint<528> l_val0, l_val1, l_val2, l_val3;
        ap_axiu<512, 0, 0, 16> l_outVal0, l_outVal1, l_outVal2, l_outVal3;
        if (p_inStr[0].read_nb(l_val0)) {
            l_outVal0.data = l_val0(527,16);
            l_outVal0.dest = l_val0(15,0);
            rx0_axis.write(l_outVal0);
        }
        if (p_inStr[1].read_nb(l_val1)) {
            l_outVal1.data = l_val1(527,16);
            l_outVal1.dest = l_val1(15,0);
            rx1_axis.write(l_outVal1);
        }
        if (p_inStr[2].read_nb(l_val2)) {
            l_outVal2.data = l_val2(527,16);
            l_outVal2.dest = l_val2(15,0);
            rx2_axis.write(l_outVal2);
        }
        if (p_inStr[3].read_nb(l_val3)) {
            l_outVal3.data = l_val3(527,16);
            l_outVal3.dest = l_val3(15,0);
            rx3_axis.write(l_outVal3);
        }
    }
}


extern "C" void dv_switch(hls::stream<ap_axiu<512, 0, 0, 16> >& tx0_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& tx1_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& tx2_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& tx3_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& rx0_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& rx1_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& rx2_axis,
                                  hls::stream<ap_axiu<512, 0, 0, 16> >& rx3_axis) { 
    // Connects the Rx and Tx AXI Streams
    AXIS(tx0_axis)
    AXIS(rx0_axis)

    AXIS(tx1_axis)
    AXIS(rx1_axis)

    AXIS(tx2_axis)
    AXIS(rx2_axis)

    AXIS(tx3_axis)
    AXIS(rx3_axis)

    AP_CTRL_NONE(return)
#pragma HLS DATAFLOW
    hls::stream<ap_uint<512> > l_strSplit[4][4];
#pragma HLS STREAM variable=l_strSplit depth = 4
    hls::stream<ap_uint<528> > l_strMerge[4];
#pragma HLS STREAM variable=l_strMerge depth = 4
    
    splitAxis2Str(tx0_axis,l_strSplit[0]);
    splitAxis2Str(tx1_axis,l_strSplit[1]);
    splitAxis2Str(tx2_axis,l_strSplit[2]);
    splitAxis2Str(tx3_axis,l_strSplit[3]);
    mergeStr(l_strSplit,l_strMerge); 
    strArr2Axis(l_strMerge,rx0_axis,rx1_axis,rx2_axis, rx3_axis);
}
