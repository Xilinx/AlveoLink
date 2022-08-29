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

#ifndef DVADAPTER_HPP
#define DVADAPTER_HPP

#include <cassert>
#include <bitset>
#include <vector>
#include <string>
#include "xNativeFPGA.hpp"

namespace AlveoLink {
namespace network_dv {

constexpr size_t ClrCnts = 0x08;  // Write Clears All Counters -- Reads as DEADBEEF
constexpr size_t IdStatus = 0x0C;  // bit[15:0]: port ID, bit[19:16]: lane status, 1-up, 0-down, bit[20]: port valid, 1-when all lanes are up
constexpr size_t CardID01 = 0x10;
constexpr size_t CardID23 = 0x14;

constexpr size_t CardMap = 0x18;  // Card Address Map -- 1 signifies that card is present & ready
//constexpr size_t TranStatus = 0x24;
constexpr size_t RSVD0 = 0x00;    // Reserved0
constexpr size_t RxCntL0 = 0x40;  // Rx Count0 Low Word
constexpr size_t RxCntH0 = 0x44;  // Rx Count0 High Word
constexpr size_t RxCntL1 = 0x48;  // Rx Count1 Low Word
constexpr size_t RxCntH1 = 0x4C;  // Rx Count1 High Word
constexpr size_t RxCntL2 = 0x50;  // Rx Count2 Low Word
constexpr size_t RxCntH2 = 0x54;  // Rx Count2 High Word
constexpr size_t RxCntL3 = 0x58;  // Rx Count3 Low Word
constexpr size_t RxCntH3 = 0x5C;  // Rx Count3 High Word
constexpr size_t TxCntL0 = 0x60;  // Tx Count0 Low Word
constexpr size_t TxCntH0 = 0x64;  // Tx Count0 High Word
constexpr size_t TxCntL1 = 0x68;  // Tx Count1 Low Word
constexpr size_t TxCntH1 = 0x6C;  // Tx Count1 High Word
constexpr size_t TxCntL2 = 0x70;  // Tx Count2 Low Word
constexpr size_t TxCntH2 = 0x74;  // Tx Count2 High Word
constexpr size_t TxCntL3 = 0x78;  // Tx Count3 Low Word
constexpr size_t TxCntH3 = 0x7C;  // Tx Count3 High Word
constexpr size_t RETRYL0 = 0x30;  // Retry Count Lane 0
constexpr size_t RETRYL1 = 0x34;  // Retry Count Lane 1
constexpr size_t RETRYL2 = 0x38;  // Retry Count Lane 2
constexpr size_t RETRYL3 = 0x3C;  // Retry Count Lane 3
constexpr size_t LSTPKTL0W0 = 0x80;   // Last Rx Pkt L0 Wrd0
constexpr size_t LSTPKTL0W1 = 0x84;   // Last Rx Pkt L0 Wrd1
constexpr size_t LSTPKTL0W2 = 0x88;   // Last Rx Pkt L0 Wrd2
constexpr size_t LSTPKTL0W3 = 0x8C;   // Last Rx Pkt L0 Wrd3
constexpr size_t LSTPKTL1W0 = 0x90;   // Last Rx Pkt L1 Wrd0
constexpr size_t LSTPKTL1W1 = 0x94;   // Last Rx Pkt L1 Wrd1
constexpr size_t LSTPKTL1W2 = 0x98;   // Last Rx Pkt L1 Wrd2
constexpr size_t LSTPKTL1W3 = 0x9C;   // Last Rx Pkt L1 Wrd3
constexpr size_t LSTPKTL2W0 = 0xA0;   // Last Rx Pkt L2 Wrd0
constexpr size_t LSTPKTL2W1 = 0xA4;   // Last Rx Pkt L2 Wrd1
constexpr size_t LSTPKTL2W2 = 0xA8;   // Last Rx Pkt L2 Wrd2
constexpr size_t LSTPKTL2W3 = 0xAC;   // Last Rx Pkt L2 Wrd3
constexpr size_t LSTPKTL3W0 = 0xB0;   // Last Rx Pkt L3 Wrd0
constexpr size_t LSTPKTL3W1 = 0xB4;   // Last Rx Pkt L3 Wrd1
constexpr size_t LSTPKTL3W2 = 0xB8;   // Last Rx Pkt L3 Wrd2
constexpr size_t LSTPKTL3W3 = 0xBC;   // Last Rx Pkt L3 Wrd3
constexpr size_t LSTTXPKTL0W0 = 0xC0; //Last Tx Pkt L0 Wrd0
constexpr size_t LSTTXPKTL0W1 = 0xC4; //Last Tx Pkt L0 Wrd1
constexpr size_t LSTTXPKTL0W2 = 0xC8; //Last Tx Pkt L0 Wrd2
constexpr size_t LSTTXPKTL0W3 = 0xCC; //Last Tx Pkt L0 Wrd3
constexpr size_t LSTTXPKTL1W0 = 0xD0; //Last Tx Pkt L1 Wrd0
constexpr size_t LSTTXPKTL1W1 = 0xD4; //Last Tx Pkt L1 Wrd1
constexpr size_t LSTTXPKTL1W2 = 0xD8; //Last Tx Pkt L1 Wrd2
constexpr size_t LSTTXPKTL1W3 = 0xDC; //Last Tx Pkt L1 Wrd3
constexpr size_t LSTTXPKTL2W0 = 0xE0; //Last Tx Pkt L2 Wrd0
constexpr size_t LSTTXPKTL2W1 = 0xE4; //Last Tx Pkt L2 Wrd1
constexpr size_t LSTTXPKTL2W2 = 0xE8; //Last Tx Pkt L2 Wrd2
constexpr size_t LSTTXPKTL2W3 = 0xEC; //Last Tx Pkt L2 Wrd3
constexpr size_t LSTTXPKTL3W0 = 0xF0; //Last Tx Pkt L3 Wrd0
constexpr size_t LSTTXPKTL3W1 = 0xF4; //Last Tx Pkt L3 Wrd1
constexpr size_t LSTTXPKTL3W2 = 0xF8; //Last Tx Pkt L3 Wrd2
constexpr size_t LSTTXPKTL3W3 = 0xFC; //Last Tx Pkt L3 Wrd3



template <unsigned int t_MaxConnections, unsigned int t_DestBits>
class dvAdapter : public AlveoLink::common::IP {
    public:
        dvAdapter() {
            m_dests.reset();
            m_laneStatus.reset();
            m_linkUp = false;
            m_myId = 0;
            m_numDests = 0;
            for (auto i=0; i<4; ++i) {
                m_laneIds[i] = 0;
            }
        }
        void initCU(const unsigned int p_id) {
#if TM
            if (p_id == 1) {
                std::string l_cuName = "dva1_1src:{dv_adapter" + std::to_string(p_id) + "}";
                getIP(l_cuName);
            }
            else if (p_id == 0) {
                std::string l_cuName = "dv_adapter25_"+std::to_string(p_id)+":{dv_adapter" + std::to_string(p_id) + "}";
                getIP(l_cuName);
            }
#else 
            std::string l_cuName = "dv_adapter25_"+std::to_string(p_id)+":{dv_adapter" + std::to_string(p_id) + "}";
            getIP(l_cuName);
#endif
        }
        void updateStatus() {//get dests map, my_id, and laneStatus
            int l_portMap = readReg(CardMap);
            for (auto i=0; i<t_MaxConnections; ++i) {
                int l_valid = (l_portMap >> i) & 0x01;
                if (l_valid == 1) {
                    m_numDests++;
                    m_dests[i] = true;
                }
                else {
                    m_dests[i] = false;
                }
            }
//            int l_tranStatus = readReg(TranStatus);
//            std::cout <<"INFO: hex value of reg 0x24 is: 0x" << std::hex << l_tranStatus << std::endl;

            int l_status = readReg(IdStatus);
            std::cout <<"INFO: hex value of reg 0x0c is: 0x" << std::hex << l_status << std::endl;
            int l_laneStatus = l_status;
            for (auto i=0; i<4; ++i) {
                if (((l_laneStatus >> i) & 0x01) == 1) {
                    m_laneStatus[i] = true;
                }
                else {
                    m_laneStatus[i] = false;
                }
            }
            m_linkUp = m_laneStatus.all(); 
            int l_ids01 = readReg(CardID01);
            int l_ids23 = readReg(CardID23);
            std::cout <<"INFO: hex value of reg 0x10 is: 0x" << std::hex << l_ids01 << std::endl;
            std::cout <<"INFO: hex value of reg 0x14 is: 0x" << std::hex << l_ids23 << std::endl;
            std::cout << std::dec;
            m_laneIds[0] = l_ids01 & 0x0ff;
            m_laneIds[1] = (l_ids01 >> 16) & 0x0ff;
            m_laneIds[2] = l_ids23 & 0x0ff;
            m_laneIds[3] = (l_ids23 >> 16) & 0x0ff;
            assert(m_laneIds[0] == m_laneIds[1]);
            assert(m_laneIds[1] == m_laneIds[2]);
            assert(m_laneIds[2] == m_laneIds[3]);
            m_myId = m_laneIds[0];
        }
        
        std::bitset<t_MaxConnections> getDestMap() {
            return m_dests;
        }
        std::bitset<4> getLaneStatus() {
            return m_laneStatus;
        }
        bool isLinkUp() {
            return m_linkUp;
        }
        uint16_t getMyId() {
            return m_myId;
        }
        uint16_t getNumDests() {
            return m_numDests;
        }

        void clearCounters() {
            writeReg(ClrCnts, 2);
        }
        unsigned int getClrCnts() {
            unsigned int l_res = readReg(ClrCnts);
            return l_res;
        }
        
        std::vector<uint64_t> getLaneRxCounter() {
            std::vector<uint64_t> l_res(4);
            for (auto i=0; i<4; ++i) {
                uint32_t l_highWord = readReg(RxCntH0+8*i);
                uint32_t l_lowWord = readReg(RxCntL0+8*i);
                l_res[i] = l_highWord;
                l_res[i] = l_res[i] << 32;
                l_res[i] += l_lowWord;
            }
            return l_res;
        }
        std::vector<uint64_t> getLaneTxCounter() {
            std::vector<uint64_t> l_res(4);
            for (auto i=0; i<4; ++i) {
                uint32_t l_highWord = readReg(TxCntH0+8*i);
                uint32_t l_lowWord = readReg(TxCntL0+8*i);
                l_res[i] = l_highWord;
                l_res[i] = l_res[i] << 32;
                l_res[i] += l_lowWord;
            }
            return l_res;
        }
        std::vector<uint32_t> getLastRxPkt(const unsigned int p_laneId) {
            std::vector<uint32_t> l_pkts(4);
            l_pkts[0] = readReg(LSTPKTL0W0+p_laneId*16);
            l_pkts[1] = readReg(LSTPKTL0W1+p_laneId*16);
            l_pkts[2] = readReg(LSTPKTL0W2+p_laneId*16);
            l_pkts[3] = readReg(LSTPKTL0W3+p_laneId*16);
            return l_pkts;           
        }
        std::vector<uint32_t> getLastTxPkt(const unsigned int p_laneId) {
            std::vector<uint32_t> l_pkts(4);
            l_pkts[0] = readReg(LSTTXPKTL0W0+p_laneId*16);
            l_pkts[1] = readReg(LSTTXPKTL0W1+p_laneId*16);
            l_pkts[2] = readReg(LSTTXPKTL0W2+p_laneId*16);
            l_pkts[3] = readReg(LSTTXPKTL0W3+p_laneId*16);
            return l_pkts;           
        }


    private:
        std::bitset<4> m_laneStatus;
        std::bitset<t_MaxConnections> m_dests;
        bool m_linkUp;
        uint16_t m_numDests;
        uint16_t m_myId;
        uint16_t m_laneIds[4];
};
}
}

#endif
