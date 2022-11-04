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
#ifndef _NHOPPARTITION_H_
#define _NHOPPARTITION_H_

#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include <unordered_map>
#include <map>

#include "nHopCSR.h"
#include "nHopCSR.hpp"
#include "ctime.hpp"
#include "packBuff.hpp"
#include "hopImp.hpp"
#include "xNativeFPGA.hpp"

extern CTimeModule<unsigned long> gtimer;

#define MAX_CORE_COPY (128)
#define MAX_CORE (128)
#define MAX_NUM_CHNL_KNL (16)
// MAX_NUM_CHNL_PAR (MAX_CORE * MAX_CHNL)
#define MAX_NUM_CHNL_PAR (1024)
#define MAX_NUM_KNL_PAR (1024)
#define MAX_NUM_KNL_USED (1024)
#define MAX_NUM_HOP (16)

// return the real number of partition //call FindSegmentPoint()
template <class T>
int CSRPartition_average(int num_chnl_par, CSR<T>& src, T limit_nv, T limit_ne, CSR<T>* des[]);

template <class T>
class PartitionHop { // the class for partition graph, generate all pointer of channel data
   public:
    CSR<T>* hop_src;
    AlveoLink::common::FPGA* m_card;

    // Number of hop channel in kernel. Now it is suppoosed that all kernels have same number of hop channel
    // Different kernel implementation may have different num_chnl_knl, from 2 ~ 16 maybe.
    // Usually, for graphs with large degree less channel may already match the bandwidth of network and more HBM space
    // can be saved for buffering
    int num_chnl_knl;
    T limit_v_b; // space to load the vettex by Byte
    T limit_e_b; // space to load the edge by Byte

    // Number of kernel(card) used for cover entire graph
    // The ID of partition kernel begins with 0, as 0, 1, 2, 3, .....num_knl_par - 1
    int num_knl_par;

    // Number of hop channel used for cover entire graph, = num_knl_par * num_chnl_inDev
    // The ID of partition channel begins with 0, as 0, 1, 2, 3, .....num_chnl_par - 1
    int num_chnl_par;

    // Number of kernel used for deploying graph. num_knl_used >= num_knl_par
    // The ID of kernel used begins with 0, as 0, 1, 2, 3, .....num_knl_used - 1
    // The mapping for ID_knl_used to ID_knl_par =  ID_knl_used % num_knl_par, this will be used by host for deploying
    // data on kernel
    // The number of cpopy for eatch ID_knl_par =  ID_knl_used / num_knl_par, this will be deploied on kernel
    // The mapping for ID_knl_par to ID_knl_used =  ID_knl_par + Status_RoundRobin * num_knl_par, this will be used for
    // dispatching package on dev
    int num_knl_used;

    CSR<T>* par_chnl_csr[MAX_NUM_CHNL_PAR];
    HopImp<T>* hopKnl[MAX_NUM_KNL_USED];
    int tab_copy_knl[MAX_NUM_KNL_PAR];
    int tab_state_knl[MAX_NUM_KNL_USED];
    T tab_disp_knl[MAX_NUM_KNL_PAR];

    PartitionHop();
    PartitionHop(CSR<T>* src);
    ~PartitionHop();

    int CreatePartitionForKernel(
        commentInfo commentInfo, int num_knl_in, int num_chnl_knl_in, T limit_nv_byte, T limit_ne_byte);
    int initKernel(commentInfo commentInfo, int num_knl_in, int num_chnl_knl_in, T limit_nv_byte, T limit_ne_byte);
    void PartitionPair(std::string foldername, ap_uint<64>* pairs, int num_pair, int pairGroup);
    void PartitionPair(PackBuff<T>* pair_in[MAX_NUM_KNL_USED], ap_uint<64>* pairs, int num_pair);
    int LoadPair2Buff(T NV, T NE, int num_hop, commentInfo commentInfo, timeInfo* p_timeInfo, IndexStatistic* p_stt);
    int LoadPair2Buff(std::vector<T>& pairs,
                      int num_total_pair,
                      int num_part_pair,
                      T NV,
                      T NE,
                      int num_hop,
                      commentInfo commentInfo,
                      std::vector<unsigned int> card_table,
                      timeInfo* p_timeInfo,
                      IndexStatistic* p_stt);
    void MergeResult(commentInfo* commentInfo);
    void PrintRpt(int numHop, commentInfo commentInfo, timeInfo timeInfo, IndexStatistic stt);

    // ParID_ch to ParID_knl
    int ParID_knl(int ParID_ch) { return ParID_ch / num_chnl_knl; }
    // ParID_ch to NumCpy
    int NumCpy_ch(int ParID_ch) { return NumCpy_knl(ParID_knl(ParID_ch)); }
    // ParID_knl to NumCpy //TODO

    int GetNumKnlCopy(int id_kp); // Return how many copies for this kernel partition
    void syncDispTab_inter();
    void syncDispTab_inter(int ikp);
    void ShowDevPar(int myId);
    // int Deploykernel(int num_knl_used_in, HopImp<T>* p_knl){}
    // give a channel parition ID, the function return how many and which kernel and which hop-channel in the kernel
    // deployed the channel paritition with the ID
    //   int* p_id_knl, //output: kernels' id in a stream
    //   int* p_no_ch,  //output: which channel in the kernel used for store the 'id_ch_par'
    //   int id_ch_par  //intput: Channel-paritition ID
    int FindChnlInKnl(int* p_id_knl, int* p_no_ch, int id_ch_par, int myId);
    void ShowInfo();
    void ShowChnlPar(int myId);
    void ShowInfo_buffs(int rnd, int myId);
    int DoSwitching(int id_src);
    int DoSwitching();
    void FreePar();
    void FreeKnl();
};

template <class T>
PartitionHop<T>::PartitionHop() {
    num_knl_used = 0;
    num_chnl_par = 0;
    hop_src = NULL;
    for (int i = 0; i < MAX_NUM_CHNL_PAR; i++) par_chnl_csr[i] = NULL;
    for (int i = 0; i < MAX_NUM_KNL_USED; i++) {
        tab_state_knl[i] = 0;
        hopKnl[i] = NULL;
    }
}
template <class T>
PartitionHop<T>::PartitionHop(CSR<T>* src) {
    PartitionHop();
    hop_src = src;
}
template <class T>
PartitionHop<T>::~PartitionHop() {
    FreePar();
    FreeKnl();
}
#endif
