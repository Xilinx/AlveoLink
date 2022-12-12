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

extern "C" void krnl_cntUsrTxPkts(uint32_t* p_numPktsPtr,
                                hls::stream<ap_uint<1> >& p_ctrlStr,
                                hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
                                hls::stream<ap_uint<AL_netDataBits> >& p_outStr) {
    AXIS(p_inStr)
    AXIS(p_outStr)
    AXIS(p_ctrlStr)
    POINTER(p_numPktsPtr, p_numPktsPtr)
    SCALAR(return)

    bool l_exit = false;
    uint32_t l_numDonePkts = 0;
    uint32_t l_numWorkLoadPkts = 0;
    uint32_t l_totalPkts = 0;
    while (!l_exit) {
#pragma HLS PIPELINE II=1
        ap_uint<AL_netDataBits> l_val;
        if (p_inStr.read_nb(l_val)) {
            p_outStr.write(l_val);
            l_totalPkts++;
            if (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::done) {
                l_numDonePkts++;
            }
            if (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::workload) {
                l_numWorkLoadPkts++;
            }
        }
        ap_uint<1> l_tmp;
        l_exit = p_ctrlStr.read_nb(l_tmp); 
    }
    p_numPktsPtr[0] = l_totalPkts;
    p_numPktsPtr[1] = l_numWorkLoadPkts;
    p_numPktsPtr[2] = l_numDonePkts;
}
