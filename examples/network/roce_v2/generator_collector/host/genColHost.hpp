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

enum GEN_COLL_TYPE{
    collector=0,
    generator=1
};
template <unsigned int t_NumPorts>
class genColHost {
    public:
        genColHost() {

            m_card=nullptr;
        }
        void init(AlveoLink::common::FPGA* p_fpga,
            GEN_COLL_TYPE* p_genCol,
            unsigned int p_id,
            unsigned long long p_numPkts,
            unsigned int dim) {
            m_card = p_fpga;
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                m_krnl[i].fpga(m_card);
                std::string l_cuName = "generator_collector:{generator_collector_" + std::to_string(i) + "}";
                m_krnl[i].getIP(l_cuName);
                m_krnl[i].writeReg(gen_col_i_offset, (p_genCol[i]==collector?0:1));
                m_krnl[i].writeReg(id_i_offset, p_id);
                m_krnl[i].writeReg(packets_to_send_i_offset_1, p_numPkts);
                m_krnl[i].writeReg(packets_to_send_i_offset_2, p_numPkts>>32);
                m_krnl[i].writeReg(dim_i_offset, dim);

            }
        }
        void runCollectors() {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                if(getType(i)==collector){
                    m_krnl[i].writeReg(start_i_offset, 1);
                }
            }
        }
        void runGenerators() {
            for (unsigned int i=0; i<t_NumPorts; ++i) {
                if(getType(i)==generator){
                    m_krnl[i].writeReg(start_i_offset, 1);
                }
            }
        }
        

        int stopIP(int port,GEN_COLL_TYPE type){
            if(m_card != nullptr && getType(port)==type){
                m_krnl[port].writeReg(start_i_offset, 0);
                for(int i=0;m_krnl[port].readReg(pkt_cnt_o_offset_1) && i<20;i++){
                }
                if(m_krnl[port].readReg(pkt_cnt_o_offset_1)){
                    return -1;
                }
            }
            return 0;
        }

        unsigned long long getCurrPkts(int port) {
            unsigned long long temp_cnt;
            temp_cnt =  m_krnl[port].readReg(pkt_cnt_o_offset_2);
            temp_cnt = temp_cnt<<32;
            temp_cnt += m_krnl[port].readReg(pkt_cnt_o_offset_1);
            return temp_cnt;
        }
       
        bool getErrors(int port) {
            return m_krnl[port].readReg(error_o_offset);
        } 
    private:
        AlveoLink::common::FPGA* m_card;
        AlveoLink::common::IP m_krnl[t_NumPorts];


        static const unsigned int gen_col_i_offset           =0x10;
		static const unsigned int start_i_offset             =0x18;
		static const unsigned int id_i_offset                =0x20;
		static const unsigned int pkt_cnt_o_offset_1         =0x28;
        static const unsigned int pkt_cnt_o_offset_2         =0x2c;
		static const unsigned int packets_to_send_i_offset_1 =0x40;
        static const unsigned int packets_to_send_i_offset_2 =0x44;
	    static const unsigned int error_o_offset             =0x4c;
		static const unsigned int  dim_i_offset              =0x5c;
        GEN_COLL_TYPE getType(int port) {
            return static_cast<GEN_COLL_TYPE>(m_krnl[port].readReg(gen_col_i_offset));
        }
        
};

#endif
