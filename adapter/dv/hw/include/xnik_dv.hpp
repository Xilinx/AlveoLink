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
        static const unsigned int t_WideDataBits = t_DVdataBits + t_DestBits;
        typedef ap_axiu<t_DVdataBits, 0, 0, t_DestBits> DV_PktType;
        typedef ap_axiu<t_NetDataBits, 0, 0, t_DestBits> XNIK_WidePktType;
    public:
        XNIK_DV(){}
        void readAXIS(hls::stream<XNIK_WidePktType>& p_inStr,
                      hls::stream<ap_uint<t_WideDataBits> >& p_outStr0,
                      hls::stream<ap_uint<t_WideDataBits> >& p_outStr1,
                      hls::stream<ap_uint<t_WideDataBits> >& p_outStr2,
                      hls::stream<ap_uint<t_WideDataBits> >& p_outStr3) {
            while (true) {
#pragma HLS PIPELINE II=1
                if (!p_outStr0.full() && !p_outStr1.full() && !p_outStr2.full() && !p_outStr3.full()) {
                    XNIK_WidePktType l_xnikPkt = p_inStr.read();
                    ap_uint<t_WideDataBits> l_val0, l_val1, l_val2, l_val3;

                    l_val0(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val0(t_WideDataBits-1, t_DestBits) = l_xnikPkt.data(t_DVdataBits-1, 0);

                    l_val1(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val1(t_WideDataBits-1, t_DestBits) = l_xnikPkt.data(2*t_DVdataBits-1, t_DVdataBits);

                    l_val2(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val2(t_WideDataBits-1, t_DestBits) = l_xnikPkt.data(3*t_DVdataBits-1, 2*t_DVdataBits);

                    l_val3(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val3(t_WideDataBits-1, t_DestBits) = l_xnikPkt.data(4*t_DVdataBits-1, 3*t_DVdataBits);

                    p_outStr0.write(l_val0);
                    p_outStr1.write(l_val1);
                    p_outStr2.write(l_val2);
                    p_outStr3.write(l_val3);
                }
            }
        }

        void writeAXIS(hls::stream<ap_uint<t_WideDataBits> >& p_inStr,
                       hls::stream<DV_PktType>& p_outStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                ap_uint<t_WideDataBits> l_val = p_inStr.read();
                DV_PktType l_dvPkt;
                l_dvPkt.dest = l_val(t_DestBits-1,0);
                l_dvPkt.data = l_val(t_WideDataBits-1, t_DestBits);
                p_outStr.write(l_dvPkt); 
            }
        }
                

        void xnikWide2DV(hls::stream<XNIK_WidePktType>& p_inStr,
                         hls::stream<DV_PktType>& p_outStr0,
                         hls::stream<DV_PktType>& p_outStr1,
                         hls::stream<DV_PktType>& p_outStr2,
                         hls::stream<DV_PktType>& p_outStr3) {

            hls::stream<ap_uint<t_WideDataBits> > l_str[4];
#pragma HLS STREAM variable=l_str depth=4
#pragma HLS DATAFLOW
            readAXIS(p_inStr, l_str[0], l_str[1], l_str[2], l_str[3]);
            writeAXIS(l_str[0], p_outStr0);
            writeAXIS(l_str[1], p_outStr1);
            writeAXIS(l_str[2], p_outStr2);
            writeAXIS(l_str[3], p_outStr3);
            
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
