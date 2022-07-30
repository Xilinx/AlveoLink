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
#include "xnikTmDV.hpp"

extern "C" void krnl_xnikTM(uint32_t* p_config,
                            ap_uint<AL_netDataBits>* p_memPtr,
                            hls::stream<ap_uint<AL_netDataBits> >& p_inStr,
                            hls::stream<ap_uint<AL_netDataBits> >& p_outStr) {
    POINTER(p_config, p_config)
    POINTER(p_memPtr, p_memPtr)
    AXIS(p_inStr)
    AXIS(p_outStr)
    SCALAR(return)

    AlveoLink::kernel::xnikTraffic_Manager<AL_netDataBits, AL_destBits> l_tm;

    uint32_t l_myId = p_config[0];
    uint32_t l_tmId = p_config[1];
    uint32_t l_numDevs = p_config[2];
    uint32_t l_waitCycles = p_config[3];
    uint32_t l_maxAddr = p_config[4];

    l_tm.process(l_myId, l_tmId, l_numDevs, l_waitCycles, l_maxAddr, p_memPtr, p_inStr, p_outStr);
}
