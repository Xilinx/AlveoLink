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

typedef enum {
    tx_idle=0,
    tx_ack,
    tx_sendData,
    tx_waitAck
} TX_STATE;


//data transfer length is always aligned to t_NetDataBytes;
template <unsigned int t_MaxConnections,
          unsigned int t_MTUBytes,
          unsigned int t_NetDataBits,
          unsigned int t_DestBits>
class XNIK_TX {
public:
    static const unsigned int t_TypeBits = t_DestBits / 2;
    static const unsigned int t_SeqBits = t_DestBits / 2;
    static const unsigned int t_TypeSeqBits = t_TypeBits + t_SeqBits;
    static const unsigned int t_NetDataBytes = (t_NetDataBits/8);
    static const unsigned int t_MaxStrobes = t_MTUBytes / t_NetDataBytes;
    static const unsigned int t_BufSize = t_MaxStrobes * 2;
    static const unsigned int t_MaxTxSeqNo = t_MaxStrobes - 1;
    static const unsigned int t_AckPktBits = t_DestBits + t_TypeBits + t_SeqBits;
    typedef typename PktUDP<t_NetDataBits, t_DestBits>::TypeAXIS UdpPktType;
public:
    XNIK_TX(){
LOOP_INIT_SEQNO:
        m_state = TX_STATE::tx_idle;
        m_waitAck = false;
        m_isDone = false;
        m_waitCycles = 0;
        m_waitCyclesConfig = 0;
        m_sentArr = 0;
        m_waitArr = 0;
        for (unsigned int i=0; i<t_MaxConnections; ++i) {
#pragma HLS PIPELINE II=1
            m_sendSeqNoArr[i] = 0;
        }
    }
    void pktGen(hls::stream<UdpPktType>& p_inKrnlStr,
            hls::stream<ap_uint<t_AckPktBits> >& p_inAckStr,
            hls::stream<UdpPktType>& p_outPktStr) {
#pragma HLS ARRAY_PARTITION variable = m_sendSeqNoArr complete dim=0
        uint16_t l_ackSeqNo = m_ackPkt.getSeqNo();
        uint16_t l_sendSeqNo = m_sendSeqNoArr[m_ackDest];
        bool l_received = (l_ackSeqNo == l_sendSeqNo) && m_ackPkt.isRecAck();
        bool l_retran = (l_ackSeqNo < l_sendSeqNo) && m_ackPkt.isRecAck();
        switch(m_state) {
            case TX_STATE::tx_idle:
                if (!p_inAckStr.empty()) {
                    m_ackPkt.read(p_inAckStr);
                    m_ackDest = m_ackPkt.getDest();
                    m_state = TX_STATE::tx_ack;
                }
                else if (!p_inKrnlStr.empty()) {
                    m_state = TX_STATE::tx_sendData;
                }
                break;
            case TX_STATE::tx_ack:
                if (l_received) {
                    m_sendSeqNoArr[m_ackDest] = 0;
                    m_waitArr[m_ackDest] = 0;
                    m_waitAck = m_waitArr.or_reduce();
                    if (m_waitAck){
                        m_waitCycles = m_waitCyclesConfig;
                        m_state = TX_STATE::tx_waitAck;
                    }
                    else {
                        if (m_isDone) {
                            m_isDone = false;
                            m_sentArr = 0;
                        }
                        m_state = TX_STATE::tx_idle;
                    }
                }
                else if (m_ackPkt.isSendAck()) {
                    m_ackPkt.setType(XNIK_PKT_TYPE::ACK);
                    m_ackPkt.send(p_outPktStr);
                    if (m_waitAck) {
                        m_waitCycles = m_waitCyclesConfig;
                        m_state = TX_STATE::tx_waitAck;
                    } else {
                        m_state = TX_STATE::tx_idle;
                    }
                }
                else if (l_retran) {
                    for (unsigned int i=l_ackSeqNo; i< l_sendSeqNo; ++i) {
#pragma HLS PIPELINE II=1
                        ap_uint<t_SeqBits> l_seqNo = i;
                        ap_uint<t_NetDataBits> l_dat = m_pktDatBuf[m_ackDest][i];
                        ap_uint<1> l_last = m_pktLastBuf[m_ackDest][i];
                        PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
                        l_xnikPkt.sendData(m_ackDest, l_dat, l_last, l_seqNo, p_outPktStr);
                    }
                    if (m_waitAck) {
                        ap_uint<t_NetDataBits> l_dat = m_pktDatBuf[m_ackDest][l_sendSeqNo];
                        ap_uint<1> l_last = m_pktLastBuf[m_ackDest][l_sendSeqNo];
                        PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
                        l_xnikPkt.sendData(m_ackDest, l_dat, l_last, l_sendSeqNo, p_outPktStr);
                        m_waitCycles = m_waitCyclesConfig;
                        m_state = TX_STATE::tx_waitAck;
                    } else {
                        m_state = TX_STATE::tx_idle;
                    }
                }
                break;
            case TX_STATE::tx_sendData:
TX_LOOP_SENDDATA:
                while (p_inAckStr.empty() && !m_waitAck && !p_inKrnlStr.empty()) {
#pragma HLS PIPELINE II=1
                    PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
                    l_xnikPkt.read(p_inKrnlStr);
                    ap_uint<t_DestBits> l_dest = l_xnikPkt.getDest();
                    ap_uint<t_SeqBits> l_nextSeqNo = m_sendSeqNoArr[l_dest];
                    m_waitArr[l_dest] = (!(l_nextSeqNo < t_MaxTxSeqNo) && (l_xnikPkt.getLast() == 1)) || (!l_xnikPkt.isWorkload());
                    m_waitAck = m_waitArr.or_reduce();
                    if (l_xnikPkt.isDone()) {
                        m_isDone = true;
                    }
                    if (l_xnikPkt.isWorkload()) {
                        m_sentArr[l_dest] = 1;
                    }
                    l_xnikPkt.send(l_nextSeqNo, p_outPktStr);
                    if (!m_waitAck) {
                        m_sendSeqNoArr[l_dest] = l_nextSeqNo + 1;
                    }
                    m_pktDatBuf[l_dest][l_nextSeqNo] = l_xnikPkt.getData();
                    m_pktLastBuf[l_dest][l_nextSeqNo] = l_xnikPkt.getLast();
                    if (l_xnikPkt.isStart()) {
                        m_waitCyclesConfig = l_xnikPkt.getWaitCycles();
                    }
                }
                if (m_waitAck || m_isDone) {
                    if (m_isDone) {
TX_LOOP_SETWAITARR:
                        for (unsigned int i=0; i<t_MaxConnections; ++i) {
#pragma HLS PIPELINE II=1
                            if (m_sentArr[i] && !m_waitArr[i]) {
                                m_waitArr[i] = 1; 
                            }
                        }
                    }
                    m_waitAck = m_waitArr.or_reduce();
                    m_waitCycles = m_waitCyclesConfig;
                    m_state = TX_STATE::tx_waitAck;
                }
                else if (!p_inAckStr.empty()) {
                    m_ackPkt.read(p_inAckStr);
                    m_ackDest = m_ackPkt.getDest();
                    m_state = TX_STATE::tx_ack;
                }
                break;
            case TX_STATE::tx_waitAck:
                if (!p_inAckStr.empty()) {
                    m_ackPkt.read(p_inAckStr);
                    m_ackDest = m_ackPkt.getDest();
                    m_state = TX_STATE::tx_ack;
                }
                else if (m_waitCycles == 0) {
                    ap_uint<t_DestBits> l_dest;
TX_LOOP_FINDWAITDST:
                    for (unsigned int i=0; i<t_MaxConnections; ++i) {
#pragma HLS PIPELINE II=1
                        if (m_waitArr[i]) {
                            l_dest = i;
                            break;
                        }
                    }
                    PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
                    ap_uint<t_SeqBits> l_seqNo = m_sendSeqNoArr[l_dest];
                    PktType<t_TypeBits, t_SeqBits> l_pktType(XNIK_PKT_TYPE::QUERY, l_seqNo);
                    ap_uint<t_NetDataBits> l_dat;
                    l_dat.range(t_TypeSeqBits-1,0) = l_pktType.getTypeSeq();
                    l_dat.range(t_NetDataBits-1, t_TypeSeqBits) = 0;
                    UdpPktType l_pkt;
                    l_pkt.data = l_dat;
                    l_pkt.keep = -1;
                    l_pkt.last = 1;
                    l_pkt.dest = l_dest;
                    p_outPktStr.write(l_pkt);
                    m_waitCycles = m_waitCyclesConfig;
                }
                else {
                    m_waitCycles--;
                }
                break;
        }

    }

private:
        ap_uint<t_SeqBits> m_sendSeqNoArr[t_MaxConnections];
        bool m_waitAck;
        bool m_isDone;
        ap_uint<t_MaxConnections> m_sentArr;
        ap_uint<t_MaxConnections> m_waitArr;
        ap_uint<t_NetDataBits> m_pktDatBuf[t_MaxConnections][t_BufSize];
        ap_uint<1> m_pktLastBuf[t_MaxConnections][t_BufSize];
        AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> m_ackPkt;
        ap_uint<t_DestBits> m_ackDest;
        uint16_t m_waitCycles, m_waitCyclesConfig;
        TX_STATE m_state;
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
    typedef typename PktUDP<t_NetDataBits, t_DestBits>::TypeAXIS UdpPktType;
public:
    XNIK_RX(){
LOOP_INIT_SEQNO:
        for (unsigned int i=0; i<t_MaxConnections; ++i) {
#pragma HLS PIPELINE II=1
            m_recSeqNoArr[i] = 0;
            m_ackSeqNoArr[i] = 0;
            m_waitArr[i] = false;
            m_ackArr[i] = false;
        }
    }
    
    void pktDec(hls::stream<UdpPktType>& p_inPktStr,
                hls::stream<UdpPktType>& p_outKrnlStr,
                hls::stream<ap_uint<t_AckPktBits> >& p_outAckStr) {
        PktXNIK<t_NetDataBits, t_DestBits> l_xnikPkt;
#pragma HLS ARRAY_PARTITION variable = m_recSeqNoArr complete dim=0
#pragma HLS ARRAY_PARTITION variable = m_ackSeqNoArr complete dim=0
#pragma HLS ARRAY_PARTITION variable = m_waitArr complete dim=0
#pragma HLS ARRAY_PARTITION variable = m_ackArr complete dim=0
LOOP_DECODER:
        while (!p_inPktStr.empty()) {
#pragma HLS PIPELINE II=1
            l_xnikPkt.read(p_inPktStr);
            ap_uint<t_DestBits> l_dest = l_xnikPkt.getDest();
            ap_uint<t_SeqBits> l_recSeqNo = l_xnikPkt.getSeqNo();
            ap_uint<t_SeqBits> l_expSeqNo = m_recSeqNoArr[l_dest];
            ap_uint<t_SeqBits> l_ackSeqNo = m_ackSeqNoArr[l_dest];
            bool l_wait = m_waitArr[l_dest];
            bool l_hasAcked = m_ackArr[l_dest];

            AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_sendAckPkt(XNIK_PKT_TYPE::SEND_ACK, l_expSeqNo, l_dest);
            AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_reSendAckPkt(XNIK_PKT_TYPE::SEND_ACK, l_ackSeqNo, l_dest);
            AckPkt<t_NetDataBits, t_DestBits, t_TypeBits, t_SeqBits> l_recAckPkt(XNIK_PKT_TYPE::REC_ACK, l_recSeqNo, l_dest);

            bool l_isExpected = (l_recSeqNo == l_expSeqNo);
            bool l_isMissingPkts = (l_recSeqNo > l_expSeqNo);
            bool l_shouldAck = ((l_recSeqNo >= t_MaxSeqNo) && l_xnikPkt.isLast()) || 
                                (!l_xnikPkt.isWorkload()); 

            if (l_xnikPkt.isData()) {
                if (l_isExpected) {
                    l_xnikPkt.write(p_outKrnlStr);
                    m_waitArr[l_dest] = false;
                    if (l_shouldAck) {
                        l_sendAckPkt.write(p_outAckStr);
                        m_recSeqNoArr[l_dest] = 0;
                        m_ackArr[l_dest] = true;
                        m_ackSeqNoArr[l_dest] = l_recSeqNo;
                    }
                    else {
                        m_recSeqNoArr[l_dest] = l_recSeqNo + 1;
                        m_ackArr[l_dest] = false;
                    }
                }
                else if (l_isMissingPkts && !l_wait) {//send ACK to sender to ask for misssing pkts
                    l_sendAckPkt.write(p_outAckStr);
                    m_waitArr[l_dest] = true;
                }
            }
            else if (l_xnikPkt.isAck()) {
                l_recAckPkt.write(p_outAckStr);
            }
            else if (l_xnikPkt.isQuery()) {
                if (l_hasAcked) {
                    l_reSendAckPkt.write(p_outAckStr);
                }
                else {
                    if (l_isExpected) {
                        m_ackArr[l_dest] = true;
                        m_ackSeqNoArr[l_dest] = l_recSeqNo;
                        m_recSeqNoArr[l_dest] = 0;
                        l_sendAckPkt.write(p_outAckStr);
                    }
                }
            }
        }
    }

private:
        ap_uint<t_SeqBits> m_recSeqNoArr[t_MaxConnections];
        ap_uint<t_SeqBits> m_ackSeqNoArr[t_MaxConnections];
        bool m_waitArr[t_MaxConnections];
        bool m_ackArr[t_MaxConnections];
};

}
}

#endif
