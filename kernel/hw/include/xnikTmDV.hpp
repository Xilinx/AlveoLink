
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
                  unsigned int t_DestBits>
        class xnikTraffic_Manager {
            public:
                xnikTraffic_Manager() {
                    m_state = TM_STATE::tm_idle;
                    m_myId = 0;
                    m_tmId = 0;
                    m_numDevs = 0;
                    m_waitCycles = 0;
                    m_maxAddr = 0;
                    m_rdAddr = 0;
                    m_wrAddr = 0;
                }
                void incrWrAddr(){
    #pragma HLS INLINE
                    if (m_wrAddr == m_maxAddr) {
                        m_wrAddr = 0;
                    }
                    else {
                        m_wrAddr++;
                    }
                }
                
                void incrRdAddr(){
    #pragma HLS INLINE
                    if (m_rdAddr == m_maxAddr) {
                        m_rdAddr = 0;
                    }
                    else {
                        m_rdAddr++;
                    }
                }

                void storePkts(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                               ap_uint<t_NetDataBits>* p_memPtr) {
#pragma HLS INLINE
                    uint32_t l_waitCycles = m_waitCycles;
LOOP_TM_RECEIVING:
                    while (l_waitCycles != 0){
#pragma HLS PIPELINE II=1
                        if (!p_inStr.empty()) {
                            ap_uint<t_NetDataBits> l_val = p_inStr.read();
                            p_memPtr[m_wrAddr] = l_val;
                            incrWrAddr();
                            l_waitCycles = m_waitCycles;
                        }
                        else {
                            l_waitCycles--;
                        }
                    }
                }

                void loadPkts(hls::stream<ap_uint<t_NetDataBits> >& p_inStr,
                              ap_uint<t_NetDataBits>* p_memPtr,
                              hls::stream<ap_uint<t_NetDataBits> >& p_outStr) {
#pragma HLS INLINE
    LOOP_TM_SENDING:
                    while ((m_rdAddr != m_wrAddr) && p_inStr.empty()) {
            #pragma HLS PIPELINE II=1
                        ap_uint<t_NetDataBits> l_rdVal = p_memPtr[m_rdAddr];
                        p_outStr.write(l_rdVal);
                        incrRdAddr();
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
                    while (!l_procExit || (m_state != TM_STATE::tm_idle)) {
//#pragma HLS PIPELINE off
                        DvHopCtrlPkt<t_NetDataBits, t_DestBits> l_ctrlPkt;
                        switch(m_state) {
                            case TM_STATE::tm_idle:
                                l_ctrlPkt.setSrcId(m_myId);
                                l_ctrlPkt.setNumDevs(m_numDevs);
                                l_ctrlPkt.setTmId(m_tmId);
                                l_ctrlPkt.setType(PKT_TYPE::config);
                                l_ctrlPkt.write(p_outStr);
                                m_state = TM_STATE::tm_receiving;
                            break;

                            case TM_STATE::tm_receiving:
                                storePkts(p_inStr, p_memPtr);
                                m_state = TM_STATE::tm_sending;
                            break;

                            case TM_STATE::tm_sending:
                                loadPkts(p_inStr, p_memPtr, p_outStr);
                                if (!p_inStr.empty()) {
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
                                    p_memPtr[m_wrAddr] = l_ctrlPkt.getCtrlPkt();
                                    incrWrAddr();
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
            }

        private:
            TM_STATE m_state;
            ap_uint<t_DestBits> m_myId, m_tmId, m_numDevs;
            uint32_t m_waitCycles;
            uint32_t m_maxAddr;
            uint32_t m_rdAddr;
            uint32_t m_wrAddr;
    };
    
}
}
#endif
