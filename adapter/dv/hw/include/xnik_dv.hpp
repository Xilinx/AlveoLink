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

#ifndef ALVEOLINK_ADAPTER_XNIKDV_HPP
#define ALVEOLINK_ADAPTER_XNIKDV_HPP

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h" 

namespace AlveoLink {
namespace adapter {

template <unsigned int t_NetDataBits = 512,
          unsigned int t_DestBits = 16>
class XNIK_DV{
    public:
        static const unsigned int t_DVdataBits = t_NetDataBits / 4;
        typedef ap_axiu<t_DVdataBits, 0, 0, t_DestBits> DV_PktType;
        typedef ap_axiu<t_NetDataBits, 0, 0, t_DestBits> XNIK_WidePktType;
    public:
        XNIK_DV(){}
        void xnikWide2DV(hls::stream<XNIK_WidePktType>& p_inStr,
                         hls::stream<DV_PktType>& p_outStr0,
                         hls::stream<DV_PktType>& p_outStr1,
                         hls::stream<DV_PktType>& p_outStr2,
                         hls::stream<DV_PktType>& p_outStr3) {
            while (true) {
#pragma HLS PIPELINE II=1
                XNIK_WidePktType l_xnikPkt;
                DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
                l_xnikPkt = p_inStr.read();
                l_dvPkt0.data = l_xnikPkt.data(t_DVdataBits-1, 0);
                l_dvPkt1.data = l_xnikPkt.data(2*t_DVdataBits-1, t_DVdataBits);
                l_dvPkt2.data = l_xnikPkt.data(3*t_DVdataBits-1, 2*t_DVdataBits);
                l_dvPkt3.data = l_xnikPkt.data(4*t_DVdataBits-1, 3*t_DVdataBits);
                l_dvPkt0.dest = l_xnikPkt.dest;
                l_dvPkt1.dest = l_xnikPkt.dest;
                l_dvPkt2.dest = l_xnikPkt.dest;
                l_dvPkt3.dest = l_xnikPkt.dest;
                p_outStr0.write(l_dvPkt0);
                p_outStr1.write(l_dvPkt1);
                p_outStr2.write(l_dvPkt2);
                p_outStr3.write(l_dvPkt3);
            }
        }
        void DV2XnikWide(hls::stream<DV_PktType>& p_inStr0,
                         hls::stream<DV_PktType>& p_inStr1,
                         hls::stream<DV_PktType>& p_inStr2,
                         hls::stream<DV_PktType>& p_inStr3,
                         hls::stream<XNIK_WidePktType>& p_outStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                XNIK_WidePktType l_xnikPkt;
                DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
                l_dvPkt0 = p_inStr0.read();
                l_dvPkt1 = p_inStr1.read();
                l_dvPkt2 = p_inStr2.read();
                l_dvPkt3 = p_inStr3.read();

                l_xnikPkt.data(t_DVdataBits-1, 0) = l_dvPkt0.data;
                l_xnikPkt.data(2*t_DVdataBits-1, t_DVdataBits) = l_dvPkt1.data;
                l_xnikPkt.data(3*t_DVdataBits-1, 2*t_DVdataBits) = l_dvPkt2.data;
                l_xnikPkt.data(4*t_DVdataBits-1, 3*t_DVdataBits) = l_dvPkt3.data;
                l_xnikPkt.dest = l_dvPkt0.dest;
                p_outStr.write(l_xnikPkt);
            }
        }

};
}
}
#endif
