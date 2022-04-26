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
constexpr size_t LanesUp = 0x04;  // Lanes Up -- Bits[3:0] signify which lanes are up
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


template <unsigned int t_MaxConnections>
class dvAdapter : public AlveoLink::common::IP {
    public:
        dvAdapter() {
            m_dests.reset();
            m_myId = 0;
        }
        void initCU(const unsigned int p_id) {
            std::string l_cuName = "dv_adapter0:{dv_adapter" + std::to_string(p_id) + "}";
            getIP(l_cuName);
        }
        bool isInfUp() {
            int l_status = readReg(CardMap);
            if (l_status == 1) {
                return true;
            }
            else {
                return false;
            }
        }
        std::bitset<4> getLaneStatus() {
            std::bitset<4> l_status;
            l_status.reset();
            uint8_t l_laneStatus = readReg(LanesUp);
            l_status[0] = ((l_laneStatus & 0x01) == 1);
            l_status[1] = (((l_laneStatus >> 1) & 0x01) == 1);
            l_status[2] = (((l_laneStatus >> 2) & 0x01) == 1);
            l_status[3] = (((l_laneStatus >> 3) & 0x01) == 1);
            return l_status;
        }
        void clearCounters() {
            writeReg(ClrCnts, 1);
        }
        unsigned int getClrCnts() {
            unsigned int l_res = readReg(ClrCnts);
            return l_res;
        }
        
        uint64_t getLaneRxCounter(const unsigned int p_laneId) {
            uint32_t l_highWord = readReg(RxCntH0+8*p_laneId);
            uint32_t l_lowWord = readReg(RxCntL0+8*p_laneId);
            uint64_t l_res = (l_highWord << 32) + l_lowWord;
            return l_res;
        }
        uint64_t getLaneTxCounter(const unsigned int p_laneId) {
            uint32_t l_highWord = readReg(TxCntH0+8*p_laneId);
            uint32_t l_lowWord = readReg(TxCntL0+8*p_laneId);
            uint64_t l_res = (l_highWord << 32) + l_lowWord;
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


    private:
        std::bitset<t_MaxConnections> m_dests;
        unsigned int m_myId;
};
}
}

#endif
