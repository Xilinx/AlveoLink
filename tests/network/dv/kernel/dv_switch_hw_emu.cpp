
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

void splitAxis2Str(hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis,
                  hls::stream<ap_uint<128> > p_outStr[4]) {
    while (true) {
#pragma HLS PIPELINE II=1
        if (!p_axis.empty()) {
            ap_axiu<128, 0, 0, 16> l_inVal = p_axis.read();
            ap_uint<2> l_dest = l_inVal.dest;
            p_outStr[l_dest].write(l_inVal.data);
        }
    }
}

void mergeStr(hls::stream<ap_uint<128> > p_inStr[4][4],
              hls::stream<ap_uint<144> > p_outStr[4]) {
    while (true) {
#pragma HLS PIPELINE II=1
        for (auto b=0; b<4; ++b) {
            ap_uint<2> l_src = 0;
            for (auto n=0; n<4; ++n) {
                if (!p_inStr[n][b].empty()) {
                    l_src = n;
                    break;
                } 
            }
            
            ap_uint<128> l_val;
            ap_uint<144> l_outVal;
            if (p_inStr[l_src][b].read_nb(l_val)) {
                l_outVal(143,16) = l_val;
                l_outVal(15,0) = l_src;
                p_outStr[b].write(l_outVal);
            }
        }
    }
}

void strArr2Axis(hls::stream<ap_uint<144> > p_inStr[4],
                 hls::stream<ap_axiu<128, 0, 0, 16> >& rx0_axis, 
                 hls::stream<ap_axiu<128, 0, 0, 16> >& rx1_axis, 
                 hls::stream<ap_axiu<128, 0, 0, 16> >& rx2_axis, 
                 hls::stream<ap_axiu<128, 0, 0, 16> >& rx3_axis) {
    while (true) {
#pragma HLS PIPELINE II=1
        ap_uint<144> l_val0, l_val1, l_val2, l_val3;
        ap_axiu<128, 0, 0, 16> l_outVal0, l_outVal1, l_outVal2, l_outVal3;
        if (p_inStr[0].read_nb(l_val0)) {
            l_outVal0.data = l_val0(143,16);
            l_outVal0.dest = l_val0(15,0);
            rx0_axis.write(l_outVal0);
        }
        if (p_inStr[1].read_nb(l_val1)) {
            l_outVal1.data = l_val1(143,16);
            l_outVal1.dest = l_val1(15,0);
            rx1_axis.write(l_outVal1);
        }
        if (p_inStr[2].read_nb(l_val2)) {
            l_outVal2.data = l_val2(143,16);
            l_outVal2.dest = l_val2(15,0);
            rx2_axis.write(l_outVal2);
        }
        if (p_inStr[3].read_nb(l_val3)) {
            l_outVal3.data = l_val3(143,16);
            l_outVal3.dest = l_val3(15,0);
            rx3_axis.write(l_outVal3);
        }
    }
}


extern "C" void dv_switch(hls::stream<ap_axiu<128, 0, 0, 16> >& tx00_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx01_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx02_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx03_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx10_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx11_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx12_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx13_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx20_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx21_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx22_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx23_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx30_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx31_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx32_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& tx33_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx00_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx01_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx02_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx03_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx10_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx11_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx12_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx13_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx20_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx21_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx22_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx23_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx30_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx31_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx32_axis,
                                  hls::stream<ap_axiu<128, 0, 0, 16> >& rx33_axis) { 
    // Connects the Rx and Tx AXI Streams
    AXIS(tx00_axis)
    AXIS(tx01_axis)
    AXIS(tx02_axis)
    AXIS(tx03_axis)
    AXIS(rx00_axis)
    AXIS(rx01_axis)
    AXIS(rx02_axis)
    AXIS(rx03_axis)

    AXIS(tx10_axis)
    AXIS(tx11_axis)
    AXIS(tx12_axis)
    AXIS(tx13_axis)
    AXIS(rx10_axis)
    AXIS(rx11_axis)
    AXIS(rx12_axis)
    AXIS(rx13_axis)

    AXIS(tx20_axis)
    AXIS(tx21_axis)
    AXIS(tx22_axis)
    AXIS(tx23_axis)
    AXIS(rx20_axis)
    AXIS(rx21_axis)
    AXIS(rx22_axis)
    AXIS(rx23_axis)

    AXIS(tx30_axis)
    AXIS(tx31_axis)
    AXIS(tx32_axis)
    AXIS(tx33_axis)
    AXIS(rx30_axis)
    AXIS(rx31_axis)
    AXIS(rx32_axis)
    AXIS(rx33_axis)

    AP_CTRL_NONE(return)
#pragma HLS DATAFLOW
    hls::stream<ap_uint<128> > l_strSplit0[4][4];
    hls::stream<ap_uint<128> > l_strSplit1[4][4];
    hls::stream<ap_uint<128> > l_strSplit2[4][4];
    hls::stream<ap_uint<128> > l_strSplit3[4][4];
#pragma HLS STREAM variable=l_strSplit0 depth = 4
#pragma HLS STREAM variable=l_strSplit1 depth = 4
#pragma HLS STREAM variable=l_strSplit2 depth = 4
#pragma HLS STREAM variable=l_strSplit3 depth = 4
    hls::stream<ap_uint<144> > l_strMerge0[4];
    hls::stream<ap_uint<144> > l_strMerge1[4];
    hls::stream<ap_uint<144> > l_strMerge2[4];
    hls::stream<ap_uint<144> > l_strMerge3[4];
#pragma HLS STREAM variable=l_strMerge0 depth = 4
#pragma HLS STREAM variable=l_strMerge1 depth = 4
#pragma HLS STREAM variable=l_strMerge2 depth = 4
#pragma HLS STREAM variable=l_strMerge3 depth = 4
    
    splitAxis2Str(tx00_axis,l_strSplit0[0]);
    splitAxis2Str(tx10_axis,l_strSplit0[1]);
    splitAxis2Str(tx20_axis,l_strSplit0[2]);
    splitAxis2Str(tx30_axis,l_strSplit0[3]);
    mergeStr(l_strSplit0,l_strMerge0); 
    strArr2Axis(l_strMerge0,rx00_axis,rx10_axis,rx20_axis, rx30_axis);

    splitAxis2Str(tx01_axis,l_strSplit1[0]);
    splitAxis2Str(tx11_axis,l_strSplit1[1]);
    splitAxis2Str(tx21_axis,l_strSplit1[2]);
    splitAxis2Str(tx31_axis,l_strSplit1[3]);
    mergeStr(l_strSplit1,l_strMerge1); 
    strArr2Axis(l_strMerge1,rx01_axis,rx11_axis,rx21_axis, rx31_axis);

    splitAxis2Str(tx02_axis,l_strSplit2[0]);
    splitAxis2Str(tx12_axis,l_strSplit2[1]);
    splitAxis2Str(tx22_axis,l_strSplit2[2]);
    splitAxis2Str(tx32_axis,l_strSplit2[3]);
    mergeStr(l_strSplit2,l_strMerge2); 
    strArr2Axis(l_strMerge2,rx02_axis,rx12_axis,rx22_axis, rx32_axis);

    splitAxis2Str(tx03_axis,l_strSplit3[0]);
    splitAxis2Str(tx13_axis,l_strSplit3[1]);
    splitAxis2Str(tx23_axis,l_strSplit3[2]);
    splitAxis2Str(tx33_axis,l_strSplit3[3]);
    mergeStr(l_strSplit3,l_strMerge3); 
    strArr2Axis(l_strMerge3,rx03_axis,rx13_axis,rx23_axis, rx33_axis);

}
