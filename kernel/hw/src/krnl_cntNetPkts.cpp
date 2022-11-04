
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

extern "C" void krnl_cntNetPkts(int& p_numPkts,
                                hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
                                hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr) {
    AXIS(p_inStr);
    AXIS(p_outStr)
    SCALAR(p_numPkts)
#pragma HLS INTERFACE mode=ap_vld port=p_numPkts
    SCALAR(return)

    bool l_exit = false;
    int l_numPkts = 0;
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        ap_axiu<AL_netDataBits, 0, 0, AL_destBits> l_pkt = p_inStr.read();
        ap_uint<AL_netDataBits> l_val = l_pkt.data;
        p_outStr.write(l_pkt);
        l_numPkts++;
        l_exit = (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::terminate); 
    }
    p_numPkts = l_numPkts;
}
