
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

#ifndef DVADAPTER_NETLAYER_HPP
#define DVADAPTER_NETLAYER_HPP

#include "dvAdapter.hpp"

namespace AlveoLink {
namespace network_dv {

template <unsigned int t_numInfs,
          unsigned int t_MaxConnections, 
          unsigned int t_DestBits> 
class dvNetLayer {
    public:
        dvNetLayer() {
            m_numDests = 0;
        }
        void init(AlveoLink::common::FPGA* p_fpga = nullptr) {
            m_linkUp.reset();
            for (auto i=0; i<t_numInfs; ++i) {
                m_adapters[i].fpga(p_fpga);
                m_adapters[i].initCU(i);
                m_adapters[i].updateStatus();
                m_linkUp[i] = m_adapters[i].isLinkUp();
                m_ids[i] = m_adapters[i].getMyId();
            }
            m_numDests = m_adapters[0].getNumDests();
        }
        void updateStatus() {
            m_linkUp.reset();
            for (auto i=0; i<t_numInfs; ++i) {
                m_adapters[i].updateStatus();
                m_linkUp[i] = m_adapters[i].isLinkUp();
                m_ids[i] = m_adapters[i].getMyId();
            }
            m_numDests = m_adapters[0].getNumDests();
        }

        void clearCounters() {
            for (auto i=0; i<t_numInfs; ++i) {
                m_adapters[i].clearCounters();
            }
        }

        std::vector<unsigned int> getClrCnts() {
            std::vector<unsigned int> l_res(t_numInfs); 
            for (auto i=0; i<t_numInfs; ++i) {
                l_res[i] = m_adapters[i].getClrCnts();
            }
            return l_res;
        }

        std::vector<uint64_t> getLaneRxPktsCnt() {
            std::vector<uint64_t> l_res(t_numInfs * 4);
            for (auto i=0; i<t_numInfs; ++i) {
                std::vector<uint64_t> l_pkts = m_adapters[i].getLaneRxCounter();
                for (auto j=0; j<4; ++j) {
                    l_res[i*4+j] = l_pkts[j];
                }
            }
            return l_res;
        }

        std::vector<uint64_t> getLaneTxPktsCnt() {
            std::vector<uint64_t> l_res(t_numInfs * 4);
            for (auto i=0; i<t_numInfs; ++i) {
                std::vector<uint64_t> l_pkts = m_adapters[i].getLaneTxCounter();
                for (auto j=0; j<4; ++j) {
                    l_res[i*4+j] = l_pkts[j];
                }
            }
            return l_res;
        }

        std::vector<uint32_t> getLastRxPkt(const unsigned int p_portIndex, const unsigned int p_laneIndex) {
           return m_adapters[p_portIndex].getLastRxPkt(p_laneIndex); 
        }
        uint16_t getNumDests() {
            return m_numDests;
        }

        uint16_t* getIds() {
            return m_ids;
        }

        std::bitset<t_numInfs> getLinkStatus() {
            return m_linkUp;
        }
        
    private:
        dvAdapter<t_MaxConnections, t_DestBits> m_adapters[t_numInfs];
        uint16_t m_numDests;
        std::bitset<t_numInfs> m_linkUp;
        uint16_t m_ids[t_numInfs];
};

}
}
#endif
