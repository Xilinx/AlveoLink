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
#include "xnikSync.hpp"

typedef typename AlveoLink::kernel::HopCtrlPkt<AL_netDataBits, AL_destBits>::TypeAXIS UdpPktType;

AlveoLink::kernel::xnikSync_manager<AL_netDataBits, AL_destBits> l_xnikSyncManager;

extern "C" void krnl_dummyManager(hls::stream<UdpPktType>& p_inStr,
                                 hls::stream<UdpPktType>& p_outStr) {
    AXIS(p_inStr)
    AXIS(p_outStr)
    AP_CTRL_NONE(return)

    l_xnikSyncManager.process(p_inStr, p_outStr);
}
