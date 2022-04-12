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
#include <cstdint>
#include "interface.hpp"
#include "xnikDefs.hpp"

typedef AlveoLink::adapter::PktUDP<AL_netDataBits, AL_destBits>::TypeAXIS UdpPktType;

extern "C" void krnl_switch(hls::stream<UdpPktType>& p_inStr,
                                hls::stream<UdpPktType>& p_out2ComputeStr,
                                hls::stream<UdpPktType>& p_out2ManagerStr) {
    AXIS(p_inStr)
    AXIS(p_out2ComputeStr)
    AXIS(p_out2ManagerStr)
    AP_CTRL_NONE(return)

    while (!p_inStr.empty()) {
#pragma HLS PIPELINE II=1
        UdpPktType l_val = p_inStr.read();
        if (l_val.dest == 1) {
            p_out2ManagerStr.write(l_val);
        }
        else {
            p_out2ComputeStr.write(l_val);
        }
    }
}
