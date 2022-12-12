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
#include "xnikSyncDV.hpp"

extern "C" void krnl_cntUsrPkts(int& p_numPkts,
                                hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
                                hls::stream<ap_uint<AL_netDataBits> >& p_outStr,
                                hls::stream<ap_uint<1> >& p_ctrlStr) {
    AXIS(p_inStr)
    AXIS(p_outStr)
    AXIS(p_ctrlStr)
    SCALAR(p_numPkts)
#pragma HLS INTERFACE mode=ap_vld port=p_numPkts
    SCALAR(return)

    bool l_exit = false;
    int l_numPkts = 0;
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_val = p_inStr.read();
        p_outStr.write(l_val);
        l_numPkts++;
        l_exit = (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::done); 
    }
    p_ctrlStr.write(1);
    p_numPkts = l_numPkts;

}
