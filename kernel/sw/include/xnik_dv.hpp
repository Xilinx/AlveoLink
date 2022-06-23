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

#ifndef XNIKDV_HPP
#define XNIKDV_HPP

#include <cassert>
#include <vector>
#include "xNativeFPGA.hpp"

namespace AlveoLink {
namespace kernel {

template<unsigned int t_NumDevs> 
class xnikTX {
    public:
        xnikTX() {m_myId = 0;}
        
        void init(AlveoLink::common::FPGA* p_fpga) {
            m_card = p_fpga;
            m_krnlTx.fpga(m_card);
        }
        void createCU(const unsigned int p_id = 0) {
            assert(p_id < t_NumDevs);
            m_myId = p_id;
            std::string l_cuName = "krnl_xnikSyncTX:{krnl_xnikSyncTX_" + std::to_string(p_id) + "}";
            m_krnlTx.createKernel(l_cuName);
        }

        void runCU() {
            m_krnlTx.run();
        }

        void* createStatsBuf(const size_t p_dataBytes) {
            void* l_statsBuf = m_krnlTx.createBO(4, p_dataBytes);
            m_krnlTx.setMemArg(4);
            m_krnlTxBufs.insert({4, l_statsBuf});
            return l_statsBuf;
        }

        void sendBO() { m_krnlTx.sendBO(4); }
        void syncBO() { m_krnlTx.syncBO(4); }
        void finish() { m_krnlTx.wait(); }

    private:
        uint16_t m_myId;
        AlveoLink::common::FPGA* m_card;
        AlveoLink::common::KERNEL m_krnlTx;
        std::map<const int, void*> m_krnlTxBufs;
};
 
template<unsigned int t_NumDevs> 
class xnikRX {
    public:
        xnikRX() {m_myId = 0;}
        
        void init(AlveoLink::common::FPGA* p_fpga) {
            m_card = p_fpga;
            m_krnlRx.fpga(m_card);
        }
        void createCU(const unsigned int p_id = 0) {
            assert(p_id < t_NumDevs);
            m_myId = p_id;
            std::string l_cuName = "krnl_xnikSyncRX:{krnl_xnikSyncRX_" + std::to_string(p_id) + "}";
            m_krnlRx.createKernel(l_cuName);
        }

        void runCU() {
            m_krnlRx.run();
        }

        void* createStatsBuf(const size_t p_dataBytes) {
            void* l_statsBuf = m_krnlRx.createBO(4, p_dataBytes);
            m_krnlRx.setMemArg(4);
            m_krnlRxBufs.insert({4, l_statsBuf});
            return l_statsBuf;
        }

        void sendBO() { m_krnlRx.sendBO(4); }
        void syncBO() { m_krnlRx.syncBO(4); }
        void finish() { m_krnlRx.wait(); }

    private:
        uint16_t m_myId;
        AlveoLink::common::FPGA* m_card;
        AlveoLink::common::KERNEL m_krnlRx;
        std::map<const int, void*> m_krnlRxBufs;
};
 
}
}
#endif
