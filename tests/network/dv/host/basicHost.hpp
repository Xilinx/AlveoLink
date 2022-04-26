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

#ifndef BASICHOST_HPP
#define BASICHOST_HPP

#include "xNativeFPGA.hpp"
#include "basicException.hpp"

template <unsigned int t_NetDataBits, unsigned int t_MaxConnections>
class basicHost {
   public:
    static const unsigned int t_NetDataBytes = t_NetDataBits / 8;

   public:
    basicHost() {}
    void init(AlveoLink::common::FPGA* p_fpga) {
        m_card = p_fpga;
        m_krnlDriver.fpga(m_card);
    }
    void createCU(const unsigned int p_id = 0) {
        std::string l_cuName = "krnl_driver:{krnl_driver_" + std::to_string(p_id) + "}";
        m_krnlDriver.createKernel(l_cuName);
    }
    void createTxBufs(const size_t p_numWords) {
        size_t l_bufBytes = p_numWords * t_NetDataBytes;
        void* l_buf = m_krnlDriver.createBO(1, l_bufBytes);
        m_krnlDriver.setMemArg(1);
        m_krnlDriverBufs.insert({1, l_buf});
        l_bufBytes = p_numWords * t_NetDataBytes / 8;
        l_buf = m_krnlDriver.createBO(2, l_bufBytes);
        m_krnlDriver.setMemArg(2);
        m_krnlDriverBufs.insert({2, l_buf});
    }
    void createRxBufs(const size_t p_numWords) {
        assert((p_numWords % 2) == 0);
        size_t l_bufBytes = p_numWords * t_NetDataBytes;
        void* l_buf = m_krnlDriver.createBO(5, l_dataBytes);
        m_krnlDriver.setMemArg(5);
        m_krnlDriverBufs.insert({5, l_buf});
        l_bufBytes = p_numWords * t_NetDataBytes/8;
        l_buf = m_krnlDriver.createBO(6, l_bufBytes);
        m_krnlDriver.setMemArg(6);
        m_krnlDriverBufs.insert({6, l_buf});
        l_bufBytes = p_numWords * 4 / 8;
        l_buf = m_krnlDriver.createBO(7, l_bufBytes);
        m_krnlDriver.setMemArg(7);
        m_krnlDriverBufs.insert({7, l_buf});
    }
    void sendBO() { 
        m_krnlDriver.sendBO(1); 
        m_krnlDriver.sendBO(2); 
    }

    void runCU(const unsigned int p_numWords){
        unsigned int l_argId = 0;
        m_krnlDriver.setScalarArg(l_argId++, p_numWords);
        m_krnlDriver.run();
    }
    
    void* getTxDataPtr() {
        void* l_outBuf = m_krnlDriverBufs.find(1)->second;
        return l_outBuf;
    }

    void* getTxKeepPtr() {
        void* l_outBuf = m_krnlDriverBufs.find(2)->second;
        return l_outBuf;
    }

    void* getRecData() {
        m_krnlDriver.getBO(5);
        void* l_outBuf = m_krnlDriverBufs.find(5)->second;
        return l_outBuf;
    }
    void* getRecKeep() {
        m_krnlDriver.getBO(6);
        void* l_outBuf = m_krnlDriverBufs.find(6)->second;
        return l_outBuf;
    }
    void* getRecDest() {
        m_krnlDriver.getBO(7);
        void* l_outBuf = m_krnlDriverBufs.find(7)->second;
        return l_outBuf;
    }



   private:
    AlveoLink::common::FPGA* m_card;
    AlveoLink::common::KERNEL m_krnlDriver;
    std::map<const int, void*> m_krnlDriverBufs;
};

#endif
