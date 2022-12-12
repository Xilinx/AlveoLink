
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

extern "C" void krnl_xnikSyncTX(hls::stream<ap_uint<AL_netDataBits> >& p_nhop2xnikStr,
                                 hls::stream<ap_uint<AL_netDataBits> >& p_rxStr,
                                 hls::stream<ap_uint<AL_netDataBits> >& p_txStr,
                                 hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr) {
    AXIS(p_nhop2xnikStr)
    AXIS(p_rxStr)
    AXIS(p_txStr)
    AXIS(p_outStr)
    AP_CTRL_NONE(return)

    AlveoLink::kernel::xnikSync_TX<AL_netDataBits, AL_destBits> l_xnikSyncTX;
    hls::stream<ap_uint<AL_netDataBits> > l_netStr;
#pragma HLS STREAM variable = l_netStr depth=16
    hls::stream<ap_uint<AL_netDataBits> > l_rxStr;
    hls::stream<ap_uint<AL_netDataBits> > l_nhop2xnikStr;
#pragma HLS STREAM variable = l_rxStr depth=16
#pragma HLS STREAM variable = l_nhop2xnikStr depth=16
#pragma HLS DATAFLOW
    AlveoLink::kernel::fwdKrnl2TxStr<AL_netDataBits>(p_nhop2xnikStr, p_rxStr, l_nhop2xnikStr, l_rxStr);
    l_xnikSyncTX.process(l_nhop2xnikStr,l_rxStr,p_txStr,l_netStr);
    AlveoLink::kernel::writeAxis<AL_netDataBits, AL_destBits>(l_netStr, p_outStr);
}