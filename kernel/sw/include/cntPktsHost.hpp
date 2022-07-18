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

#ifndef CNTPKTSHOST_HPP
#define CNTPKTHOST_HPP

#include "xNativeFPGA.hpp"
//#include "basicException.hpp"

template <unsigned int t_numNetPktsCnts>
class cntPktsHost {
   public:
    cntPktsHost() {}
    void init(AlveoLink::common::FPGA* p_fpga) {
        m_card = p_fpga;
        for (auto i=0; i<t_numNetPktsCnts; ++i) {
            m_krnlCntNetPkts[i].fpga(m_card);
        }
        m_krnlCntUsrPkts.fpga(m_card);
    }
    void createCU(const unsigned int p_krnCntNetPktsId = 0, const unsigned int p_krnCntUsrPktsId=0) {
        for (auto i=0; i<t_numNetPktsCnts; ++i) {
            std::string l_cuName = "krnl_cntNetPkts:{krnl_cntNetPkts_" + std::to_string(p_krnCntNetPktsId+i) + "}";
            m_krnlCntNetPkts[i].createKernel(l_cuName);
        }
        std::string l_cuName = "krnl_cntUsrPkts:{krnl_cntUsrPkts_" + std::to_string(p_krnCntUsrPktsId) + "}";
        m_krnlCntUsrPkts.createKernel(l_cuName);
    }

    void runCU(unsigned int p_numNetPkts[t_numNetPktsCnts], unsigned int p_numRxUsrPkts) {
        for (auto i=0; i<t_numNetPktsCnts; ++i) {
            m_krnlCntNetPkts[i].setScalarArg(0,  p_numNetPkts[i]);
            m_krnlCntNetPkts[i].run();
        }
        m_krnlCntUsrPkts.setScalarArg(0, p_numRxUsrPkts);
        m_krnlCntUsrPkts.run();
    }

    void finish() {
        for (auto i=0; i<t_numNetPktsCnts; ++i) {
            m_krnlCntNetPkts[i].wait();
        }
        m_krnlCntUsrPkts.wait();
    }

    void getNumTxNetPkts(const unsigned int p_krnCntNetPktsId, unsigned int& p_numPkts) {
        p_numPkts = m_krnlCntNetPkts[p_krnCntNetPktsId].readReg(0);
    }

    void getNumRxNetPkts(const unsigned int p_krnCntNetPktsId, unsigned int& p_numPkts) {
        p_numPkts = m_krnlCntNetPkts[p_krnCntNetPktsId+1].readReg(0);
    }
    
    void getNumRxUsrPkts(unsigned int& p_numPkts) {
        p_numPkts = m_krnlCntUsrPkts.readReg(0);
    }
    
   private:
    AlveoLink::common::FPGA* m_card;
    AlveoLink::common::KERNEL m_krnlCntNetPkts[t_numNetPktsCnts];
    AlveoLink::common::KERNEL m_krnlCntUsrPkts;
};
#endif
