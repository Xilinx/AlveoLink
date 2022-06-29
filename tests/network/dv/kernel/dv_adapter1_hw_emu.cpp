
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

void fwdAxis(hls::stream<ap_axiu<128, 0, 0, 16> >& p_inAxis,
             hls::stream<ap_axiu<128, 0, 0, 16> >& p_outAxis) {
    while (true) {
#pragma HLS PIPELINE II=1
        if (!p_inAxis.empty()) {
            ap_axiu<128, 0, 0, 16> l_val = p_inAxis.read();
            p_outAxis.write(l_val);
        }
    }
}

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
#pragma HLS DATAFLOW

#ifdef WITH_DVSWITCH
        fwdAxis(tx0_axis,tx0_axis2Switch);
        fwdAxis(tx1_axis,tx1_axis2Switch);
        fwdAxis(tx2_axis,tx2_axis2Switch);
        fwdAxis(tx3_axis,tx3_axis2Switch);

        fwdAxis(rx0_axisFromSwitch,rx0_axis); 
        fwdAxis(rx1_axisFromSwitch,rx1_axis); 
        fwdAxis(rx2_axisFromSwitch,rx2_axis); 
        fwdAxis(rx3_axisFromSwitch,rx3_axis);
 
#else
        fwdAxis(tx0_axis,rx0_axis);
        fwdAxis(tx1_axis,rx1_axis);
        fwdAxis(tx2_axis,rx2_axis);
        fwdAxis(tx3_axis,rx3_axis);
#endif
}
