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

#define DropCtrl 1 
#define DropData 0
#define DropAck 0 

typedef AlveoLink::adapter::PktUDP<AL_netDataBits, AL_destBits>::TypeAXIS UdpPktType;

static bool l_drop = true;
static unsigned int l_dropTimes = 0;
extern "C" void krnl_pktDropper_0(hls::stream<UdpPktType>& p_inStr,
                                hls::stream<UdpPktType>& p_outStr) {
    AXIS(p_inStr)
    AXIS(p_outStr)
    AP_CTRL_NONE(return)
    uint16_t t_startSeq = 16;
    uint16_t t_dropPkts = 3;

    while (!p_inStr.empty()) {
#pragma HLS PIPELINE II=1
        AlveoLink::adapter::PktXNIK<AL_netDataBits, AL_destBits> l_xnikPkt;
        l_xnikPkt.read(p_inStr);
        ap_uint<8> l_seqNo = l_xnikPkt.getSeqNo();
#if DropData
        if (!l_drop || !l_xnikPkt.isData() || !l_xnikPkt.isWorkload() || (l_seqNo < t_startSeq) || (l_seqNo >= (t_startSeq+t_dropPkts))) {
            l_xnikPkt.write(p_outStr);
        }
        else if (l_drop && l_xnikPkt.isData() && l_xnikPkt.isWorkload() && (l_seqNo == (t_startSeq+t_dropPkts-1))) {
            //drop last data pkts and switch off l_drop
            l_drop = false;
        }
#endif
#if DropCtrl
        if (!l_xnikPkt.isData() || l_xnikPkt.isWorkload() || !l_drop || (l_xnikPkt.getSeqNo() != 8)) {
             l_xnikPkt.write(p_outStr);
        }
        else if (l_drop && l_xnikPkt.isData() && !l_xnikPkt.isWorkload() && (l_xnikPkt.getSeqNo() == 8)) {
            //drop pkts and switch off l_drop
            if (l_dropTimes == 0) {
                l_drop = false;
            }
            else {
                l_dropTimes--;
            }
        }
#endif
#if DropAck
        if (!l_xnikPkt.isAck() || !l_drop) {
             l_xnikPkt.write(p_outStr);
        }
        else if (l_drop && l_xnikPkt.isAck()) {
            //drop pkts and switch off l_drop
            l_drop = false;
        }
#endif
    }
}
