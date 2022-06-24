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
#include "xnikPktDefs.hpp"

namespace AlveoLink {
namespace adapter {

template <unsigned int t_NetDataBits = 512,
          unsigned int t_DestBits = 16>
class XNIK_DV{
    public:
        static const unsigned int t_DVdataBits = t_NetDataBits / 4;
        static const unsigned int t_WideDvDataBits = t_DVdataBits + t_DestBits;
        static const unsigned int t_WideDestBits = t_DestBits * 4;
        static const unsigned int t_WideNetDataBits = t_NetDataBits+t_WideDestBits;
        typedef ap_axiu<t_DVdataBits, 0, 0, t_DestBits> DV_PktType;
    public:
        XNIK_DV(){}
        void readAXIS(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr0,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr1,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr2,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr3) {
            while (true) {
#pragma HLS PIPELINE II=1
                if (!p_outStr0.full() && !p_outStr1.full() && !p_outStr2.full() && !p_outStr3.full()) {
                    AlveoLink::kernel::DvHopCtrlPkt<t_NetDataBits, t_DestBits>  l_xnikPkt;
                    l_xnikPkt.read(p_inStr);
                    ap_uint<t_WideDvDataBits> l_val0, l_val1, l_val2, l_val3;

                    ap_uint<t_DestBits> l_dest = l_xnikPkt.getDest();
                    ap_uint<t_NetDataBits> l_val = l_xnikPkt.getCtrlPkt();
                    l_val0(t_DestBits-1,0) = l_dest;
                    l_val0(t_WideDvDataBits-1, t_DestBits) = l_val(t_DVdataBits-1, 0);

                    l_val1(t_DestBits-1,0) = l_dest;
                    l_val1(t_WideDvDataBits-1, t_DestBits) = l_val(2*t_DVdataBits-1, t_DVdataBits);

                    l_val2(t_DestBits-1,0) = l_dest;
                    l_val2(t_WideDvDataBits-1, t_DestBits) = l_val(3*t_DVdataBits-1, 2*t_DVdataBits);

                    l_val3(t_DestBits-1,0) = l_dest;
                    l_val3(t_WideDvDataBits-1, t_DestBits) = l_val(4*t_DVdataBits-1, 3*t_DVdataBits);

                    p_outStr0.write(l_val0);
                    p_outStr1.write(l_val1);
                    p_outStr2.write(l_val2);
                    p_outStr3.write(l_val3);
                }
            }
        }

        void writeAXIS(hls::stream<ap_uint<t_WideDvDataBits> >& p_inStr,
                       hls::stream<DV_PktType>& p_outStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                ap_uint<t_WideDvDataBits> l_val = p_inStr.read();
                DV_PktType l_dvPkt;
                l_dvPkt.dest = l_val(t_DestBits-1,0);
                l_dvPkt.data = l_val(t_WideDvDataBits-1, t_DestBits);
                p_outStr.write(l_dvPkt); 
            }
        }
                

        void xnikWide2DV(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                         hls::stream<DV_PktType>& p_outStr0,
                         hls::stream<DV_PktType>& p_outStr1,
                         hls::stream<DV_PktType>& p_outStr2,
                         hls::stream<DV_PktType>& p_outStr3) {

            hls::stream<ap_uint<t_WideDvDataBits> > l_str[4];
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
                         hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                ap_uint<t_NetDataBits>  l_xnikPkt;
                DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
                l_dvPkt0 = p_inStr0.read();
                l_dvPkt1 = p_inStr1.read();
                l_dvPkt2 = p_inStr2.read();
                l_dvPkt3 = p_inStr3.read();

                l_xnikPkt(t_DVdataBits-1, 0) = l_dvPkt0.data;
                l_xnikPkt(2*t_DVdataBits-1, t_DVdataBits) = l_dvPkt1.data;
                l_xnikPkt(3*t_DVdataBits-1, 2*t_DVdataBits) = l_dvPkt2.data;
                l_xnikPkt(4*t_DVdataBits-1, 3*t_DVdataBits) = l_dvPkt3.data;
                p_outStr.write(l_xnikPkt);
            }
        }

        //following code is a work around out-of-order pkts for nhop. 
        void readDV(hls::stream<DV_PktType>& p_inStr0,
                    hls::stream<DV_PktType>& p_inStr1,
                    hls::stream<DV_PktType>& p_inStr2,
                    hls::stream<DV_PktType>& p_inStr3,
                    hls::stream<ap_uint<t_WideNetDataBits> >& p_outStr) {
            ap_uint<t_NetDataBits> l_dat;
            ap_uint<t_DestBits> l_dest;
            while (true) {
#pragma HLS PIPELINE II=1
                DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
                ap_uint<t_WideNetDataBits> l_val=0;
                if (!p_inStr0.empty() || !p_inStr1.empty() || !p_inStr2.empty() || !p_inStr3.empty()) {
                    if (p_inStr0.read_nb(l_dvPkt0)) {
                        l_val(t_DestBits-1, 0) = l_dvPkt0.dest;
                        l_val(t_WideDvDataBits-1, t_DestBits) = l_dvPkt0.data;
                    }
                    if (p_inStr1.read_nb(l_dvPkt1)) {
                        l_val(t_DestBits+t_WideDvDataBits-1, t_WideDvDataBits) = l_dvPkt1.dest;
                        l_val(2*t_WideDvDataBits-1, t_DestBits+t_WideDvDataBits) = l_dvPkt1.data;
                    }
                    if (p_inStr2.read_nb(l_dvPkt2)) {
                        l_val(t_DestBits+2*t_WideDvDataBits-1, 2*t_WideDvDataBits) = l_dvPkt2.dest;
                        l_val(3*t_WideDvDataBits-1, t_DestBits+2*t_WideDvDataBits) = l_dvPkt2.data;
                    }
                    if (p_inStr3.read_nb(l_dvPkt3)) {
                        l_val(t_DestBits+3*t_WideDvDataBits-1, 3*t_WideDvDataBits) = l_dvPkt3.dest;
                        l_val(4*t_WideDvDataBits-1, t_DestBits+3*t_WideDvDataBits) = l_dvPkt3.data;
                    }
                    p_outStr.write(l_val);
                }
            }
        }

        void splitPkt(hls::stream<ap_uint<t_WideNetDataBits> >& p_inStr, 
                      hls::stream<ap_uint<t_NetDataBits> >& p_outMgrStr,
                      hls::stream<ap_uint<t_NetDataBits> >& p_outWorkStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                ap_uint<t_NetDataBits> l_mgrDat = 0;
                ap_uint<t_NetDataBits> l_workDat = 0;
                ap_uint<t_WideDvDataBits> l_dvArr[4];
                ap_uint<t_DVdataBits> l_dataArr[4];
                ap_uint<t_DestBits> l_destArr[4];
                bool l_writeMgr;
#pragma HLS ARRAY_PARTITION variable = l_dvArr complete dim=1
#pragma HLS ARRAY_PARTITION variable = l_dataArr complete dim=1
#pragma HLS ARRAY_PARTITION variable = l_destArr complete dim=1
                if (!p_inStr.empty()) {
                    ap_uint<t_WideNetDataBits> l_val = p_inStr.read();
                    for (auto i=0; i<4; ++i) {
                        l_dvArr[i] = l_val((i+1)*t_WideDvDataBits-1, i*t_WideDvDataBits);
                        l_dataArr[i] = l_dvArr[i](t_DVdataBits-1, t_DestBits);
                        l_destArr[i] = l_dvArr[i](t_DestBits-1, 0);
                        if (i==0) {
                            l_mgrDat(t_DVdataBits-1, 0) = l_dataArr[0];
                            l_writeMgr = (l_dataArr[0](23,20) != 0);
                            l_workDat(t_DVdataBits-1, 0) = (l_writeMgr)? (ap_uint<t_DVdataBits>)0: l_dataArr[0];
                        }
                        else {
                            l_workDat(t_DVdataBits*(i+1)-1, i*t_DVdataBits) = l_dataArr[i];
                        }
                    }
                    if (l_writeMgr) {
                        p_outMgrStr.write(l_mgrDat);
                    }
                    p_outWorkStr.write(l_workDat);
                }
            }
        }
        
        void mergePkt(hls::stream<ap_uint<t_NetDataBits> >& p_inMgrStr,
                      hls::stream<ap_uint<t_NetDataBits> >& p_inWorkStr,
                      hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                ap_uint<t_NetDataBits> l_dat;
                ap_uint<t_NetDataBits>  l_xnikPkt;
                if (!p_inMgrStr.empty() || !p_inWorkStr.empty()) {
                    if (!p_inMgrStr.empty()) {
                        l_dat = p_inMgrStr.read();
                    }
                    else if(!p_inWorkStr.empty()) {
                        l_dat = p_inWorkStr.read();
                    }
                    p_outStr.write(l_dat);
                }
            }
        }

        void DV2Xnik4nHop(hls::stream<DV_PktType>& p_inStr0,
                          hls::stream<DV_PktType>& p_inStr1,
                          hls::stream<DV_PktType>& p_inStr2,
                          hls::stream<DV_PktType>& p_inStr3,
                          hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
#pragma HLS DATAFLOW
            hls::stream<ap_uint<t_WideNetDataBits> > l_datStr;
            hls::stream<ap_uint<t_NetDataBits> > l_mgrStr;
            hls::stream<ap_uint<t_NetDataBits> > l_workStr;

//#pragma HLS STREAM variable=l_datStr depth=4
//#pragma HLS STREAM variable=l_mgrStr depth=4
//#pragma HLS STREAM variable=l_workStr depth=4

            readDV(p_inStr0, p_inStr1, p_inStr2, p_inStr3, l_datStr);
            splitPkt(l_datStr, l_mgrStr, l_workStr);
            mergePkt(l_mgrStr, l_workStr, p_outStr);
        }
};
}
}
#endif
