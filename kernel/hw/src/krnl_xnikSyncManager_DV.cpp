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

extern "C" void krnl_xnikSyncManager(uint16_t* p_config,
                                 hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_inStr,
                                 hls::stream<ap_axiu<AL_netDataBits, 0, 0, AL_destBits> >& p_outStr) {
    POINTER(p_config, p_config)
    AXIS(p_inStr)
    AXIS(p_outStr)
    SCALAR(return)

    AlveoLink::kernel::xnikSync_Manager<AL_netDataBits, AL_destBits> l_xnikSyncManager;
    uint16_t l_numDevs = p_config[0];
    uint16_t l_waitCycles = p_config[1];
    uint16_t l_flushCounter = p_config[2];
#pragma HLS DATAFLOW
    hls::stream<ap_uint<AL_netDataBits> > l_inNetStr;
    hls::stream<ap_uint<AL_netDataBits> > l_outNetStr;

    AlveoLink::kernel::readAxis<AL_netDataBits, AL_destBits>(l_numDevs, p_inStr, l_inNetStr);
    l_xnikSyncManager.process(l_numDevs, l_waitCycles, l_flushCounter, l_inNetStr, l_outNetStr);
    AlveoLink::kernel::writeAxis<AL_netDataBits, AL_destBits>(l_numDevs, l_outNetStr, p_outStr);
}
