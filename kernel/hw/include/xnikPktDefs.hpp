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

#ifndef GRAPHSYNC_XNIKPKTDEFS_HPP
#define GRAPHSYNC_XNIKPKTDEFS_HPP

#include <cstdint>
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "graphPktDefs.hpp"

namespace AlveoLink {
namespace kernel {

    template <unsigned int t_DestBits> 
    class HopPktHeader {
        public:
            static const unsigned int t_PktBits = t_DestBits + 8;
        public:
            HopPktHeader() {m_len=0;}
            ap_uint<t_PktBits> getHeader() {
#pragma HLS INLINE
                ap_uint<t_PktBits> l_val;
                l_val(t_DestBits-1, 0) = m_dest;
                l_val(t_DestBits+3, t_DestBits) = m_len;
                l_val(t_PktBits-1, t_DestBits+4) = m_type;
                return l_val;
            }
            void setHeader(const ap_uint<t_PktBits>& p_val) {
#pragma HLS INLINE
                m_dest = p_val(t_DestBits-1, 0);
                m_type = p_val(t_PktBits-1, t_DestBits+4);
            }
            bool isWorkload() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::workload);
            }
            bool isConfig() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::config);
            }
            bool isStart() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::start);
            }
            bool isDone() {
#pragma HLS INLINE
                return ((m_type == PKT_TYPE::done) || (m_type == PKT_TYPE::idle_after_done));
            }
            bool isDoneWork() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::done);
            }
            bool isDoneNothing() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::idle_after_done);
            }
            bool isQueryStatus() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::query_status);
            }
            bool isTerminate() {
#pragma HLS INLINE
                return (m_type == PKT_TYPE::terminate);
            }
            ap_uint<t_DestBits> getDest() {
#pragma HLS INLINE
                return m_dest;
            }
            void setDest(const ap_uint<t_DestBits>& p_dest) {
#pragma HLS INLINE
                m_dest = p_dest;
            }
            ap_uint<4> getType() {
#pragma HLS INLINE
                return m_type;
            }
            void setType(const ap_uint<4>& p_type) {
#pragma HLS INLINE
                m_type = p_type;
            }
            
        private:
            ap_uint<t_DestBits> m_dest;
            ap_uint<4> m_len;
            ap_uint<4> m_type;
    };

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits,
              unsigned int t_UserBits=1>
    class HopCtrlPkt : public HopPktHeader<t_DestBits> {
        public:
            static const unsigned int t_HopPktHeaderBits = HopPktHeader<t_DestBits>::t_PktBits; 
            static const unsigned int t_CtrlPktBits = t_HopPktHeaderBits+t_DestBits*2 + 96;
            static const unsigned int t_UnusedBits = t_NetDataBits - t_CtrlPktBits;

            typedef ap_axiu<t_NetDataBits, t_UserBits, 1, t_DestBits> TypeAXIS;
        public:
            HopCtrlPkt() {
                m_srcId = 0;
                m_numDevs = 0;
                m_rest = 0;
                m_batchPkts = 0;
                m_timeOutCycles = 0;
                m_waitCycles = 0;
            }
            ap_uint<t_NetDataBits> getCtrlPkt() {
#pragma HLS INLINE
                ap_uint<t_NetDataBits> l_val;
                l_val(t_HopPktHeaderBits-1,0) = this->getHeader();
                l_val(t_HopPktHeaderBits+t_DestBits-1, t_HopPktHeaderBits) = m_srcId;
                l_val(t_HopPktHeaderBits+2*t_DestBits-1, t_HopPktHeaderBits+t_DestBits) = m_numDevs;
                l_val(t_HopPktHeaderBits+2*t_DestBits+31, t_HopPktHeaderBits+2*t_DestBits) = m_batchPkts;
                l_val(t_HopPktHeaderBits+2*t_DestBits+63, t_HopPktHeaderBits+2*t_DestBits+32) = m_timeOutCycles;
                l_val(t_HopPktHeaderBits+2*t_DestBits+95, t_HopPktHeaderBits+2*t_DestBits+64) = m_waitCycles;
                l_val(t_NetDataBits-1, t_CtrlPktBits) = m_rest;
                return l_val;
            }
            void setCtrlPkt(const ap_uint<t_NetDataBits>& p_val) {
#pragma HLS INLINE
                this->setHeader(p_val(t_HopPktHeaderBits-1,0));
                m_srcId = p_val(t_HopPktHeaderBits+t_DestBits-1, t_HopPktHeaderBits);
                m_numDevs = p_val(t_HopPktHeaderBits+2*t_DestBits-1, t_HopPktHeaderBits+t_DestBits);
                m_batchPkts = p_val(t_HopPktHeaderBits+2*t_DestBits+31, t_HopPktHeaderBits+2*t_DestBits);
                m_timeOutCycles = p_val(t_HopPktHeaderBits+2*t_DestBits+63, t_HopPktHeaderBits+2*t_DestBits+32);
                m_waitCycles = p_val(t_HopPktHeaderBits+2*t_DestBits+95, t_HopPktHeaderBits+2*t_DestBits+64);
                m_rest = p_val(t_NetDataBits-1, t_CtrlPktBits);
            
            }
            ap_uint<t_DestBits> getSrcId() {
#pragma HLS INLINE
                return m_srcId;
            }
            void setSrcId(const ap_uint<t_DestBits>& p_srcId) {
#pragma HLS INLINE
                m_srcId = p_srcId;
            }
            ap_uint<t_DestBits> getNumDevs() {
#pragma HLS INLINE
                return m_numDevs;
            }
            void setNumDevs(const ap_uint<t_DestBits>& p_numDevs) {
#pragma HLS INLINE
                m_numDevs = p_numDevs;
            }
            uint32_t getBatchPkts() {
#pragma HLS INLINE
                return m_batchPkts;
            }
            void setBatchPkts(const uint32_t p_batchPkts) {
#pragma HLS INLINE
                m_batchPkts = p_batchPkts;
            }
            uint32_t getTimeOutCycles() {
#pragma HLS INLINE
                return m_timeOutCycles;
            }
            void setTimeOutCycles(const uint32_t p_timeOutCycles) {
#pragma HLS INLINE
                m_timeOutCycles = p_timeOutCycles;
            }
            uint32_t getWaitCycles() {
#pragma HLS INLINE
                return m_waitCycles;
            }
            void setWaitCycles(const uint32_t p_waitCycles) {
#pragma HLS INLINE
                m_waitCycles = p_waitCycles;
            }
            bool readNB(hls::stream<ap_uint<t_NetDataBits> >& p_str) {
#pragma HLS INLINE
                ap_uint<t_NetDataBits> l_val;
                bool l_res = p_str.read_nb(l_val);
                if (l_res) {
                    setCtrlPkt(l_val);
                }
                return l_res;
            }
            void read(hls::stream<ap_uint<t_NetDataBits> >& p_str) {
#pragma HLS INLINE
                ap_uint<t_NetDataBits> l_val;
                p_str.read(l_val);
                setCtrlPkt(l_val);
            }
            void write(hls::stream<ap_uint<t_NetDataBits> >& p_str) {
#pragma HLS INLINE
                ap_uint<t_NetDataBits> l_val = getCtrlPkt();
                p_str.write(l_val);
            }
            bool readAxisNB(hls::stream<TypeAXIS >& p_str) {
#pragma HLS INLINE
                
                TypeAXIS l_valAxis;
                bool l_res = p_str.read_nb(l_valAxis);
                if (l_res) {
                    setCtrlPkt(l_valAxis.data);
                }
                return l_res;
            }
            void readAxis(hls::stream<TypeAXIS >& p_str) {
#pragma HLS INLINE
                TypeAXIS l_valAxis;
                p_str.read(l_valAxis);
                setCtrlPkt(l_valAxis.data);
            }
            void writeAxis(hls::stream<TypeAXIS>& p_str) {
#pragma HLS INLINE
                ap_uint<t_NetDataBits> l_val = getCtrlPkt();
                TypeAXIS l_valAxis;
                l_valAxis.data = l_val;
                l_valAxis.keep = -1;
                l_valAxis.dest = l_val(t_DestBits-1, 0);
                l_valAxis.last = 1;
                p_str.write(l_valAxis);
            }
            void writeAxis2Dest(const ap_uint<t_DestBits>& p_dest, hls::stream<TypeAXIS>& p_str) {
#pragma HLS INLINE
                ap_uint<t_NetDataBits> l_val = getCtrlPkt();
                TypeAXIS l_valAxis;
                l_valAxis.data = l_val;
                l_valAxis.keep = -1;
                l_valAxis.dest = p_dest;
                l_valAxis.last = 1;
                p_str.write(l_valAxis);
            }
        private:
            ap_uint<t_DestBits> m_srcId;
            ap_uint<t_DestBits> m_numDevs;
            ap_uint<t_UnusedBits> m_rest;
            uint32_t m_batchPkts;
            uint32_t m_timeOutCycles;
            uint32_t m_waitCycles;
    };

}
}
#endif
