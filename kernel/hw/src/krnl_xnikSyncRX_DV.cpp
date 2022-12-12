
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



extern "C" void krnl_xnikSyncRX(hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
                                 hls::stream<ap_uint<AL_netDataBits> >& p_txStr,
                                 hls::stream<ap_uint<AL_netDataBits> >& p_rxStr,
                                 hls::stream<ap_uint<AL_netDataBits> >& p_xnik2nhopStr) {
    AXIS(p_inStr)
    AXIS(p_txStr)
    AXIS(p_rxStr)
    AXIS(p_xnik2nhopStr)
    AP_CTRL_NONE(return)

    AlveoLink::kernel::xnikSync_RX<AL_netDataBits, AL_destBits> l_xnikSyncRX;
    hls::stream<ap_uint<AL_netDataBits> > l_netStr;
    hls::stream<ap_uint<AL_netDataBits> > l_net2xnikStr;
    hls::stream<ap_uint<AL_netDataBits> > l_txStr;
#pragma HLS STREAM variable = l_netStr depth=16
#pragma HLS STREAM variable = l_net2xnikStr depth=16
#pragma HLS STREAM variable = l_txStr depth=16
    hls::stream<ap_uint<AL_netDataBits> > l_toKernelStr;
#pragma HLS stream variable = l_toKernelStr depth=4096
//#pragma HLS stream variable = l_toKernelStr depth=16
#pragma HLS bind_storage variable = l_toKernelStr type = ram_2p impl = uram
#pragma HLS DATAFLOW
    AlveoLink::kernel::readAxis<AL_netDataBits, AL_destBits>(p_inStr, l_netStr);
    AlveoLink::kernel::fwdNet2RxStr<AL_netDataBits>(l_netStr, p_txStr, l_net2xnikStr, l_txStr);
    l_xnikSyncRX.process(l_net2xnikStr, l_txStr, p_rxStr, l_toKernelStr);
    AlveoLink::kernel::fwdStr<AL_netDataBits>(l_toKernelStr, p_xnik2nhopStr);
}