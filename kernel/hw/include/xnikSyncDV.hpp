
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

    typedef enum {
        manager_idle = 0,
        idle_2_active,
        manager_active,
        active_2_query,
        query,
        query_2_termination,
        termination,
        termination_2_idle,
        wait_terminate
    } MANAGER_STATE;

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    void readAxis(hls::stream<ap_axiu<t_NetDataBits, 0, 0, t_DestBits> >& p_inStr,
             hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
        bool l_exit = false;
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_axiu<t_NetDataBits, 0, 0, t_DestBits> l_pkt = p_inStr.read();
            ap_uint<t_NetDataBits> l_val = l_pkt.data;
            l_val(t_DestBits-1, 0) = l_pkt.dest;
            p_outStr.write(l_val);
            l_exit = (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::terminate); 
        }
    }
    template <unsigned int t_NetDataBits>
    void fwdNet2RxStr(hls::stream<ap_uint<t_NetDataBits> >& p_inNetStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_inTxStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_outNetStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_outTxStr) {
        bool l_exit = false;
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_uint<t_NetDataBits> l_netVal;
            ap_uint<t_NetDataBits> l_txVal;
            if (!p_inNetStr.empty() && !p_outNetStr.full()) {
                l_netVal = p_inNetStr.read();
                p_outNetStr.write(l_netVal);
                l_exit = (l_netVal(23,20) == AlveoLink::kernel::PKT_TYPE::terminate);
            }
            if (!p_inTxStr.empty() && !p_outTxStr.full()) {
                l_txVal = p_inTxStr.read();
                p_outTxStr.write(l_txVal);
            }
        }
    }

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    void readAxis(const uint16_t p_numDevs,
             hls::stream<ap_axiu<t_NetDataBits, 0, 0, t_DestBits> >& p_inStr,
             hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
        uint16_t l_exitCnts = p_numDevs;
        bool l_exit = (l_exitCnts == 0);
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_axiu<t_NetDataBits, 0, 0, t_DestBits> l_pkt = p_inStr.read();
            ap_uint<t_NetDataBits> l_val = l_pkt.data;
            l_val(t_DestBits-1, 0) = l_pkt.dest;
            p_outStr.write(l_val);
            if (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::terminate) {
                l_exitCnts--;
            }
            l_exit = (l_exitCnts == 0);
        }
    }

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    void writeAxis(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                   hls::stream<ap_axiu<t_NetDataBits, 0, 0, t_DestBits> >& p_outStr) {
        bool l_exit = false;
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_uint<t_NetDataBits> l_val = p_inStr.read();
            ap_axiu<t_NetDataBits, 0, 0, t_DestBits> l_pkt;
            l_pkt.data = l_val;
            l_pkt.dest = l_val(t_DestBits-1, 0);
            p_outStr.write(l_pkt);
            l_exit = (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::terminate);
        }
    }
    
    template <unsigned int t_NetDataBits>
    void fwdStr(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
        bool l_exit = false;
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_uint<t_NetDataBits> l_val = p_inStr.read();
            p_outStr.write(l_val);
            l_exit = (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::done);
        }
    }

    template <unsigned int t_NetDataBits>
    void fwdKrnl2TxStr(hls::stream<ap_uint<t_NetDataBits> >& p_inKrnlStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_inRxStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_outKrnlStr,
                   hls::stream<ap_uint<t_NetDataBits> >& p_outRxStr) {
        bool l_exit = false;
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_uint<t_NetDataBits> l_krnlVal;
            ap_uint<t_NetDataBits> l_rxVal;
            if (!p_inKrnlStr.empty() && !p_outKrnlStr.full()) {
                l_krnlVal=p_inKrnlStr.read();
                p_outKrnlStr.write(l_krnlVal);
            }
            if (!p_inRxStr.empty() && !p_outRxStr.full()) {
                l_rxVal = p_inRxStr.read();
                p_outRxStr.write(l_rxVal);
                l_exit = (l_rxVal(23,20) == AlveoLink::kernel::PKT_TYPE::terminate);
            }
        }
    }

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    void writeAxis(const uint16_t p_numDevs,
                   hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                   hls::stream<ap_axiu<t_NetDataBits, 0, 0, t_DestBits> >& p_outStr) {
        uint16_t l_exitCnts = p_numDevs;
        bool l_exit = (l_exitCnts == 0);
        while (!l_exit) {
#pragma HLS PIPELINE II=1
            ap_uint<t_NetDataBits> l_val = p_inStr.read();
            ap_axiu<t_NetDataBits, 0, 0, t_DestBits> l_pkt;
            l_pkt.data = l_val;
            l_pkt.dest = l_val(t_DestBits-1, 0);
            p_outStr.write(l_pkt);
            if (l_val(23,20) == AlveoLink::kernel::PKT_TYPE::terminate) {
                l_exitCnts--;
            }
            l_exit = (l_exitCnts == 0);
        }
    }

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    class xnikSync_TX {
        public:
            xnikSync_TX() {
#pragma HLS INLINE
                m_state = SYNC_STATE::idle;
                m_myId = 0;
                m_tmId = 0;
                m_numDevs = 0;
                m_response = true;
            }
            void process(hls::stream<ap_uint<t_NetDataBits> >& p_nhop2xnikStr, 
                         hls::stream<ap_uint<t_NetDataBits> >& p_rxStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_txStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {

                bool l_exit = false;
                bool l_exitActive = false;
                uint32_t l_batchPkts, l_timeOutCycles;
LOOP_NHOP2XNIK:
                while(!l_exit) {
//#pragma HLS PIPELINE II=1
                    DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                    switch(m_state) {
                        case SYNC_STATE::idle:
                            if (l_ctrlPkt.readNB(p_nhop2xnikStr)) { 
                                if (l_ctrlPkt.isConfig()) {
                                    m_myId = l_ctrlPkt.getSrcId();
                                    m_tmId = l_ctrlPkt.getTmId();
                                    m_numDevs = l_ctrlPkt.getNumDevs();
                                    l_ctrlPkt.write(p_txStr);
                                    l_ctrlPkt.setSrcId(m_myId);
                                    l_ctrlPkt.setTmId(m_tmId);
                                    l_ctrlPkt.setNumDevs(m_numDevs);
                                    l_ctrlPkt.setType(PKT_TYPE::start);
                                    l_ctrlPkt.write2Dest(m_numDevs, p_outStr);
                                    m_state = SYNC_STATE::wait_start;
                                }
                            }
                            break;
                        case SYNC_STATE::wait_start:
                            if (l_ctrlPkt.readNB(p_rxStr)) {
                                if (l_ctrlPkt.isStart()) {//rec start pkt from manager
                                    m_response = true;
                                    l_exitActive = false;
                                    m_state = SYNC_STATE::active; 
                                }
                            }
                            break;
                        case SYNC_STATE::active:
LOOP_TX_ACTIVE:
                            while (!l_exitActive) {
#pragma HLS PIPELINE II=1
                                if (l_ctrlPkt.readNB(p_rxStr)) {
                                    if (l_ctrlPkt.isWorkload()) {
                                        l_ctrlPkt.write2Dest(m_tmId, p_outStr);
                                    }
                                    if (l_ctrlPkt.isQueryStatus()) {
                                        m_response = true;
                                    }
                                }
                                else if (l_ctrlPkt.readNB(p_nhop2xnikStr)) {
                                    if (l_ctrlPkt.isDoneWork()) {
                                        /*if (m_response) {
                                            l_ctrlPkt.setSrcId(m_myId);
                                            l_ctrlPkt.setType(PKT_TYPE::done);
                                            l_ctrlPkt.write2Dest(m_numDevs, p_outStr);
                                            m_response = false;
                                        }
                                        l_ctrlPkt.write(p_txStr);*/
                                        l_exitActive = true;
                                        //m_state = SYNC_STATE::state_done;
                                    }
                                    else if (l_ctrlPkt.isWorkload()) {
                                        l_ctrlPkt.write(p_outStr);
                                    }
                                }
                            }
                            if (l_exitActive) {
                                if (m_response) {
                                    l_ctrlPkt.setSrcId(m_myId);
                                    l_ctrlPkt.setType(PKT_TYPE::done);
                                    l_ctrlPkt.write2Dest(m_numDevs, p_outStr);
                                    m_response = false;
                                }
                                l_ctrlPkt.write(p_txStr);
                                m_state = SYNC_STATE::state_done;
                            }
                            break;
                        case SYNC_STATE::state_done:
                            if (l_ctrlPkt.readNB(p_rxStr)) {
                                if (l_ctrlPkt.isWorkload()) {
                                    l_exitActive = false;
                                    m_state = SYNC_STATE::active;
                                }
                                else if (l_ctrlPkt.isQueryStatus()) {
                                    l_ctrlPkt.setSrcId(m_myId);
                                    l_ctrlPkt.setType(PKT_TYPE::idle_after_done);
                                    l_ctrlPkt.write2Dest(m_numDevs, p_outStr);
                                }
                                else if (l_ctrlPkt.isTerminate()) {
                                    l_ctrlPkt.setSrcId(m_myId);
                                    l_ctrlPkt.write2Dest(m_numDevs, p_outStr);
                                    m_state = SYNC_STATE::idle;
                                    l_exit = true;
                                }
                            }
                            else if (l_ctrlPkt.readNB(p_nhop2xnikStr)) {//do nothing, just avoid FIFO block
                                if (l_ctrlPkt.isWorkload()) {
                                    l_ctrlPkt.write(p_outStr);
                                    l_exitActive = false;
                                    m_state = SYNC_STATE::active;
                                }
                            }
                            break;
                    }
                }
            }
            
        private:
            SYNC_STATE m_state; 
            ap_uint<t_DestBits> m_myId, m_tmId, m_numDevs;
            bool m_response;
    };

    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits>
    class xnikSync_RX {
        public: 
            xnikSync_RX() {
#pragma HLS INLINE
                m_state = SYNC_STATE::idle;
                m_myId = 0;
                m_tmId = 0;
            }
            void process(hls::stream<ap_uint<t_NetDataBits>  >& p_inStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_txStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_rxStr,
                         hls::stream<ap_uint<t_NetDataBits> >& p_xnik2nhopStr) {
                bool l_exit = false;
                bool l_exitActive = false;
LOOP_XNIK2NHOP:
                while (!l_exit) {
//#pragma HLS PIPELINE II=1
                    DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                    switch (m_state) {
                        case SYNC_STATE::idle:
                            if (l_ctrlPkt.readNB(p_txStr)) {//should only receive cfg pkt from txStr
                                m_myId = l_ctrlPkt.getSrcId();
                                m_tmId = l_ctrlPkt.getTmId();
                                m_state = SYNC_STATE::wait_start;
                            }
                            else if (l_ctrlPkt.readNB(p_inStr)) {
                                m_state = SYNC_STATE::idle;
                            } 
                        break;
                        case SYNC_STATE::wait_start:
                            if (l_ctrlPkt.readNB(p_inStr)) {
                                if (l_ctrlPkt.isStart()) {
                                    l_ctrlPkt.write(p_rxStr);
                                    l_exitActive = false;
                                    m_state = SYNC_STATE::active;
                                }
                                else {
                                    l_ctrlPkt.write(p_xnik2nhopStr);
                                }
                            }
                            break;
                        case SYNC_STATE::active:
LOOP_RX_ACTIVE:
                            while (!l_exitActive) {
#pragma HLS PIPELINE II=1
                                if (l_ctrlPkt.readNB(p_txStr)) {
                                    if (l_ctrlPkt.isDoneWork()) {
                                        l_exitActive = true;
                                        //m_state = SYNC_STATE::state_done;
                                    }
                                }
                                else if (l_ctrlPkt.readNB(p_inStr)) {
                                    if (l_ctrlPkt.isQueryStatus()) {
                                        l_ctrlPkt.write(p_rxStr);
                                    }
                                    else if (l_ctrlPkt.isWorkload()) {
                                        if (!p_xnik2nhopStr.full() || (m_myId == m_tmId)) {
                                            l_ctrlPkt.write(p_xnik2nhopStr);
                                        }
                                        else {
                                            l_ctrlPkt.setDest(m_tmId);
                                            l_ctrlPkt.write(p_rxStr);
                                        }
                                    }
    #ifdef HLS_SIM
                                    else if (l_ctrlPkt.isTerminate()) {
                                        l_ctrlPkt.write(p_rxStr);
                                        l_ctrlPkt.setType(PKT_TYPE::done);
                                        l_ctrlPkt.write(p_xnik2nhopStr);
                                        l_exit = true;
                                        m_state = SYNC_STATE::idle;
                                    }
    #endif
                                }
                            }
                            if (l_exitActive) {
                                m_state = SYNC_STATE::state_done;
                            }
                        break;
                        case SYNC_STATE::state_done:
                            if (!p_txStr.empty()) {
                                ap_uint<t_NetDataBits> l_tmp = p_txStr.read(); //read out redundant tx data
                            }
                            else if (l_ctrlPkt.readNB(p_inStr)) {
                                l_ctrlPkt.write(p_rxStr);
                                if (l_ctrlPkt.isWorkload()) {
                                    l_ctrlPkt.write(p_xnik2nhopStr);
                                    l_exitActive = false;
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
            ap_uint<t_DestBits> m_myId, m_tmId;
    };
    
    template <unsigned int t_NetDataBits,
              unsigned int t_DestBits,
              unsigned int t_MaxConnections=16>
    class xnikSync_Manager {
        public:
            xnikSync_Manager() {
                m_state = MANAGER_STATE::manager_idle;
                m_numDevs = 1;
                m_waitCycles = 0;
                m_flushCounter = 1;
                m_startId = 0;
                m_recStatus = -1;
            }
            void recAllPkts(const PKT_TYPE p_pktType, hls::stream<ap_uint<t_NetDataBits> >& p_inStr) {
                m_recStatus(m_numDevs-1, 0) = 0;
                bool l_exit = m_recStatus.and_reduce();
LOOP_MANAGER_RECALL:
                while (!l_exit) {
#pragma HLS PIPELINE II=1
                    DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                    l_ctrlPkt.read(p_inStr);
                    if (l_ctrlPkt.getType() == p_pktType) {
                        ap_uint<t_DestBits> l_srcId = l_ctrlPkt.getSrcId();
                        m_recStatus[l_srcId-m_startId] = 1;
                    }
                    l_exit =  m_recStatus.and_reduce();
                }
            }
            void recAllDonePkts(hls::stream<ap_uint<t_NetDataBits> >& p_inStr, bool& p_allIdle) {
                m_recStatus(m_numDevs-1, 0) = 0;
                p_allIdle = true;
                bool l_exit = m_recStatus.and_reduce();
LOOP_MANAGER_RECALLDONE:
                while (!l_exit) {
#pragma HLS PIPELINE II=1
                    DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                     l_ctrlPkt.read(p_inStr);
                    if ((l_ctrlPkt.getType() == PKT_TYPE::done) || (l_ctrlPkt.getType() == PKT_TYPE::idle_after_done)) {
                        ap_uint<t_DestBits> l_srcId = l_ctrlPkt.getSrcId();
                        m_recStatus[l_srcId-m_startId] = 1;
                        if (l_ctrlPkt.getType() == PKT_TYPE::done) {
                            p_allIdle = false;
                        }
                    }
                    l_exit =  m_recStatus.and_reduce();
                }
            }
            void sendAllPkts(const PKT_TYPE p_pktType, hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
LOOP_MANAGER_SENDALL:
                for (unsigned int i=0; i<m_numDevs; ++i) {
#pragma HLS PIPELINE II=1
                    DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                    l_ctrlPkt.setType(p_pktType);
                    l_ctrlPkt.setDest(i+m_startId);
                    l_ctrlPkt.write(p_outStr);
                }
            }

            void process(uint16_t p_numDevs,
                        uint32_t p_waitCycles,
                        uint16_t p_flushCounter,
                        uint16_t p_startId,
                        hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                        hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
#pragma HLS PIPELINE off

                    m_numDevs = p_numDevs;
                    m_waitCycles = p_waitCycles;
                    m_flushCounter = p_flushCounter;
                    m_startId = p_startId;
                    uint16_t l_flushCounter;
                    bool l_allIdle =true;
                    bool l_procExit = false;
                    uint32_t l_cycleCounter;
LOOP_MANAGER:
                    while (!l_procExit) {
                        switch (m_state) {
                            case MANAGER_STATE::manager_idle:
                                recAllPkts(PKT_TYPE::start, p_inStr);
                                m_state = MANAGER_STATE::idle_2_active;
                                break;
                            case MANAGER_STATE::idle_2_active:
                                sendAllPkts(PKT_TYPE::start, p_outStr);
                                m_state = MANAGER_STATE::manager_active;
                                break;
                            case MANAGER_STATE::manager_active:
                                recAllDonePkts(p_inStr, l_allIdle);
                                m_state = MANAGER_STATE::active_2_query;
                                break;
                            case MANAGER_STATE::active_2_query:
                                sendAllPkts(PKT_TYPE::query_status, p_outStr);
                                m_state = MANAGER_STATE::query;
                                break;
                            case MANAGER_STATE::query:
                                recAllDonePkts(p_inStr, l_allIdle);
                                m_state = MANAGER_STATE::query_2_termination;
                                break;
                            case MANAGER_STATE::query_2_termination:
                                sendAllPkts(PKT_TYPE::query_status, p_outStr);
                                if (l_allIdle) {
                                    l_cycleCounter = m_waitCycles;
                                    l_flushCounter = m_flushCounter;
                                    m_state = MANAGER_STATE::termination;
                                }
                                else {
                                    m_state = MANAGER_STATE::query;
                                }
                                break;
                            case MANAGER_STATE::termination:
                                recAllDonePkts(p_inStr, l_allIdle);
                                m_state = MANAGER_STATE::termination_2_idle;
                                break;
                            case MANAGER_STATE::termination_2_idle:
                                if (l_allIdle && (l_flushCounter == 0)) {
                                    sendAllPkts(PKT_TYPE::terminate, p_outStr);
                                    m_state = MANAGER_STATE::wait_terminate;
                                }
                                else if (l_allIdle && (l_cycleCounter == 0)) {
                                    sendAllPkts(PKT_TYPE::query_status, p_outStr);
                                    l_flushCounter--;
                                    m_state = MANAGER_STATE::termination;
                                }
                                else if (l_allIdle && (l_cycleCounter != 0)) {
                                    l_cycleCounter--;
                                    m_state = MANAGER_STATE::termination_2_idle;
                                }
                                else {
                                    sendAllPkts(PKT_TYPE::query_status, p_outStr);
                                    m_state = MANAGER_STATE::query;
                                }
                                break;
                            case MANAGER_STATE::wait_terminate:
                                recAllPkts(PKT_TYPE::terminate, p_inStr);
                                m_state = MANAGER_STATE::manager_idle;
                                l_procExit = true;
                                break;
                        }
                    }
            }

        private:
            MANAGER_STATE m_state;
            uint16_t m_numDevs;
            uint32_t m_waitCycles;
            uint16_t m_flushCounter;
            uint16_t m_startId;
            ap_uint<t_MaxConnections> m_recStatus; 
    };
    
}
}
#endif