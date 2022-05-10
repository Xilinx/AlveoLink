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

#include <bitset>
#include <vector>
#include <string>
#include "xNativeFPGA.hpp"

namespace AlveoLink {
namespace network_dv {

constexpr size_t ClrCnts = 0x00;  // Write Clears All Counters -- Reads as DEADBEEF
constexpr size_t IdStatus = 0x04;  // bit[15:0]: port ID, bit[19:16]: lane status, 1-up, 0-down, bit[20]: port valid, 1-when all lanes are up
constexpr size_t CardMap = 0x08;  // Card Address Map -- 1 signifies that card is present & ready
constexpr size_t RSVD0 = 0x0C;    // Reserved0
constexpr size_t RxCntL0 = 0x10;  // Rx Count0 Low Word
constexpr size_t RxCntH0 = 0x14;  // Rx Count0 High Word
constexpr size_t RxCntL1 = 0x18;  // Rx Count1 Low Word
constexpr size_t RxCntH1 = 0x1C;  // Rx Count1 High Word
constexpr size_t RxCntL2 = 0x20;  // Rx Count2 Low Word
constexpr size_t RxCntH2 = 0x24;  // Rx Count2 High Word
constexpr size_t RxCntL3 = 0x28;  // Rx Count3 Low Word
constexpr size_t RxCntH3 = 0x2C;  // Rx Count3 High Word
constexpr size_t TxCntL0 = 0x30;  // Tx Count0 Low Word
constexpr size_t TxCntH0 = 0x34;  // Tx Count0 High Word
constexpr size_t TxCntL1 = 0x38;  // Tx Count1 Low Word
constexpr size_t TxCntH1 = 0x3C;  // Tx Count1 High Word
constexpr size_t TxCntL2 = 0x40;  // Tx Count2 Low Word
constexpr size_t TxCntH2 = 0x44;  // Tx Count2 High Word
constexpr size_t TxCntL3 = 0x48;  // Tx Count3 Low Word
constexpr size_t TxCntH3 = 0x4C;  // Tx Count3 High Word
constexpr size_t RETRYL0 = 0x50;  // Retry Count Lane 0
constexpr size_t RETRYL1 = 0x54;  // Retry Count Lane 1
constexpr size_t RETRYL2 = 0x58;  // Retry Count Lane 2
constexpr size_t RETRYL3 = 0x5C;  // Retry Count Lane 3
constexpr size_t LSTPKTL0W0 = 0x60;   // Last Rx Pkt L0 Wrd0
constexpr size_t LSTPKTL0W1 = 0x64;   // Last Rx Pkt L0 Wrd1
constexpr size_t LSTPKTL0W2 = 0x68;   // Last Rx Pkt L0 Wrd2
constexpr size_t LSTPKTL0W3 = 0x6C;   // Last Rx Pkt L0 Wrd3
constexpr size_t LSTPKTL1W0 = 0x70;   // Last Rx Pkt L1 Wrd0
constexpr size_t LSTPKTL1W1 = 0x74;   // Last Rx Pkt L1 Wrd1
constexpr size_t LSTPKTL1W2 = 0x78;   // Last Rx Pkt L1 Wrd2
constexpr size_t LSTPKTL1W3 = 0x7C;   // Last Rx Pkt L1 Wrd3
constexpr size_t LSTPKTL2W0 = 0x80;   // Last Rx Pkt L2 Wrd0
constexpr size_t LSTPKTL2W1 = 0x84;   // Last Rx Pkt L2 Wrd1
constexpr size_t LSTPKTL2W2 = 0x88;   // Last Rx Pkt L2 Wrd2
constexpr size_t LSTPKTL2W3 = 0x8C;   // Last Rx Pkt L2 Wrd3
constexpr size_t LSTPKTL3W0 = 0x90;   // Last Rx Pkt L3 Wrd0
constexpr size_t LSTPKTL3W1 = 0x94;   // Last Rx Pkt L3 Wrd1
constexpr size_t LSTPKTL3W2 = 0x98;   // Last Rx Pkt L3 Wrd2
constexpr size_t LSTPKTL3W3 = 0x9C;   // Last Rx Pkt L3 Wrd3
constexpr size_t LSTTXPKTL0W0 = 0xA0; //Last Tx Pkt L0 Wrd0
constexpr size_t LSTTXPKTL0W1 = 0xA4; //Last Tx Pkt L0 Wrd1
constexpr size_t LSTTXPKTL0W2 = 0xA8; //Last Tx Pkt L0 Wrd2
constexpr size_t LSTTXPKTL0W3 = 0xAC; //Last Tx Pkt L0 Wrd3
constexpr size_t LSTTXPKTL1W0 = 0xB0; //Last Tx Pkt L1 Wrd0
constexpr size_t LSTTXPKTL1W1 = 0xB4; //Last Tx Pkt L1 Wrd1
constexpr size_t LSTTXPKTL1W2 = 0xB8; //Last Tx Pkt L1 Wrd2
constexpr size_t LSTTXPKTL1W3 = 0xBC; //Last Tx Pkt L1 Wrd3
constexpr size_t LSTTXPKTL2W0 = 0xC0; //Last Tx Pkt L2 Wrd0
constexpr size_t LSTTXPKTL2W1 = 0xC4; //Last Tx Pkt L2 Wrd1
constexpr size_t LSTTXPKTL2W2 = 0xC8; //Last Tx Pkt L2 Wrd2
constexpr size_t LSTTXPKTL2W3 = 0xCC; //Last Tx Pkt L2 Wrd3
constexpr size_t LSTTXPKTL3W0 = 0xD0; //Last Tx Pkt L3 Wrd0
constexpr size_t LSTTXPKTL3W1 = 0xD4; //Last Tx Pkt L3 Wrd1
constexpr size_t LSTTXPKTL3W2 = 0xD8; //Last Tx Pkt L3 Wrd2
constexpr size_t LSTTXPKTL3W3 = 0xDC; //Last Tx Pkt L3 Wrd3



template <unsigned int t_MaxConnections, unsigned int t_DestBits>
class dvAdapter : public AlveoLink::common::IP {
    public:
        dvAdapter() {
            m_dests.reset();
            m_laneStatus.reset();
            m_linkUp = false;
            m_myId = 0;
            m_numDests = 0;
        }
        void initCU(const unsigned int p_id) {
            std::string l_cuName = "dv_adapter"+std::to_string(p_id)+":{dv_adapter" + std::to_string(p_id) + "}";
            getIP(l_cuName);
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
            int l_idStatus = readReg(IdStatus);
            m_myId = l_idStatus & 0x0ffff;
            int l_laneStatus = l_idStatus >> t_DestBits;
            for (auto i=0; i<4; ++i) {
                if (((l_laneStatus >> i) & 0x01) == 1) {
                    m_laneStatus[i] = true;
                }
                else {
                    m_laneStatus[i] = false;
                }
            }
            m_linkUp = m_laneStatus.all(); 
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
            writeReg(ClrCnts, 1);
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
};
}
}

#endif
