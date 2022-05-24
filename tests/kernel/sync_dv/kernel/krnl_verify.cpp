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

void transData(ap_uint<AL_netDataBits>* p_sentDestPtr,
               unsigned int p_waitCycles,
                hls::stream<DV_PktType>& p_fromKrnlStr0,
                hls::stream<DV_PktType>& p_fromKrnlStr1,
                hls::stream<DV_PktType>& p_fromKrnlStr2,
                hls::stream<DV_PktType>& p_fromKrnlStr3,
                hls::stream<DV_PktType>& p_2dvStr0,
                hls::stream<DV_PktType>& p_2dvStr1,
                hls::stream<DV_PktType>& p_2dvStr2,
                hls::stream<DV_PktType>& p_2dvStr3) {

    unsigned int l_numPkts = 1;
    unsigned int l_waitCycles = p_waitCycles;
    while (l_waitCycles != 0) {
#pragma HLS PIPELINE II=1
        bool l_emp1 = p_fromKrnlStr0.empty();
        bool l_emp2 = p_fromKrnlStr1.empty();
        bool l_emp3 = p_fromKrnlStr2.empty();
        bool l_emp4 = p_fromKrnlStr3.empty();

        if (!l_emp1 && !l_emp2 && !l_emp3 && !l_emp4) {
            DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
            l_dvPkt0 = p_fromKrnlStr0.read();
            l_dvPkt1 = p_fromKrnlStr1.read();
            l_dvPkt2 = p_fromKrnlStr2.read();
            l_dvPkt3 = p_fromKrnlStr3.read();
        
            p_2dvStr0.write(l_dvPkt0);
            p_2dvStr1.write(l_dvPkt1);
            p_2dvStr2.write(l_dvPkt2);
            p_2dvStr3.write(l_dvPkt3);
            p_sentDestPtr[l_numPkts].range(AL_destBits-1,0) = l_dvPkt0.dest;
            l_numPkts++;
        }
        l_waitCycles--;
    }
    p_sentDestPtr[0].range(31,0) = l_numPkts-1;
}
               
void recData(ap_uint<AL_netDataBits>* p_recDestPtr,
               unsigned int p_waitCycles,
                hls::stream<DV_PktType>& p_fromDvStr0,
                hls::stream<DV_PktType>& p_fromDvStr1,
                hls::stream<DV_PktType>& p_fromDvStr2,
                hls::stream<DV_PktType>& p_fromDvStr3,
                hls::stream<DV_PktType>& p_2KrnlStr0,
                hls::stream<DV_PktType>& p_2KrnlStr1,
                hls::stream<DV_PktType>& p_2KrnlStr2,
                hls::stream<DV_PktType>& p_2KrnlStr3) {

    unsigned int l_numPkts = 1;
    unsigned int l_waitCycles = p_waitCycles;
    while (l_waitCycles != 0) {
#pragma HLS PIPELINE II=1
        bool l_emp1 = p_fromDvStr0.empty();
        bool l_emp2 = p_fromDvStr1.empty();
        bool l_emp3 = p_fromDvStr2.empty();
        bool l_emp4 = p_fromDvStr3.empty();

        if (!l_emp1 && !l_emp2 && !l_emp3 && !l_emp4) {
            DV_PktType l_dvPkt0, l_dvPkt1, l_dvPkt2, l_dvPkt3;
            l_dvPkt0 = p_fromDvStr0.read();
            l_dvPkt1 = p_fromDvStr1.read();
            l_dvPkt2 = p_fromDvStr2.read();
            l_dvPkt3 = p_fromDvStr3.read();
        
            p_2KrnlStr0.write(l_dvPkt0);
            p_2KrnlStr1.write(l_dvPkt1);
            p_2KrnlStr2.write(l_dvPkt2);
            p_2KrnlStr3.write(l_dvPkt3);
            p_recDestPtr[l_numPkts].range(AL_destBits-1,0) = l_dvPkt0.dest;
            l_numPkts++;
        }
        l_waitCycles--;
    }
    p_recDestPtr[0].range(31,0) = l_numPkts-1;
}
               
extern "C" void krnl_verify(ap_uint<AL_netDataBits>* p_sentDestPtr,
                            ap_uint<AL_netDataBits>* p_recDestPtr,
                            unsigned int p_waitCycles,
                            hls::stream<DV_PktType>& p_fromKrnlStr0,
                            hls::stream<DV_PktType>& p_fromKrnlStr1,
                            hls::stream<DV_PktType>& p_fromKrnlStr2,
                            hls::stream<DV_PktType>& p_fromKrnlStr3,
                            hls::stream<DV_PktType>& p_2dvStr0,
                            hls::stream<DV_PktType>& p_2dvStr1,
                            hls::stream<DV_PktType>& p_2dvStr2,
                            hls::stream<DV_PktType>& p_2dvStr3,
                            
                            hls::stream<DV_PktType>& p_fromDvStr0,
                            hls::stream<DV_PktType>& p_fromDvStr1,
                            hls::stream<DV_PktType>& p_fromDvStr2,
                            hls::stream<DV_PktType>& p_fromDvStr3,    
                            hls::stream<DV_PktType>& p_2KrnlStr0,
                            hls::stream<DV_PktType>& p_2KrnlStr1,
                            hls::stream<DV_PktType>& p_2KrnlStr2,
                            hls::stream<DV_PktType>& p_2KrnlStr3) {
    POINTER(p_sentDestPtr, p_sentDestPtr)
    POINTER(p_recDestPtr, p_recDestPtr)
    AXIS(p_fromKrnlStr0)
    AXIS(p_fromKrnlStr1)
    AXIS(p_fromKrnlStr2)
    AXIS(p_fromKrnlStr3)
    AXIS(p_2dvStr0)
    AXIS(p_2dvStr1)
    AXIS(p_2dvStr2)
    AXIS(p_2dvStr3)
    AXIS(p_fromDvStr0)
    AXIS(p_fromDvStr1)
    AXIS(p_fromDvStr2)
    AXIS(p_fromDvStr3)
    AXIS(p_2KrnlStr0)
    AXIS(p_2KrnlStr1)
    AXIS(p_2KrnlStr2)
    AXIS(p_2KrnlStr3)
    SCALAR(p_waitCycles)
    SCALAR(return)

#pragma HLS DATAFLOW
    transData(p_sentDestPtr, p_waitCycles, p_fromKrnlStr0, p_fromKrnlStr1, p_fromKrnlStr2, p_fromKrnlStr3,
              p_2dvStr0, p_2dvStr1, p_2dvStr2, p_2dvStr3);
    recData(p_recDestPtr, p_waitCycles, p_fromDvStr0, p_fromDvStr1, p_fromDvStr2, p_fromDvStr3,
            p_2KrnlStr0, p_2KrnlStr1, p_2KrnlStr2, p_2KrnlStr3);
}
