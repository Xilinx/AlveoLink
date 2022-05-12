
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

extern "C" void krnl_echo(hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
                          hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr) { 
    // Connects the Rx and Tx AXI Streams
    AXIS(p_inStr)
    AXIS(p_outStr)

    AP_CTRL_NONE(return)

    while (true) {
#pragma HLS PIPELINE II = 1
        ap_axiu<AL_netDataBits, 0, 0, AL_destBits> l_val = p_inStr.read(); 
        p_outStr.write(l_val);
    }

}
