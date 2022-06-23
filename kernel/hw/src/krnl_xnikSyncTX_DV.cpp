
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
                                 hls::stream<ap_uint<AL_netDataBits> >& p_outStr,
                                 uint32_t* p_stats) {
    AXIS(p_nhop2xnikStr)
    AXIS(p_rxStr)
    AXIS(p_txStr)
    AXIS(p_outStr)
    POINTER(p_stats, p_stats)
    SCALAR(return)

    AlveoLink::kernel::xnikSync_TX<AL_netDataBits, AL_destBits> l_xnikSyncTX;
    l_xnikSyncTX.process(p_nhop2xnikStr,p_rxStr,p_txStr,p_outStr,p_stats);
}
