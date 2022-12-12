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

#ifndef GENCOLHOST_HPP
#define GENCOLHOST_HPP

#include "xNativeFPGA.hpp"

template <unsigned int t_NumPorts>
class genColHost {
    public:
        genColHost() {}
        void init(AlveoLink::common::FPGA* p_fpga) {
            m_card = p_fpga;
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                m_krnl[i].fpga(m_card);
                std::string l_cuName = "generator_collector:{generator_collector_" + std::to_string(i) + "}";
                m_krnl[i].createKernel(l_cuName);
            }
        }
        void runCollectors(unsigned int* p_genCol,
                   unsigned int* p_id,
                   unsigned int* p_numPkts) {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                m_krnl[i].setScalarArg(0, p_genCol[i]);
                m_krnl[i].setScalarArg(1, p_id[i]);
                m_krnl[i].setScalarArg(2, p_numPkts[i]);
                m_krnl[i].run();
            }
        }
        void runGenerators(unsigned int* p_genCol,
                   unsigned int* p_id,
                   unsigned int* p_numPkts) {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                m_krnl[i].setScalarArg(0, p_genCol[i]);
                m_krnl[i].setScalarArg(1, p_id[i]);
                m_krnl[i].setScalarArg(2, p_numPkts[i]);
                m_krnl[i].run();
            }
        }

        void finish() {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                m_krnl[i].wait();
            }
        }
        
        void getNumBytes(unsigned int p_numBytes[t_NumPorts]) {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                p_numBytes[i] = m_krnl[i].readReg(3);
            }
        }

        void getNumPkts(unsigned int p_numPkts[t_NumPorts]) {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                p_numPkts[i] = m_krnl[i].readReg(4);
            }
        }
       
        void getErrors(unsigned int p_errors[t_NumPorts]) {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                p_errors[i] = m_krnl[i].readReg(5);
            }   
        } 
    private:
        AlveoLink::common::FPGA* m_card;
        AlveoLink::common::KERNEL m_krnl[t_NumPorts];
};

#endif
