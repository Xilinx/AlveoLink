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

#ifndef ALVEOLINK_ADAPTER_XNIKDEFS_HPP
#define ALVEOLINK_ADAPTER_XNIKDEFS_HPP

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "graphPktDefs.hpp"

namespace AlveoLink {
namespace adapter {

typedef enum {
    DATA=0,
    ACK,
    RECEIVED,
    RETRAN,
    QUERY
} XNIK_PKT_TYPE;

template <unsigned int t_DataBits,
          unsigned int t_DestBits,
          unsigned int t_UserBits=1>
class PktUDP {
    public:
        typedef ap_axiu<t_DataBits, t_UserBits, 1, t_DestBits> TypeAXIS;
    public:
        PktUDP() {
#pragma HLS INLINE
             m_pkt.keep = -1;
        }
        ap_uint<t_DataBits> getData() {
#pragma HLS INLINE
            return m_pkt.data;
        }
        void setData(ap_uint<t_DataBits>& p_dat) {
#pragma HLS INLINE
            m_pkt.data = p_dat;
        }
        ap_uint<1> getLast() {
            return m_pkt.last;
        }
        void setLast(const ap_uint<1> p_last) {
#pragma HLS INLINE
            m_pkt.last = p_last;
        }
        ap_uint<t_DestBits> getDest() {
#pragma HLS INLINE
            return m_pkt.dest;
        }
        void setDest(const ap_uint<t_DestBits> p_dest) {
#pragma HLS INLINE
            m_pkt.dest = p_dest;
        }
    protected:
        TypeAXIS m_pkt;
};

template <unsigned int t_TypeBits=8,
          unsigned int t_SeqBits=8>
class PktType {
    public:
        static const unsigned int t_TypeSeqBits = t_TypeBits + t_SeqBits;
    public:
        PktType() {}
        PktType (const ap_uint<t_TypeBits>& p_type, const ap_uint<t_SeqBits>& p_seqNo) {
#pragma HLS INLINE
            m_type = p_type;
            m_seqNo = p_seqNo;
        }
        void setTypeSeq(const ap_uint<t_TypeSeqBits>& p_dat) {
#pragma HLS INLINE
            m_type = p_dat.range(t_TypeBits-1, 0);
            m_seqNo = p_dat.range(t_TypeSeqBits-1, t_TypeBits);
        }
        ap_uint<t_TypeSeqBits> getTypeSeq() {
#pragma HLS INLINE
            ap_uint<t_TypeSeqBits> l_val;
            l_val.range(t_TypeBits-1, 0) = m_type;
            l_val.range(t_TypeSeqBits-1, t_TypeBits) = m_seqNo;
            return l_val;
        }
        void setType(const ap_uint<t_TypeBits>& p_type) {
#pragma HLS INLINE
            m_type = p_type;
        }
        ap_uint<t_TypeBits> getType() {
#pragma HLS INLINE
            return m_type;
        }

        void setSeqNo(const ap_uint<t_SeqBits>& p_seqNo) {
#pragma HLS INLINE
            m_seqNo = p_seqNo;
        }
        ap_uint<t_SeqBits> getSeqNo() {
#pragma HLS INLINE
            return m_seqNo;
        }
        bool increaseSeq(const ap_uint<t_SeqBits>& p_maxSeqNo) {
#pragma HLS INLINE
            if (m_seqNo < p_maxSeqNo) {
                m_seqNo++;
                return true;
            }
            else {
                return false;
            }
        }
        bool isData() {
#pragma HLS INLINE
            return (m_type == XNIK_PKT_TYPE::DATA);
        }
        bool isAck() {
#pragma HLS INLINE
            return (m_type == XNIK_PKT_TYPE::ACK);
        }
        bool isReceived() {
#pragma HLS INLINE
            return (m_type == XNIK_PKT_TYPE::RECEIVED);
        }
        bool isRetran() {
#pragma HLS INLINE
            return (m_type == XNIK_PKT_TYPE::RETRAN);
        }
        bool isQuery() {
#pragma HLS INLINE
            return (m_type == XNIK_PKT_TYPE::QUERY);
        }
    protected:
        ap_uint<t_TypeBits> m_type;
        ap_uint<t_SeqBits> m_seqNo;
};

template <unsigned int t_NetDataBits,
          unsigned int t_DestBits,
          unsigned int t_TypeBits=8,
          unsigned int t_SeqBits=8>
class AckPkt : public PktType<t_TypeBits,
                              t_SeqBits> {
    public:
        static const unsigned int t_AckPktBits = t_DestBits + t_TypeBits + t_SeqBits;
        static const unsigned int t_TypeSeqBits = t_TypeBits + t_SeqBits;
        typedef typename PktUDP<t_NetDataBits, t_DestBits>::TypeAXIS AxisPktType;
    public:
        AckPkt() {}
        AckPkt(const ap_uint<t_TypeBits>& p_type, const ap_uint<t_SeqBits>& p_seqNo, const ap_uint<t_DestBits>& p_dest)
        :PktType<t_TypeBits, t_SeqBits>(p_type, p_seqNo) {
#pragma HLS INLINE
            m_dest = p_dest;
        }
        ap_uint<t_DestBits> getDest() {
#pragma HLS INLINE
            return m_dest;
        }
        void setDest(const ap_uint<t_DestBits>& p_dest) {
#pragma HLS INLINE
            m_dest = p_dest;
        }
        void read(hls::stream<ap_uint<t_AckPktBits> >& p_ackStr) {
#pragma HLS INLINE
            ap_uint<t_AckPktBits> l_dat = p_ackStr.read();
            this->setTypeSeq(l_dat(t_TypeSeqBits-1, 0));
            m_dest = l_dat(t_AckPktBits-1, t_TypeSeqBits);
        }
        bool readNB(hls::stream<ap_uint<t_AckPktBits> >& p_ackStr) {
#pragma HLS INLINE
            ap_uint<t_AckPktBits> l_dat;
            bool l_res = p_ackStr.read_nb(l_dat);
            if (l_res) {
                this->setTypeSeq(l_dat(t_TypeSeqBits-1, 0));
                m_dest = l_dat(t_AckPktBits-1, t_TypeSeqBits);
            }
            return l_res;
        }
        void write(hls::stream<ap_uint<t_AckPktBits> >& p_ackStr) {
#pragma HLS INLINE
            ap_uint<t_AckPktBits> l_dat;
            l_dat.range(t_TypeSeqBits-1, 0) = this->getTypeSeq();
            l_dat.range(t_AckPktBits-1, t_TypeSeqBits) = m_dest;
            l_dat.range(t_NetDataBits-1, t_AckPktBits) = 0;
            p_ackStr.write(l_dat);            
        }
        void send(hls::stream<AxisPktType>& p_axisStr) {
#pragma HLS INLINE
            AxisPktType l_val;
            l_val.data(t_TypeSeqBits-1, 0) = this->getTypeSeq();
            l_val.data(t_NetDataBits-1, t_TypeSeqBits) = 0;
            l_val.dest = m_dest;
            l_val.last = 1;
            p_axisStr.write(l_val);
        }
    private:
        ap_uint<t_DestBits> m_dest;
};


template <unsigned int t_NetDataBits,
          unsigned int t_DestBits>
class PktXNIK : public PktUDP<t_NetDataBits,
                              t_DestBits> {
    public:
        static const unsigned int t_TypeBits = t_DestBits / 2;
        static const unsigned int t_SeqBits = t_DestBits / 2;
        static const unsigned int t_AckDataBits = t_DestBits + t_TypeBits + t_SeqBits;
        static const unsigned int t_TypeSeqBits = t_TypeBits + t_SeqBits;
        static const unsigned int t_GraphPktBits = t_DestBits + 8;
        typedef typename PktUDP<t_NetDataBits, t_DestBits>::TypeAXIS AxisPktType;
    public:
        PktXNIK() : PktUDP<t_NetDataBits, t_DestBits>() {
        }
        bool isWorkload() {
#pragma HLS INLINE
            return (m_graphPktType == AlveoLink::kernel::PKT_TYPE::workload);
        }
        bool isLast() {
#pragma HLS INLINE
            return (this->m_pkt.last == 1);
        }
        bool isData() {
#pragma HLS INLINE
            return (m_pktType.isData());
        }
        bool isAck() {
#pragma HLS INLINE
            return (m_pktType.isAck());
        }
        bool isQuery() {
#pragma HLS INLINE
            return (m_pktType.isQuery());
        }
        ap_uint<t_SeqBits> getSeqNo() {
#pragma HLS INLINE
            return (m_pktType.getSeqNo());
        }
        void setTypeSeq(const ap_uint<t_TypeSeqBits> p_typeSeq) {
#pragma HLS INLINE
            m_pktType.setTypeSeq(p_typeSeq);
        }
        void read(hls::stream<AxisPktType>& p_str) {
#pragma HLS INLINE
            p_str.read(this->m_pkt);
            m_pktType.setTypeSeq(this->m_pkt.data(t_TypeSeqBits-1, 0)); 
            m_graphPktType = this->m_pkt.data(t_GraphPktBits-1, t_DestBits+4);
        }
        bool readNB(hls::stream<AxisPktType>& p_str) {
#pragma HLS INLINE
            bool l_res = p_str.read_nb(this->m_pkt);
            if (l_res) {
                m_pktType.setTypeSeq(this->m_pkt.data(t_TypeSeqBits-1, 0)); 
                m_graphPktType = this->m_pkt.data(t_GraphPktBits-1, t_DestBits+4);
            }
            return l_res;
        }
        void write(hls::stream<AxisPktType>& p_str) {
#pragma HLS INLINE
            this->m_pkt.data(t_TypeSeqBits-1, 0) = m_pktType.getTypeSeq();
            this->m_pkt.data(t_GraphPktBits-1, t_DestBits+4) = m_graphPktType;
            p_str.write(this->m_pkt);
        }
        void sendData(ap_uint<t_DestBits> p_dest, ap_uint<t_NetDataBits> p_dat, ap_uint<1> p_last,
                      ap_uint<t_SeqBits> p_seqNo, hls::stream<AxisPktType>& p_str) {
#pragma HLS INLINE
            PktType<t_TypeBits, t_SeqBits> l_pktType(XNIK_PKT_TYPE::DATA, p_seqNo);
            ap_uint<t_NetDataBits> l_dat;
            l_dat.range(t_TypeSeqBits-1,0) = l_pktType.getTypeSeq();
            l_dat.range(t_NetDataBits-1, t_TypeSeqBits) = p_dat.range(t_NetDataBits-1, t_TypeSeqBits);
            this->setData(l_dat);
            this->setDest(p_dest);
            this->setLast(p_last);
            p_str.write(this->m_pkt);
        }
        void send(ap_uint<t_SeqBits> p_seqNo, hls::stream<AxisPktType>& p_str) {
#pragma HLS INLINE
            PktType<t_TypeBits, t_SeqBits> l_pktType(XNIK_PKT_TYPE::DATA, p_seqNo);
            this->m_pkt.data.range(t_TypeSeqBits-1,0) = l_pktType.getTypeSeq();
            p_str.write(this->m_pkt);
        }
    private:
        PktType<t_TypeBits, t_SeqBits> m_pktType;
        ap_uint<4> m_graphPktType;
};

}
}
#endif
