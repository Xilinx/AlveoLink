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
#include "xnik_dv.hpp"

typedef AlveoLink::adapter::XNIK_DV<AL_netDataBits, AL_destBits>::DV_PktType DV_PktType;
typedef AlveoLink::adapter::XNIK_DV<AL_netDataBits, AL_destBits>::XNIK_WidePktType XNIK_WidePktType;

extern "C" void krnl_xnik_wide(hls::stream<XNIK_WidePktType>& p_XNIKinStr,
                               hls::stream<DV_PktType>& p_DVinStr0,
                               hls::stream<DV_PktType>& p_DVinStr1,
                               hls::stream<DV_PktType>& p_DVinStr2,
                               hls::stream<DV_PktType>& p_DVinStr3,
                                
                               hls::stream<XNIK_WidePktType >& p_XNIKoutStr,
                               hls::stream<DV_PktType>& p_DVoutStr0,
                               hls::stream<DV_PktType>& p_DVoutStr1,
                               hls::stream<DV_PktType>& p_DVoutStr2,
                               hls::stream<DV_PktType>& p_DVoutStr3) {
    AXIS(p_XNIKinStr)
    AXIS(p_DVinStr0)
    AXIS(p_DVinStr1)
    AXIS(p_DVinStr2)
    AXIS(p_DVinStr3)
    AXIS(p_XNIKoutStr)
    AXIS(p_DVoutStr0)
    AXIS(p_DVoutStr0)
    AXIS(p_DVoutStr0)
    AXIS(p_DVoutStr0)
    AP_CTRL_NONE(return)

    AlveoLink::adapter::XNIK_DV<AL_netDataBits, AL_destBits> l_xnikDV;
#pragma HLS DATAFLOW
    l_xnikDV.xnikWide2DV(p_XNIKinStr, p_DVoutStr0, p_DVoutStr1, p_DVoutStr2, p_DVoutStr3);
    //l_xnikDV.DV2XnikWide(p_DVinStr0, p_DVinStr1, p_DVinStr2, p_DVinStr3, p_XNIKoutStr);
    l_xnikDV.DV2Xnik4nHop(p_DVinStr0, p_DVinStr1, p_DVinStr2, p_DVinStr3, p_XNIKoutStr);
}
