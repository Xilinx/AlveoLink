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

#ifndef XILINX_APPS_NHOPKERNEL_HPP
#define XILINX_APPS_NHOPKERNEL_HPP

#include "xNativeFPGA.hpp"

namespace xilinx_apps {
namespace nhop {

template <unsigned int t_NumPUs>
class nHopKernel {
   public:
    nHopKernel() {}
    void init(AlveoLink::common::FPGA* p_fpga) {
        m_card = p_fpga;
        m_krnlNhop.fpga(m_card);
    }
    void createKernel(const int p_cuId) {
        std::string l_cuName = "nHop_kernel:{nHop_kernel" + std::to_string(p_cuId) + "}";
        m_krnlNhop.createKernel(l_cuName);
    }
    void createBuf(const int p_argIdx, const size_t p_bytes, void* p_hostPtr) {
        m_krnlNhop.createBOfromHostPtr(p_argIdx, p_bytes, p_hostPtr);
        m_krnlNhop.setMemArg(p_argIdx);
        m_krnlNhopBufs.insert({p_argIdx, p_hostPtr});
    }
    void createCsrBufs(const size_t p_offsetBufSizes[t_NumPUs],
                       const size_t p_indexBufSizes[t_NumPUs],
                       void* p_offsetPtrs[],
                       void* p_indexPtrs[]) {
        int l_argIdx = 6;
        for (unsigned int i = 0; i < t_NumPUs; ++i) {
            m_krnlNhop.createBOfromHostPtr(l_argIdx + i * 2, p_offsetBufSizes[i], p_offsetPtrs[i]);
            m_krnlNhop.setMemArg(l_argIdx + i * 2);
            m_krnlNhopBufs.insert({l_argIdx + i * 2, p_offsetPtrs[i]});
            m_krnlNhop.createBOfromHostPtr(l_argIdx + i * 2 + 1, p_indexBufSizes[i], p_indexPtrs[i]);
            m_krnlNhop.setMemArg(l_argIdx + i * 2 + 1);
            m_krnlNhopBufs.insert({l_argIdx + i * 2 + 1, p_indexPtrs[i]});
        }
    }
    void createFifoBuf(const size_t p_bytes, void* p_hostPtr) {
        m_krnlNhop.createBOfromHostPtr(16, p_bytes, p_hostPtr);
        m_krnlNhop.setMemArg(16, 17);
        m_krnlNhopBufs.insert({16, p_hostPtr});
    }
    void sendBOs() {
        m_krnlNhop.sendBO(0);                         // send config
        m_krnlNhop.sendBO(1);                         // send pair
        m_krnlNhop.sendBO(3);                         // send offsetTable
        m_krnlNhop.sendBO(4);                         // send indexTable
        m_krnlNhop.sendBO(5);                         // send cardTable
        for (unsigned int i = 0; i < t_NumPUs; ++i) { // send offset, index
            m_krnlNhop.sendBO(6 + i * 2);
            m_krnlNhop.sendBO(6 + i * 2 + 1);
        }
        m_krnlNhop.sendBO(6 + t_NumPUs * 2);
        m_krnlNhop.sendBO(6 + t_NumPUs * 2 + 1);
        m_krnlNhop.sendBO(6 + t_NumPUs * 2 + 2);
        m_krnlNhop.sendBO(6 + t_NumPUs * 2 + 4);
        m_krnlNhop.sendBO(6 + t_NumPUs * 2 + 5);
    }
    void run() { m_krnlNhop.run(); }
    void getBOs() {
        m_krnlNhop.getBO(16); 
        int l_numOutIdx = 6 + t_NumPUs * 2 + 4;
        int l_buffLocalIdx = l_numOutIdx++;
        m_krnlNhop.getBO(l_numOutIdx);
        m_krnlNhop.getBO(l_buffLocalIdx);
    }

    void syncBOs() {
        m_krnlNhop.syncBO(16); // sync memFIFO that stores network data
        int l_numOutIdx = 6 + t_NumPUs * 2 + 4;
        int l_buffLocalIdx = l_numOutIdx++;
        m_krnlNhop.syncBO(l_numOutIdx);
        m_krnlNhop.syncBO(l_buffLocalIdx);
    }
   private:
    AlveoLink::common::FPGA* m_card;
    AlveoLink::common::KERNEL m_krnlNhop;
    std::map<const int, void*> m_krnlNhopBufs;
};
}
}
#endif
