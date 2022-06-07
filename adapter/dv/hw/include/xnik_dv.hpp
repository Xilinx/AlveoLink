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
        static const unsigned int t_WideDvDataBits = t_DVdataBits + t_DestBits;
        static const unsigned int t_WideNetDataBits = t_NetDataBits + t_DestBits;
        static const unsigned int t_WideDestBits = t_DestBits * 4;
        typedef ap_axiu<t_DVdataBits, 0, 0, t_DestBits> DV_PktType;
        typedef ap_axiu<t_NetDataBits, 0, 0, t_DestBits> XNIK_WidePktType;
    public:
        XNIK_DV(){}
        void readAXIS(hls::stream<XNIK_WidePktType>& p_inStr,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr0,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr1,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr2,
                      hls::stream<ap_uint<t_WideDvDataBits> >& p_outStr3) {
            while (true) {
#pragma HLS PIPELINE II=1
                if (!p_outStr0.full() && !p_outStr1.full() && !p_outStr2.full() && !p_outStr3.full()) {
                    XNIK_WidePktType l_xnikPkt = p_inStr.read();
                    ap_uint<t_WideDvDataBits> l_val0, l_val1, l_val2, l_val3;

                    l_val0(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val0(t_WideDvDataBits-1, t_DestBits) = l_xnikPkt.data(t_DVdataBits-1, 0);

                    l_val1(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val1(t_WideDvDataBits-1, t_DestBits) = l_xnikPkt.data(2*t_DVdataBits-1, t_DVdataBits);

                    l_val2(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val2(t_WideDvDataBits-1, t_DestBits) = l_xnikPkt.data(3*t_DVdataBits-1, 2*t_DVdataBits);

                    l_val3(t_DestBits-1,0) = l_xnikPkt.dest;
                    l_val3(t_WideDvDataBits-1, t_DestBits) = l_xnikPkt.data(4*t_DVdataBits-1, 3*t_DVdataBits);

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
                

        void xnikWide2DV(hls::stream<XNIK_WidePktType>& p_inStr,
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

        //following code is a work around out-of-order pkts for nhop. 
        void readDV(hls::stream<DV_PktType>& p_inStr0,
                    hls::stream<DV_PktType>& p_inStr1,
                    hls::stream<DV_PktType>& p_inStr2,
                    hls::stream<DV_PktType>& p_inStr3,
                    hls::stream<ap_uint<t_NetDataBits> >& p_outDatStr,
                    hls::stream<ap_uint<t_DestBits> >& p_outDestStr,
                    hls::stream<ap_uint<5> >& p_outCtrlStr) {
            ap_uint<5> l_ctrl = 0;
            while (true) {
#pragma HLS PIPELINE II=1
                DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
                l_dvPkt0 = p_inStr0.read();
                l_dvPkt1 = p_inStr1.read();
                l_dvPkt2 = p_inStr2.read();
                l_dvPkt3 = p_inStr3.read();
                
                ap_uint<t_NetDataBits> l_dat;
                l_dat(t_DVdataBits-1, 0) = l_dvPkt0.data;
                l_dat(2*t_DVdataBits-1, t_DVdataBits) = l_dvPkt1.data;
                l_dat(3*t_DVdataBits-1, 2*t_DVdataBits) = l_dvPkt2.data;
                l_dat(4*t_DVdataBits-1, 3*t_DVdataBits) = l_dvPkt3.data;

                ap_uint<t_DestBits> l_dest;
                l_dest = l_dvPkt0.dest;
           
                l_ctrl[4] = (l_dvPkt0.data(23,20) != 0); //not workload, pkt from manager
                l_ctrl[0] = 1;
                l_ctrl[1] = (l_dvPkt1.dest == l_dvPkt0.dest);
                l_ctrl[2] = (l_dvPkt2.dest == l_dvPkt0.dest);
                l_ctrl[3] = (l_dvPkt3.dest == l_dvPkt0.dest);

                p_outDatStr.write(l_dat);
                p_outDestStr.write(l_dest);
                p_outCtrlStr.write(l_ctrl);
            }
        }

        void splitPkt(hls::stream<ap_uint<t_NetDataBits> >& p_inDatStr, 
                      hls::stream<ap_uint<t_DestBits> >& p_inDestStr,
                      hls::stream<ap_uint<5> >& p_inCtrlStr,
                      hls::stream<ap_uint<t_WideNetDataBits> >& p_outMgrStr,
                      hls::stream<ap_uint<t_WideNetDataBits> >& p_outWorkStr) {
            bool l_exit = false;
            while (!l_exit) {
#pragma HLS PIPELINE II=1
                ap_uint<t_NetDataBits> l_dat = p_inDatStr.read();
                ap_uint<t_DestBits> l_dest = p_inDestStr.read();
                ap_uint<5> l_ctrl = p_inCtrlStr.read();
                ap_uint<t_WideNetDataBits> l_mgrDat, l_workDat;
                l_mgrDat(t_DestBits-1, 0) = l_dest;
                l_mgrDat(t_DVdataBits+t_DestBits-1, t_DestBits) = l_dat(t_DVdataBits-1, 0);
                l_mgrDat(t_WideNetDataBits-1, t_DVdataBits+t_DestBits) = 0;

                l_workDat(t_DestBits-1, 0) = 0;
                for (auto i=0; i<4; ++i) {
                    if (l_ctrl[4] & l_ctrl[i]) {
                        l_workDat(t_DVdataBits*(i+1)+t_DestBits-1, t_DestBits+i*t_DVdataBits) = 0;
                    }
                    else {
                        l_workDat(t_DVdataBits*(i+1)+t_DestBits-1, t_DestBits+i*t_DVdataBits) = 
                            l_dat(t_DVdataBits*(i+1)-1, t_DVdataBits*i);
                    }
                }

                bool l_writeMgr = l_ctrl[4];
                bool l_writeDat = !l_ctrl.and_reduce();

                if (l_ctrl[4]) {
                    p_outMgrStr.write(l_mgrDat);
                }
                if (!(l_ctrl.and_reduce())) {
                    p_outWorkStr.write(l_workDat);
                }
                l_exit = !(l_ctrl.or_reduce());
            }
        }
        
        void mergePkt(hls::stream<ap_uint<t_WideNetDataBits> >& p_inMgrStr,
                      hls::stream<ap_uint<t_WideNetDataBits> >& p_inWorkStr,
                      hls::stream<XNIK_WidePktType>& p_outStr) {
            while (true) {
#pragma HLS PIPELINE II=1
                ap_uint<t_WideNetDataBits> l_dat;
                XNIK_WidePktType l_xnikPkt;
                if (p_inMgrStr.read_nb(l_dat)) {
                    l_xnikPkt.dest = l_dat(t_DestBits-1, 0);
                    l_xnikPkt.data = l_dat(t_WideNetDataBits-1, t_DestBits);
                    p_outStr.write(l_xnikPkt);
                }
                else if(p_inWorkStr.read_nb(l_dat)) {
                    l_xnikPkt.dest = l_dat(t_DestBits-1, 0);
                    l_xnikPkt.data = l_dat(t_WideNetDataBits-1, t_DestBits);
                    p_outStr.write(l_xnikPkt);
                }
            }
        }

        void DV2Xnik4nHop(hls::stream<DV_PktType>& p_inStr0,
                          hls::stream<DV_PktType>& p_inStr1,
                          hls::stream<DV_PktType>& p_inStr2,
                          hls::stream<DV_PktType>& p_inStr3,
                          hls::stream<XNIK_WidePktType>& p_outStr) {
#pragma HLS DATAFLOW
            hls::stream<ap_uint<t_NetDataBits> > l_datStr;
            hls::stream<ap_uint<t_DestBits> > l_destStr;
            hls::stream<ap_uint<5> > l_ctrlStr;
            hls::stream<ap_uint<t_WideNetDataBits> > l_mgrStr;
            hls::stream<ap_uint<t_WideNetDataBits> > l_workStr;

#pragma HLS STREAM variable=l_datStr depth=4
#pragma HLS STREAM variable=l_destStr depth=4
#pragma HLS STREAM variable=l_ctrlStr depth=4
#pragma HLS STREAM variable=l_mgrStr depth=4
#pragma HLS STREAM variable=l_workStr depth=4

            readDV(p_inStr0, p_inStr1, p_inStr2, p_inStr3, l_datStr, l_destStr, l_ctrlStr);
            splitPkt(l_datStr, l_destStr, l_ctrlStr, l_mgrStr, l_workStr);
            mergePkt(l_mgrStr, l_workStr, p_outStr);
        }
};
}
}
#endif
