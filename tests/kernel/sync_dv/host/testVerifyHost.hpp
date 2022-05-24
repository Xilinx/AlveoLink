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

template <unsigned int t_NetDataBits>
class testVerifyHost {
   public:
    static const unsigned int t_NetDataBytes = t_NetDataBits / 8;

   public:
    testVerifyHost() {}
    void init(AlveoLink::common::FPGA* p_fpga) {
        m_card = p_fpga;
        m_krnlTestApp.fpga(m_card);
        m_krnlVerify.fpga(m_card);
    }
    void createCU(const unsigned int p_id = 0) {
        std::string l_cuName = "krnl_testApp:{krnl_testApp_" + std::to_string(p_id) + "}";
        m_krnlTestApp.createKernel(l_cuName);
        l_cuName = "krnl_verify:{krnl_verify_" + std::to_string(p_id) + "}";
        m_krnlVerify.createKernel(l_cuName);
    }
    void createRecBufs(const size_t p_dataBytes) {
        if (p_dataBytes % t_NetDataBytes != 0) {
            throw basicInvalidValue("kernel data buffer size must be multiple of " +
                                    std::to_string(t_NetDataBytes));
        }
        void* l_outDataBuf = m_krnlTestApp.createBO(1, p_dataBytes);
        m_krnlTestApp.setMemArg(1);
        m_krnlTestAppBufs.insert({1, l_outDataBuf});

        void* l_verifySentDestBuf = m_krnlVerify.createBO(0, p_dataBytes);
        void* l_verifyRecDestBuf = m_krnlVerify.createBO(1, p_dataBytes);
        m_krnlVerify.setMemArg(0);
        m_krnlVerify.setMemArg(1);
        m_krnlVerifyBufs.insert({0, l_verifySentDestBuf});
        m_krnlVerifyBufs.insert({1, l_verifyRecDestBuf});
    }

    void runCU(const unsigned int p_myId,
               const unsigned int p_numDevs,
               const unsigned int p_numPkts,
               const unsigned int p_batchPkts,
               const unsigned int p_timeOutCycles,
               const unsigned int p_waitCycles) {
        unsigned int l_argId = 4;
        m_krnlTestApp.setScalarArg(l_argId++, p_myId);
        m_krnlTestApp.setScalarArg(l_argId++, p_numDevs);
        m_krnlTestApp.setScalarArg(l_argId++, p_numPkts);
        m_krnlTestApp.setScalarArg(l_argId++, p_batchPkts);
        m_krnlTestApp.setScalarArg(l_argId++, p_timeOutCycles);
        m_krnlTestApp.setScalarArg(l_argId++, p_waitCycles);
        m_krnlTestApp.run();

        m_krnlVerify.setScalarArg(2, p_waitCycles);
        m_krnlVerify.run();
    }

    void* getRes() {
        m_krnlTestApp.getBO(1);
        void* l_outBuf = m_krnlTestAppBufs.find(1)->second;
        return l_outBuf;
    }

    void* getVerifySentDestBuf() {
        m_krnlVerify.getBO(0);
        void* l_buf = m_krnlVerifyBufs.find(0)->second;
        return l_buf;
    }
    void* getVerifyRecDestBuf() {
        m_krnlVerify.getBO(1);
        void* l_buf = m_krnlVerifyBufs.find(1)->second;
        return l_buf;
    }

    void* getTransBuf() {
        m_krnlTestApp.getBO(0);
        void* l_outBuf = m_krnlTestAppBufs.find(0)->second;
        return l_outBuf;
    }

    void* createTransBuf(const size_t p_dataBytes) {
        if (p_dataBytes % t_NetDataBytes != 0) {
            throw basicInvalidValue("kernel data buffer size must be multiple of " +
                                    std::to_string(t_NetDataBytes));
        }
        void* l_outBuf = m_krnlTestApp.createBO(0, p_dataBytes);
        m_krnlTestApp.setMemArg(0);
        m_krnlTestAppBufs.insert({0, l_outBuf});
        return l_outBuf;
    }

    void sendBO() { m_krnlTestApp.sendBO(0); }

   private:
    AlveoLink::common::FPGA* m_card;
    AlveoLink::common::KERNEL m_krnlTestApp;
    AlveoLink::common::KERNEL m_krnlVerify;
    std::map<const int, void*> m_krnlTestAppBufs;
    std::map<const int, void*> m_krnlVerifyBufs;
};

#endif
