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
#include "xnik.hpp"

typedef AlveoLink::adapter::PktUDP<AL_netDataBits, AL_destBits>::TypeAXIS PktType;
constexpr unsigned int t_AckPktBits = AL_destBits * 2;

AlveoLink::adapter::XNIK_TX<AL_maxConnections, AL_mtuBytes, AL_netDataBits, AL_destBits> l_xnikTX;
extern "C" void krnl_xnik_tx(hls::stream<PktType>& p_inKrnlStr,
             hls::stream<ap_uint<t_AckPktBits> >& p_inAckStr,
             hls::stream<PktType>& p_outPktStr) {
    AXIS(p_inKrnlStr)
    AXIS(p_inAckStr)
    AXIS(p_outPktStr)
    AP_CTRL_NONE(return)
    l_xnikTX.pktGen(p_inKrnlStr, p_inAckStr, p_outPktStr);
}
