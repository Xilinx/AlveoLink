
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

void splitStr2Axis(hls::stream<ap_uint<160> >& p_inStr,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis0,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis1,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis2,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis3) {
    while (true) {
#pragma HLS PIPELINE II=1
        ap_uint<160> l_inVal; // bit 15-0: dest, bit 31-16: src, bit 32-159: data
        ap_axiu<128, 0, 0, 16>  l_outVal;
        if (p_inStr.read_nb(l_inVal)) {
            ap_uint<16> l_dest = l_inVal(15,0);
            l_outVal.dest = l_inVal(31,16);
            l_outVal.data = l_inVal(159, 32);
            if (l_dest == 0) {
                p_axis0.write(l_outVal);
            }
            if (l_dest == 1) {
                p_axis1.write(l_outVal);
            }
            if (l_dest == 2) {
                p_axis2.write(l_outVal);
            }
            if (l_dest == 3) {
                p_axis3.write(l_outVal);
            }
        }
    }
}

void mergeAxis2Str(hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis0,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis1,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis2,
               hls::stream<ap_axiu<128, 0, 0, 16> >& p_axis3,
               hls::stream<ap_uint<160> >& p_outStr) {
    ap_uint<4> l_ctrl = 1;
    while (true) {
#pragma HLS PIPELINE II=1
        ap_axiu<128, 0, 0, 16> l_inVal;
        ap_uint<160> l_outVal;
        if (l_ctrl[0] && p_axis0.read_nb(l_inVal)) {
            l_outVal(15,0) = l_inVal.dest;
            l_outVal(31,16) = 0;
            l_outVal(159,32) = l_inVal.data;
            p_outStr.write(l_outVal);
        }
        else if (l_ctrl[1] && p_axis1.read_nb(l_inVal)) {
            l_outVal(15,0) = l_inVal.dest;
            l_outVal(31,16) = 1;
            l_outVal(159,32) = l_inVal.data;
            p_outStr.write(l_outVal);
        }
        else if (l_ctrl[2] && p_axis2.read_nb(l_inVal)) {
            l_outVal(15,0) = l_inVal.dest;
            l_outVal(31,16) = 2;
            l_outVal(159,32) = l_inVal.data;
            p_outStr.write(l_outVal);
        }
        else if (l_ctrl[3] && p_axis3.read_nb(l_inVal)) {
            l_outVal(15,0) = l_inVal.dest;
            l_outVal(31,16) = 3;
            l_outVal(159,32) = l_inVal.data;
            p_outStr.write(l_outVal);
        }

        if (l_ctrl[3]) {
            l_ctrl = 1;
        }
        else {
            l_ctrl = l_ctrl << 1;
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
    hls::stream<ap_uint<160> > l_str0, l_str1, l_str2, l_str3;
    mergeAxis2Str(tx00_axis, tx10_axis, tx20_axis, tx30_axis, l_str0);
    splitStr2Axis(l_str0, rx00_axis, rx10_axis, rx20_axis, rx30_axis);

    mergeAxis2Str(tx01_axis, tx11_axis, tx21_axis, tx31_axis, l_str1);
    splitStr2Axis(l_str1, rx01_axis, rx11_axis, rx21_axis, rx31_axis);

    mergeAxis2Str(tx02_axis, tx12_axis, tx22_axis, tx32_axis, l_str2);
    splitStr2Axis(l_str2, rx02_axis, rx12_axis, rx22_axis, rx32_axis);

    mergeAxis2Str(tx03_axis, tx13_axis, tx23_axis, tx33_axis, l_str3);
    splitStr2Axis(l_str3, rx03_axis, rx13_axis, rx23_axis, rx33_axis);

}
