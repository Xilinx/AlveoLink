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

#ifndef ALVEOLINK_ADAPTER_XNIK_HPP
#define ALVEOLINK_ADAPTER_XNIK_HPP

#include "xnikDefs.hpp"
#include "types.hpp"

#ifndef __SYNTHESIS__
#include <cassert>
#endif

namespace AlveoLink {
namespace adapter {

//data transfer length is always aligned to t_NetDataBytes;
template <unsigned int t_MaxConnections,
          unsigned int t_MTUBytes,
          unsigned int t_NetDataBits,
          unsigned int t_DestBits>
class XNIK_TX {
public:
    static const unsigned int t_TypeBits = t_DestBits / 2;
    static const unsigned int t_SeqBits = t_DestBits / 2;
    static const unsigned int t_NetDataBytes = (t_NetDataBits/8);
    static const unsigned int t_MaxStrobes = t_MTUBytes / t_NetDataBytes;
    static const unsigned int t_BufSize = t_MaxStrobes * 2;
    static const unsigned int t_MaxTxSeqNo = t_MaxStrobes - 1;
    static const unsigned int t_AckPktBits = t_DestBits + t_TypeBits + t_SeqBits;
    typedef typename PktUDP<t_NetDataBits, t_DestBits>::TypeAXIS PktType;
public:
    XNIK_TX(){
LOOP_INIT_SEQNO:
        m_sendDestFull = false;
        m_waitDest = 0;
        for (unsigned int i=0; i<t_MaxConnections; ++i) {
#pragma HLS PIPELINE II=1
            m_sendSeqNoArr[i] = 0;
        }
    }
    void pktGen(hls::stream<PktType>& p_inKrnlStr,
            hls::stream<ap_uint<t_AckPktBits> >& p_inAckStr,
            hls::stream<PktType>& p_outPktStr) {
#pragma HLS ARRAY_PARTITION variable = m_sendSeqNoArr complete dim=0 
        
        AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_ackPkt;

        if (l_ackPkt.readNB(p_inAckStr)) { //receive flow control info
            ap_uint<t_DestBits> l_dest = l_ackPkt.getDest();
            ap_uint<t_SeqBits> l_nextSeqNo = m_sendSeqNoArr[l_dest];
            if (l_ackPkt.isAck()) {
                l_ackPkt.send(p_outPktStr);
            }
            if (l_ackPkt.isReceived()) {
                m_sendSeqNoArr[l_dest] = 0;
                if (l_dest == m_waitDest) {
                    m_sendDestFull = false;
                }
            }
            if (l_ackPkt.isRetran()) {
                ap_uint<t_SeqBits> l_ackSeqNo = l_ackPkt.getSeqNo();
LOOP_RETRAN:
                for (unsigned int i=l_ackSeqNo; i< l_nextSeqNo; ++i) {
#pragma HLS PIPELINE II=1
                    ap_uint<t_SeqBits> l_seqNo = i;
                    ap_uint<t_NetDataBits> l_dat = m_pktDatBuf[l_dest][i];
                    ap_uint<1> l_last = m_pktLastBuf[l_dest][i];
                    PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
                    l_xnikPkt.sendData(l_dest, l_dat, l_last, l_seqNo, p_outPktStr);
                }
            }
        }
        else {
BLOCK_TRAN_DATA:
            while (p_inAckStr.empty() && !m_sendDestFull && !p_inKrnlStr.empty()) {
    #pragma HLS PIPELINE II=1
                PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
                l_xnikPkt.read(p_inKrnlStr);
                if (l_xnikPkt.isWorkload()) {
                    ap_uint<t_DestBits> l_dest = l_xnikPkt.getDest();
                    ap_uint<t_SeqBits> l_nextSeqNo = m_sendSeqNoArr[l_dest];
                    m_sendDestFull = !(l_nextSeqNo < t_MaxTxSeqNo) && (l_xnikPkt.getLast() == 1);
                    m_waitDest = l_dest;
                    l_xnikPkt.send(l_nextSeqNo, p_outPktStr);
                    m_sendSeqNoArr[l_dest] = m_sendSeqNoArr[l_dest] + 1;
                    m_pktDatBuf[l_dest][l_nextSeqNo] = l_xnikPkt.getData();
                    m_pktLastBuf[l_dest][l_nextSeqNo] = l_xnikPkt.getLast();
                }
                else {
                    l_xnikPkt.setTypeSeq(0);
                    l_xnikPkt.write(p_outPktStr);
                }
            }
        }
    }

private:
        ap_uint<t_SeqBits> m_sendSeqNoArr[t_MaxConnections];
        bool m_sendDestFull;
        ap_uint<t_DestBits> m_waitDest;
        ap_uint<t_NetDataBits> m_pktDatBuf[t_MaxConnections][t_BufSize];
        ap_uint<1> m_pktLastBuf[t_MaxConnections][t_BufSize];
};

template <unsigned int t_MaxConnections,
          unsigned int t_MTUBytes,
          unsigned int t_NetDataBits,
          unsigned int t_DestBits>
class XNIK_RX {
public:
    static const unsigned int t_TypeBits = t_DestBits / 2;
    static const unsigned int t_SeqBits = t_DestBits / 2;
    static const unsigned int t_NetDataBytes = (t_NetDataBits/8);
    static const unsigned int t_MaxStrobes = t_MTUBytes / t_NetDataBytes;
    static const unsigned int t_BufSize = t_MaxStrobes * 2;
    static const unsigned int t_MaxSeqNo = t_MaxStrobes - 1;
    static const unsigned int t_AckPktBits = t_DestBits + t_TypeBits + t_SeqBits;
    typedef typename PktUDP<t_NetDataBits, t_DestBits>::TypeAXIS PktType;
public:
    XNIK_RX(){
LOOP_INIT_SEQNO:
        for (unsigned int i=0; i<t_MaxConnections; ++i) {
#pragma HLS PIPELINE II=1
            m_recSeqNoArr[i] = 0;
        }
    }
    
    void pktDec(hls::stream<PktType>& p_inPktStr,
                hls::stream<PktType>& p_outKrnlStr,
                hls::stream<ap_uint<t_AckPktBits> >& p_outAckStr) {
        PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
#pragma HLS ARRAY_PARTITION variable = m_recSeqNoArr complete dim=0
LOOP_DECODER:
        while (!p_inPktStr.empty()) {
#pragma HLS PIPELINE II=1
            l_xnikPkt.read(p_inPktStr);
            ap_uint<t_DestBits> l_dest = l_xnikPkt.getDest();
            ap_uint<t_SeqBits> l_seqNo = l_xnikPkt.getSeqNo();
            ap_uint<t_SeqBits> l_expSeqNo = m_recSeqNoArr[l_dest]; 
            AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_ackPkt(XNIK_PKT_TYPE::ACK, l_expSeqNo, l_dest);
            AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_receivedPkt(XNIK_PKT_TYPE::RECEIVED, l_seqNo, l_dest);
            AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_retranPkt(XNIK_PKT_TYPE::RETRAN, l_seqNo, l_dest);
            if (l_xnikPkt.isData()) {
                if (!l_xnikPkt.isWorkload()) {
                    l_xnikPkt.write(p_outKrnlStr);
                }
                else if (l_seqNo == l_expSeqNo) {
                    //push data to user kernel
                    l_xnikPkt.write(p_outKrnlStr);
                    if ((l_expSeqNo >= t_MaxSeqNo) && l_xnikPkt.isLast()) {
                        //send ACK packet with expSeqNo
                        l_ackPkt.write(p_outAckStr);
                        m_recSeqNoArr[l_dest] = 0;
                    }
                    else {
                        m_recSeqNoArr[l_dest] =  m_recSeqNoArr[l_dest] + 1;
                    }
                }
                else if (l_seqNo > l_expSeqNo) {
                    //send ACK packet with expSeqNo
                    l_ackPkt.write(p_outAckStr);
                }
            }
            else if (l_xnikPkt.isAck() && (l_seqNo >= t_MaxSeqNo)) {
                //if seqNo == t_MaxSeqNo, push received pkt to ackStream
                l_receivedPkt.write(p_outAckStr);
           }
           else if (l_xnikPkt.isAck()) {
                //else if seqNo < t_MaxSeqNo, push retran pkt to ackStream
                l_retranPkt.write(p_outAckStr);
           }
        }
    }

private:
        ap_uint<t_SeqBits> m_recSeqNoArr[t_MaxConnections];
};

}
}

#endif
