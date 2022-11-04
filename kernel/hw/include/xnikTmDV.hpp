
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

    #ifndef XNIKTM_HPP
    #define XNIKTM_HPP

    #include "xnikPktDefs.hpp"

    namespace AlveoLink {
    namespace kernel {

        typedef enum {
            tm_idle = 0,
            tm_receiving,
            tm_sending,
            tm_done        
        } TM_STATE;

        template <unsigned int t_NetDataBits,
                  unsigned int t_DestBits,
                  unsigned int t_BufSize = 2048>
        class xnikTraffic_Manager {
            public:
                static const unsigned int t_HalfBufSize = t_BufSize/2;
                static const unsigned int t_HalfBufSizeMinus1 = t_HalfBufSize-1;
            public:
                xnikTraffic_Manager() {
                    m_state = TM_STATE::tm_idle;
                    m_myId = 0;
                    m_tmId = 0;
                    m_numDevs = 0;
                    m_waitCycles = 0;
                    m_maxAddr = 0;
                    m_wrAddr = 0;
                    m_rdAddr = 0;
                    m_totalPkts = 0;
                    m_numFulls = 0;
                    m_full = false;
                    m_empty = false;
                }
                
                void bufWrPkts(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                               hls::stream<ap_uint<t_NetDataBits> >& p_outStr,
                               hls::stream<uint32_t>& p_addrCntStr) {
                    uint32_t l_cnt = 0;
                    uint32_t l_waitCycles = m_waitCycles;
                    bool l_hasCnt = false;
                    
                    p_addrCntStr.write(m_wrAddr);
LOOP_BUF_WRPKTS:
                    while (l_waitCycles != 0){
#pragma HLS PIPELINE II=1
                        if (!p_inStr.empty()) {
                            ap_uint<t_NetDataBits> l_val = p_inStr.read();
                            if (!m_full) {
                                m_full = (m_wrAddr == m_maxAddr);
                                //p_memPtr[m_wrAddr] = l_val;
                                p_outStr.write(l_val);
                                if (l_cnt == t_HalfBufSizeMinus1) {
                                    p_addrCntStr.write(t_HalfBufSize);
                                    l_hasCnt = true;
                                    l_cnt = 0;
                                }
                                else { 
                                    if (l_hasCnt) {
                                        p_addrCntStr.write(m_wrAddr);
                                        l_hasCnt = false;
                                    }
                                    l_cnt++;
                                }
                                m_wrAddr++;
                            }
                            else {
                                m_numFulls++;
                            }
                            l_waitCycles = m_waitCycles;
                            m_totalPkts++;
                        }
                        else {
                            l_waitCycles--;
                        }
                    }
                    if (!l_hasCnt) {
                        p_addrCntStr.write(l_cnt);
                    } 
                    p_addrCntStr.write(0);
                }
               
                void writePkts2Mem(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                                   hls::stream<uint32_t>& p_addrCntStr,
                                   ap_uint<t_NetDataBits>* p_memPtr) {

                    uint32_t l_addr = p_addrCntStr.read();
                    bool l_exit = (l_addr == 0);
                    while (!l_exit) {
                        uint32_t l_cnt = p_addrCntStr.read();
LOOP_WRITE_MEM:
                        for (unsigned int i=0; i<l_cnt; ++i) {
#pragma HLS PIPELINE II=1
                            ap_uint<t_NetDataBits> l_val = p_inStr.read();
                            p_memPtr[l_addr+i] = l_val;
                        }
                        l_addr = p_addrCntStr.read();
                        l_exit = (l_addr == 0);
                    }
                }
 
                void storePkts(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                               ap_uint<t_NetDataBits>* p_memPtr) {
#pragma HLS INLINE off
                    hls::stream<ap_uint<t_NetDataBits> > l_valStr;
                    hls::stream<uint32_t> l_addrCntStr;
#pragma HLS STREAM variable=l_valStr depth=t_BufSize
#pragma HLS STREAM variable=l_addrCntStr depth=16

#pragma HLS DATAFLOW
                    bufWrPkts(p_inStr, l_valStr, l_addrCntStr);
                    writePkts2Mem(l_valStr, l_addrCntStr, p_memPtr);
                }

                void loadPkts(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                              ap_uint<t_NetDataBits>* p_memPtr,
                              hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
#pragma HLS INLINE off
    LOOP_TM_SENDING:
                    while (!m_empty && p_inStr.empty()) {
            #pragma HLS PIPELINE II=1
                        m_empty = (m_rdAddr == 1);
                        ap_uint<t_NetDataBits> l_rdVal = p_memPtr[m_rdAddr];
                        p_outStr.write(l_rdVal);
                        m_rdAddr--;
                    }
                }
                void process(uint32_t* p_config,
                            ap_uint<t_NetDataBits>* p_memPtr,
                            hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                            hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
                    bool l_procExit = false;
                    m_myId =  p_config[0];
                    m_tmId = p_config[1];
                    m_numDevs = p_config[2];
                    m_waitCycles = p_config[3];
                    m_maxAddr = p_config[4]; //number of 512-bit words that the memory can store - 1
                    m_wrAddr = 0;
                    m_rdAddr = 0;
                    m_totalPkts = 0;
                    m_numFulls = 0;
                    while (!l_procExit || (m_state != TM_STATE::tm_idle)) {
#pragma HLS PIPELINE off
                        DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                        switch(m_state) {
                            case TM_STATE::tm_idle:
                                l_ctrlPkt.setSrcId(m_myId);
                                l_ctrlPkt.setNumDevs(m_numDevs);
                                l_ctrlPkt.setTmId(m_tmId);
                                l_ctrlPkt.setType(PKT_TYPE::config);
                                l_ctrlPkt.write(p_outStr);
                                m_full = (m_rdAddr == m_maxAddr);
                                m_wrAddr = m_rdAddr + 1;
                                m_state = TM_STATE::tm_receiving;
                            break;

                            case TM_STATE::tm_receiving:
                                storePkts(p_inStr, p_memPtr);
                                m_empty = (m_wrAddr == 1);
                                m_rdAddr = m_wrAddr - 1;
                                m_state = TM_STATE::tm_sending;
                            break;

                            case TM_STATE::tm_sending:
                                loadPkts(p_inStr, p_memPtr, p_outStr);
                                if (!p_inStr.empty()) {
                                    m_full = (m_rdAddr == m_maxAddr);
                                    m_wrAddr = m_rdAddr + 1;
                                    m_state = TM_STATE::tm_receiving;
                                }
                                else {
                                    l_ctrlPkt.setType(PKT_TYPE::done);
                                    l_ctrlPkt.write(p_outStr);
                                    m_state = TM_STATE::tm_done;
                                }
                            break;

                        case TM_STATE::tm_done:
                             if (l_ctrlPkt.readNB(p_inStr)) {
                                if (l_ctrlPkt.isWorkload()) {
                                    m_full = (m_rdAddr == m_maxAddr);
                                    m_wrAddr = m_rdAddr + 1;
                                    if (!m_full) {
                                        p_memPtr[m_wrAddr] = l_ctrlPkt.getCtrlPkt();
                                        m_full = (m_wrAddr == m_maxAddr);
                                        m_wrAddr++;
                                    }
                                    m_state = TM_STATE::tm_receiving;
                                }
                                else if (l_ctrlPkt.isDoneWork()) {
                                    m_state = TM_STATE::tm_idle;
                                    l_procExit = true;
                                }
                             }
                        break;
                    }
                }

                p_config[16] = m_rdAddr;
                p_config[17] = m_wrAddr;
                p_config[18] = m_totalPkts;
                p_config[19] = m_numFulls;
            }

        private:
            TM_STATE m_state;
            ap_uint<t_DestBits> m_myId, m_tmId, m_numDevs;
            uint32_t m_waitCycles;
            uint32_t m_maxAddr;
            uint32_t m_wrAddr;
            uint32_t m_rdAddr;
            uint32_t m_totalPkts;
            uint32_t m_numFulls;
            bool m_full;
            bool m_empty;
    };
    
}
}
#endif
