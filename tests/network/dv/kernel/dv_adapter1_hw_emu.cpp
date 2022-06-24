
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

extern "C" void dv_adapter1(hls::stream<ap_axiu<128, 0, 0, 16> >& tx0_axis,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx1_axis,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx2_axis,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx3_axis,
#ifdef WITH_DVSWITCH
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx0_axis2Switch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx1_axis2Switch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx2_axis2Switch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& tx3_axis2Switch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx0_axisFromSwitch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx1_axisFromSwitch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx2_axisFromSwitch,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx3_axisFromSwitch, 
#endif
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx0_axis,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx1_axis,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx2_axis,
                              hls::stream<ap_axiu<128, 0, 0, 16> >& rx3_axis) { 
    // Connects the Rx and Tx AXI Streams
    AXIS(tx0_axis)
    AXIS(tx1_axis)
    AXIS(tx2_axis)
    AXIS(tx3_axis)
    AXIS(rx0_axis)
    AXIS(rx1_axis)
    AXIS(rx2_axis)
    AXIS(rx3_axis)
    
#ifdef WITH_DVSWITCH
    AXIS(tx0_axis2Switch)
    AXIS(tx1_axis2Switch)
    AXIS(tx2_axis2Switch)
    AXIS(tx3_axis2Switch)
    AXIS(rx0_axisFromSwitch)
    AXIS(rx1_axisFromSwitch)
    AXIS(rx2_axisFromSwitch)
    AXIS(rx3_axisFromSwitch)
#endif
    AP_CTRL_NONE(return)

    while (true) {
#pragma HLS PIPELINE II=1
        ap_axiu<128, 0, 0, 16> l_val0, l_val1, l_val2, l_val3;
#ifdef WITH_DVSWITCH
        if (tx0_axis.read_nb(l_val0)) {
            tx0_axis2Switch.write(l_val0);
        }
        if (tx1_axis.read_nb(l_val1)) {
            tx1_axis2Switch.write(l_val1);
        }
        if (tx2_axis.read_nb(l_val2)) {
            tx2_axis2Switch.write(l_val2);
        }
        if (tx3_axis.read_nb(l_val3)) {
            tx3_axis2Switch.write(l_val3);
        }
        
        ap_axiu<128, 0, 0, 16> l_val4, l_val5, l_val6, l_val7;
        if (rx0_axisFromSwitch.read_nb(l_val4)) {
            rx0_axis.write(l_val4);
        }
        if (rx1_axisFromSwitch.read_nb(l_val5)) {
            rx1_axis.write(l_val5);
        }
        if (rx2_axisFromSwitch.read_nb(l_val6)) {
            rx2_axis.write(l_val6);
        }
        if (rx3_axisFromSwitch.read_nb(l_val7)) {
            rx3_axis.write(l_val7);
        }
#else
        if (tx0_axis.read_nb(l_val0)) {
            rx0_axis.write(l_val0);
        }
        if (tx1_axis.read_nb(l_val1)) {
            rx1_axis.write(l_val1);
        }
        if (tx2_axis.read_nb(l_val2)) {
            rx2_axis.write(l_val2);
        }
        if (tx3_axis.read_nb(l_val3)) {
            rx3_axis.write(l_val3);
        }
#endif
    }
}
