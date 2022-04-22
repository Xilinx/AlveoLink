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

#define DropCtrl 0 
#define DropData 1
#define DropAck 1

typedef AlveoLink::adapter::PktUDP<AL_netDataBits, AL_destBits>::TypeAXIS UdpPktType;

static bool l_dropData = true;
static bool l_dropCtrl = true;
static bool l_dropAck = true;

extern "C" void krnl_pktDropper_1(hls::stream<UdpPktType>& p_inStr,
                                hls::stream<UdpPktType>& p_outStr) {
    AXIS(p_inStr)
    AXIS(p_outStr)
    AP_CTRL_NONE(return)
    uint16_t t_startSeq = 12;
    uint16_t t_dropPkts = 1;

    while (!p_inStr.empty()) {
#pragma HLS PIPELINE II=1
        AlveoLink::adapter::PktXNIK<AL_netDataBits, AL_destBits> l_xnikPkt;
        l_xnikPkt.read(p_inStr);
        ap_uint<8> l_seqNo = l_xnikPkt.getSeqNo();

#if DropData
        if (!l_dropData || !l_xnikPkt.isData() || !l_xnikPkt.isWorkload() || (l_seqNo < t_startSeq) || (l_seqNo >= (t_startSeq+t_dropPkts))) {
            l_xnikPkt.write(p_outStr);
        }
        else if (l_dropData && l_xnikPkt.isData() && l_xnikPkt.isWorkload() && (l_seqNo == (t_startSeq+t_dropPkts-1))) {
            //drop last data pkts and switch off l_dropData
            l_dropData = false;
        }
#endif
#if DropCtrl
        if (!l_xnikPkt.isData() || l_xnikPkt.isWorkload() || !l_dropCtrl) {
             l_xnikPkt.write(p_outStr);
        }
        else if (l_dropCtrl && l_xnikPkt.isData() && !l_xnikPkt.isWorkload()) {
            //drop pkts and switch off l_dropCtrl
            l_dropCtrl = false;
        }
#endif
#if DropAck
        if (!l_xnikPkt.isAck() || !l_dropAck || (l_xnikPkt.getSeqNo() != 13)) {
             l_xnikPkt.write(p_outStr);
        }
        else if (l_dropAck && l_xnikPkt.isAck() && (l_xnikPkt.getSeqNo() == 13)) {
            //drop pkts and switch off l_dropAck
            l_dropAck = false;
        }
#endif
    }
}
