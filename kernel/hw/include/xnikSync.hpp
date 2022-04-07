
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

#ifndef XNIKSYNC_HPP
#define XNIKSYNC_HPP

#include "xnikPktDefs.hpp"

namespace AlveoLink {
namespace kernel {

    typedef enum {
        idle = 0,
        wait_start,
        active,
        state_done
    } SYNC_STATE;

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    class xnikSync_TX {
        public:
            typedef typename HopCtrlPkt<t_NetDataBits, t_DestBits>::TypeAXIS UdpPktType;
        public:
            xnikSync_TX() {
#pragma HLS INLINE
                m_state = SYNC_STATE::idle;
                m_myId = 0;
                m_numDevs = 0;
                m_response = true;
                m_batchPkts = 7;
                m_timeOutCycles = 199;
            }
            void process(hls::stream<ap_uint<t_NetDataBits> >& p_nhop2xnikStr, 
                         hls::stream<ap_uint<t_NetDataBits> >& p_rxStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_txStr,
                         hls::stream<UdpPktType>& p_outStr) {

                bool l_exit = false;
                bool l_waitSending = false;
                uint32_t l_batchPkts, l_timeOutCycles;
LOOP_NHOP2XNIK:
                while(!l_exit) {
#pragma HLS PIPELINE II=1
                    HopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                    switch(m_state) {
                        case SYNC_STATE::idle:
                            if (l_ctrlPkt.readNB(p_nhop2xnikStr)) { 
                                if (l_ctrlPkt.isConfig()) {
                                    m_myId = l_ctrlPkt.getSrcId();
                                    m_numDevs = l_ctrlPkt.getNumDevs();
                                    m_batchPkts = l_ctrlPkt.getBatchPkts();
                                    m_timeOutCycles = l_ctrlPkt.getTimeOutCycles();
                                    l_batchPkts = m_batchPkts;
                                    l_timeOutCycles = m_timeOutCycles;
                                    l_ctrlPkt.write(p_txStr);
                                    l_ctrlPkt.setSrcId(m_myId);
                                    l_ctrlPkt.setNumDevs(m_numDevs);
                                    l_ctrlPkt.setType(PKT_TYPE::start);
                                    l_ctrlPkt.writeAxis2Dest(m_numDevs, p_outStr);
                                    m_state = SYNC_STATE::wait_start;
                                }
                            }
                            break;
                        case SYNC_STATE::wait_start:
                            if (l_ctrlPkt.readNB(p_rxStr)) {
                                if (l_ctrlPkt.isStart()) {//rec start pkt from manager
                                    m_response = true;
                                    m_state = SYNC_STATE::active; 
                                }
                            }
                            break;
                        case SYNC_STATE::active:
                            if (l_ctrlPkt.readNB(p_rxStr)) {
                                if (l_ctrlPkt.isQueryStatus()) {
                                    m_response = true;
                                }
                            }
                            else if (l_waitSending) {
                                if (l_timeOutCycles == 0) {
                                    l_waitSending = false;
                                    l_timeOutCycles = m_timeOutCycles;
                                }
                                else {
                                    l_timeOutCycles = l_timeOutCycles-1;
                                }
                            }
                            else if (l_ctrlPkt.readNB(p_nhop2xnikStr)) {
                                if (l_ctrlPkt.isDoneWork()) {
                                    if (m_response) {
                                        l_ctrlPkt.writeAxis2Dest(m_numDevs, p_outStr);
                                        m_response = false;
                                    }
                                    l_ctrlPkt.write(p_txStr);
                                    m_state = SYNC_STATE::state_done;
                                }
                                else if (l_ctrlPkt.isWorkload()) {
                                    if (!l_waitSending) {
                                        if (l_batchPkts == 0) {
                                            l_waitSending = true;
                                            l_batchPkts = m_batchPkts;
                                        }
                                        else {
                                            l_batchPkts = l_batchPkts-1;
                                        }
                                        l_ctrlPkt.writeAxis(p_outStr);
                                    }
                                }
                            }
                            break;
                        case SYNC_STATE::state_done:
                            if (l_ctrlPkt.readNB(p_rxStr)) {
                                if (l_ctrlPkt.isWorkload()) {
                                    m_state = SYNC_STATE::active;
                                }
                                else if (l_ctrlPkt.isQueryStatus()) {
                                    l_ctrlPkt.setSrcId(m_myId);
                                    l_ctrlPkt.setType(PKT_TYPE::idle_after_done);
                                    l_ctrlPkt.writeAxis2Dest(m_numDevs, p_outStr);
                                }
                                else if (l_ctrlPkt.isTerminate()) {
                                    l_exit = true;
                                    m_state = SYNC_STATE::idle;
                                }
                            }
                            else if (l_ctrlPkt.readNB(p_nhop2xnikStr)) {//do nothing, just avoid FIFO block
                                m_state = SYNC_STATE::state_done;
                            }
                            break;
                    }
                }
            }
            
        private:
            SYNC_STATE m_state; 
            ap_uint<t_DestBits> m_myId, m_numDevs;
            bool m_response;
            uint32_t m_batchPkts;
            uint32_t m_timeOutCycles;
    };

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    class xnikSync_RX {
        public:
            typedef typename HopCtrlPkt<t_NetDataBits, t_DestBits>::TypeAXIS UdpPktType;
        public: 
            xnikSync_RX() {
#pragma HLS INLINE
                m_state = SYNC_STATE::idle;
                m_myId = 0;
            }
            void process(hls::stream<UdpPktType >& p_inStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_txStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_rxStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_xnik2nhopStr) {
                bool l_exit = false;
LOOP_XNIK2NHOP:
                while (!l_exit) {
#pragma HLS PIPELINE II=1
                    HopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                    switch (m_state) {
                        case SYNC_STATE::idle:
                            if (l_ctrlPkt.readNB(p_txStr)) {//should only receive cfg pkt from txStr
                                m_myId = l_ctrlPkt.getSrcId();
                                m_state = SYNC_STATE::wait_start;
                            }
                            else if (l_ctrlPkt.readAxisNB(p_inStr)) {
                                m_state = SYNC_STATE::idle;
                            } 
                        break;
                        case SYNC_STATE::wait_start:
                            if (l_ctrlPkt.readAxisNB(p_inStr)) {
                                if (l_ctrlPkt.isStart()) {
                                    l_ctrlPkt.write(p_rxStr);
                                    m_state = SYNC_STATE::active;
                                }
                                else {
                                    l_ctrlPkt.write(p_xnik2nhopStr);
                                }
                            }
                            break;
                        case SYNC_STATE::active:
                            if (l_ctrlPkt.readAxisNB(p_inStr)) {
                                if (l_ctrlPkt.isQueryStatus()) {
                                    l_ctrlPkt.write(p_rxStr);
                                }
                                else if (l_ctrlPkt.isWorkload()) {
                                    l_ctrlPkt.write(p_xnik2nhopStr);
                                }
                            }
                            else if (l_ctrlPkt.readNB(p_txStr)) {
                                if (l_ctrlPkt.isDoneWork()) {
                                    m_state = SYNC_STATE::state_done;
                                }
                            }
                        break;
                        case SYNC_STATE::state_done:
                            if (l_ctrlPkt.readAxisNB(p_inStr)) {
                                l_ctrlPkt.write(p_rxStr);
                                if (l_ctrlPkt.isWorkload()) {
                                    l_ctrlPkt.write(p_xnik2nhopStr);
                                    m_state = SYNC_STATE::active;
                                }
                                else { //pkt is query_status or terminate
                                    if (l_ctrlPkt.isTerminate()) {
                                        l_ctrlPkt.setType(PKT_TYPE::done);
                                        l_ctrlPkt.write(p_xnik2nhopStr);
                                        l_exit = true;
                                        m_state = SYNC_STATE::idle;
                                    }
                                }
                            }
                        break;
                    }
                    
                }
            }
        private:
            SYNC_STATE m_state; 
            ap_uint<t_DestBits> m_myId;
    };
    
    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    class xnikSync_dummyManager {//simple manager only for hw_emu
        public:
            typedef typename HopCtrlPkt<t_NetDataBits, t_DestBits>::TypeAXIS UdpPktType;
        public:
            xnikSync_dummyManager() {
            }
            void process(hls::stream<UdpPktType>& p_inStr,
                         hls::stream<UdpPktType>& p_outStr) {
                    HopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
LOOP_MANAGER:
                    while (!p_inStr.empty()) {
#pragma HLS PIPELINE II=1
                        UdpPktType l_val = p_inStr.read();
                        l_ctrlPkt.setCtrlPkt(l_val.data);
                        ap_uint<t_DestBits> l_pktDest = l_val.dest;

                        ap_uint<t_DestBits> l_typeSeq = l_ctrlPkt.getDest();
                        if (l_ctrlPkt.isDoneWork() && (l_typeSeq(7,0) == 0)) {
                            l_ctrlPkt.setType(PKT_TYPE::query_status);
                        }
                        else if (l_ctrlPkt.isDoneNothing() && (l_typeSeq(7,0) == 0)) {
                            l_ctrlPkt.setType(PKT_TYPE::terminate);
                        }
                        l_val.data = l_ctrlPkt.getCtrlPkt();
                        p_outStr.write(l_val); 
                    }
            }
    };
}
}
#endif
