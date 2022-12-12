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
#ifndef _NHOPPARTITION_HPP_
#define _NHOPPARTITION_HPP_

#include "nHopPartition.h"
#include <sys/stat.h>

template <class T>
int FindSegmentPoint( // return nv_end_start
    int num_chnl_par,
    CSR<T>& src,
    T limit_nv,
    T limit_ne,
    T nv_start) {
    T nv_end_start; // return

    // Calculat reference nv and ne
    T nv_ref = (src.NV + num_chnl_par - 1) / num_chnl_par;
    T ne_ref = (src.NE + num_chnl_par - 1) / num_chnl_par;
    if (nv_ref > limit_nv) nv_ref = limit_nv;
    if (ne_ref > limit_ne) ne_ref = limit_ne; // fixed limit_nv to limit_ne

    nv_end_start = nv_start + nv_ref;
    if (nv_end_start > src.NV) nv_end_start = src.NV;

    T ne_start = src.offset[nv_start];
    T ne_end = src.offset[nv_end_start];

    while ((ne_end - ne_start) > limit_ne) {
        nv_end_start--;
        ne_end = src.offset[nv_end_start];
    }
    return nv_end_start;
}

template <class T>
int CSRPartition_average( // return the real number of partition
    int num_chnl_par,
    CSR<T>& src,
    T limit_nv,
    T limit_ne,
    CSR<T>* des[]) {
    if (num_chnl_par <= 0 || !src.isInit()) return -1;

    T nv_start = 0;
    T ne_start = 0;
    int cnt_par = 0;
    do {
        T nv_end_start = FindSegmentPoint<T>(num_chnl_par, src, limit_nv, limit_ne, nv_start);
        T ne_end = src.offset[nv_end_start];
        des[cnt_par] = new CSR<T>;
        des[cnt_par]->Init(nv_start, nv_end_start, src);
        nv_start = nv_end_start;
        cnt_par++;
    } while (nv_start != src.NV);
    return cnt_par;
}

template <class T>
int FindPar(T v, int num_par, T* tab_startV) {
    for (int i_par = 0; i_par < num_par; i_par++) {
        if (i_par == num_par - 1)
            return i_par;
        else if (v >= tab_startV[i_par + 1])
            continue;
        else
            return i_par;
    }
    printf("ERROR: FindPar can't find the partition ID, the content of tab_startV may have error\n");
    for (int i = 0; i < num_par; i++) printf("%d: startV=%d\n", i, tab_startV[i]);
    return -1;
}

template <class T>
int SelectParID(T v, int num_par, T* tab_startV, int* tab_copy, int* state_copy) {
    int i_par = FindPar<T>(v, num_par, tab_startV);
    int ncp = tab_copy[i_par];
    if (ncp == 1) return i_par;    // Only one copy, so the i_par is the final parition index
    int st_rr = state_copy[i_par]; // st_rr: Round-Robin state register, including states from 0, 1,.., to ncp-1;
    int ret = st_rr * num_par +
              i_par; //[0, 1,...., num_par-1], then to [0, 1,...., num_par-1]........[0, 1,... num_dev % num_par]
    // Updating Round-Robin state register
    if (st_rr < ncp - 1)
        state_copy[i_par]++;
    else
        state_copy[i_par] = 0;
    return ret;
}

/***********************************/
// PartitionHop mothed
/***********************************/

template <class T>
void PartitionHop<T>::FreePar() {
    for (int i = 0; i < num_chnl_par; i++) delete par_chnl_csr[i];
    num_chnl_par = 0;
}

template <class T>
void PartitionHop<T>::FreeKnl() {
    for (int i = 0; i < num_knl_used; i++) delete hopKnl[i];
    num_knl_used = 0;
}

template <class T>
int PartitionHop<T>::GetNumKnlCopy(int id_kp) { // Return how many copies for this kernel partition
    int min = this->num_knl_used / this->num_knl_par;
    int rem = this->num_knl_used % this->num_knl_par;
    int rem_id = (id_kp) % this->num_knl_par;
    if (rem_id >= rem)
        return min;
    else
        return min + 1;
}

template <class T>
void PartitionHop<T>::syncDispTab_inter() {
    printf("Updating Kernel Dispatching Table: \n");
    printf("-------------------V_start------Copy---- \n");
    for (int ikp = 0; ikp < this->num_knl_par; ikp++) {
        this->tab_disp_knl[ikp] = this->par_chnl_csr[ikp * this->num_chnl_knl]->V_start;
        this->tab_copy_knl[ikp] = this->GetNumKnlCopy(ikp);
        printf("Par_ID_knl:%3d:  %8d,      %3d\n", ikp, this->tab_disp_knl[ikp], this->tab_copy_knl[ikp]);
    }
}

template <class T>
void PartitionHop<T>::syncDispTab_inter(int myId) {
    printf("-------------------V_start------Copy---- \n");
    this->tab_disp_knl[myId] = this->par_chnl_csr[myId * this->num_chnl_knl]->V_start;
    this->tab_copy_knl[myId] = this->GetNumKnlCopy(myId);
    printf("Par_ID_knl:%3d:  %8d,      %3d\n", myId, this->tab_disp_knl[myId], this->tab_copy_knl[myId]);
}

template <class T>
void PartitionHop<T>::ShowDevPar(int myId) {
    this->hopKnl[myId]->ShowHopImpInfo(myId);
}

template <class T>
int PartitionHop<T>::FindChnlInKnl( // give a channel parition ID, the function return how many and which kernel and
                                    // which hop-channel in the kernel deployed the channel paritition with the ID
    int* p_id_knl,                  // output: kernels' id in a stream
    int* p_no_ch,                   // output: which channel in the kernel used for store the 'id_ch_par'
    int id_ch_par,                  // intput: Channel-paritition ID
    int myId) {
    int ret = 0; // How many channels in this kernel
    for (int c = 0; c < this->num_chnl_knl; c++) {
        int ich_d_c = this->hopKnl[myId]->GetChnlParID(c);
        if (ich_d_c == id_ch_par) {
            p_id_knl[ret] = myId;
            p_no_ch[ret] = c;
            ret++;
        }
    }
    return ret;
}

template <class T>
void PartitionHop<T>::ShowInfo() {
    for (int i = 0; i < num_chnl_par; i++) {
        par_chnl_csr[i]->ShowInfo("Par_ID_ch: ", i, limit_v_b, limit_e_b);
        printf("\n");
    }
}

template <class T>
void PartitionHop<T>::ShowChnlPar(int myId) {
    int id_knl = myId % this->num_knl_par;
    int id_ch_start = id_knl * this->num_chnl_knl;
    for (int i = id_ch_start; i < this->num_chnl_knl + id_ch_start; i++) {
        int ids[MAX_NUM_KNL_USED];
        int idc[MAX_NUM_KNL_USED];
        int num_copy = FindChnlInKnl(ids, idc, i, myId);
        if ((i) % num_chnl_knl == 0) {
            printf(
                "---------------------------kernel Partition %d with %d channel paritions "
                "---------------------------------------and has %d copies on ",
                i / num_chnl_knl, num_chnl_knl, num_copy);
            for (int d = 0; d < num_copy; d++) printf("Knl(%d) ", ids[d]);
            printf("-------------------------\n");
        }
        par_chnl_csr[i]->ShowInfo("Par_ID_ch: ", i, limit_v_b, limit_e_b);
        printf("The channel has %d copy (Knl, Ch):", num_copy);
        for (int d = 0; d < num_copy; d++) printf("(%d,%d), ", ids[d], idc[d]);
        printf("\n");
    }
}

template <class T>
void PartitionHop<T>::ShowInfo_buffs(int rnd, int myId) {
    printf("\n");
    printf("Report of status of buffs of round(%d): \n", rnd);
    printf("ID_KERNEL_USED:%d  ", myId);
    this->hopKnl[myId]->p_buff_in->ShowInfo_buff("in", false);
    this->hopKnl[myId]->p_buff_ping->ShowInfo_buff("ping", false);
    this->hopKnl[myId]->p_buff_pang->ShowInfo_buff("pang", false);
    this->hopKnl[myId]->p_buff_out->ShowInfo_buff("switch", false);
    this->hopKnl[myId]->p_buff_agg->ShowInfo_buff("agg", true);
}

template <class T>
int PartitionHop<T>::DoSwitching(int id_src) {
    PackBuff<T>* pbf = this->hopKnl[id_src]->p_buff_out;
    long num = pbf->GetNum();
    for (int i = 0; i < num; i++) {
        HopPack<T> hpk;
        pbf->pop(&hpk);

        int idk = SelectParID(hpk.idx, this->num_knl_par, this->tab_disp_knl, this->tab_copy_knl, this->tab_state_knl);
        if ((hpk.src == 3771775) || (hpk.des == 936630)) {
            printf("before DoSwitching: (idx = %d,%d,to %d,kenrelinter %d)\n", hpk.idx, hpk.hop, idk, hpk.id);
        }
        if (0 == this->hopKnl[idk]->p_buff_in->push(&hpk)) {
            printf("ERROR Switching faild HopKnl(%d) is full\n", i);
            return -1;
        }
    }
    return 0;
}

template <class T>
int PartitionHop<T>::DoSwitching() {
    for (int i = 0; i < num_knl_used; i++) {
        HopImp<T>* pk = this->hopKnl[i];
        if (pk->p_buff_out->GetNum() == 0)
            continue;
        else if (DoSwitching(i) == -1)
            return -1;
    }
    return 0;
}

template <class T>
void PartitionHop<T>::PrintRpt(int numHop, commentInfo commentInfo, timeInfo timeInfo, IndexStatistic stt) {
    T nv;
    T ne;
    std::fstream infoFile;
    std::string foldername = commentInfo.foldername;
    infoFile.open(foldername + "/" + "partition_info.txt");
    infoFile >> this->num_knl_par >> this->num_chnl_par >> nv >> ne;

    double degree = (double)ne / nv;
    printf("\n");
    printf("************************************************************************************************\n");
    printf("**************************************  nHop Summary   *****************************************\n");
    printf("************************************************************************************************\n");
    printf("INFO : disturbute nHop compute time all   : %lf\n", timeInfo.timeWrkCompute);
    printf("INFO : disturbute nHop compute kernel time: %lf\n", timeInfo.timeKernel);
    printf("INFO : disturbute nHop compute memcy time : %lf\n", timeInfo.timeWrkCompute - timeInfo.timeKernel);
    printf("************************************************************************************************\n");
    printf("*********************************************************\n");
    printf("************* Hardware resources for hopping ************\n");
    printf("*********************************************************\n");
    printf("Number of kernels                         : %9d\n", commentInfo.numKernel);
    printf("Number of channel in kernel               : %9d\n", commentInfo.numPuPerKernel);
    printf("Number of total channel in kernel         : %9d\n", commentInfo.numKernel * commentInfo.numPuPerKernel);
    printf("Size limitation for storing offsets(Byte) : %9d\n", limit_v_b);
    printf("Size limitation for storing indices(Byte) : %9d\n", limit_e_b);
    printf("*********************************************************\n");
    printf("************* The graph features for hopping ************\n");
    printf("*********************************************************\n");
    printf("Total number of vertex in graph           : %9d\n", nv);
    printf("Total number of edge in graph             : %9d\n", ne);
    printf("Bytes for storing graph data              : %9d\n", sizeof(T));
    printf("Total number of bytes for storing offsets : %9d\n", (nv + 1) * sizeof(T));
    printf("Total number of bytes for storing indices : %9d\n", ne * sizeof(T));
    printf("Degree of graph                           : %9.1lf\n", degree);
    printf("*********************************************************\n");
    printf("************* Commend line and input ********************\n");
    printf("*********************************************************\n");
    printf("Number of hop                             : %9d\n", numHop);
    printf("Number of kernels                         : %9d\n", commentInfo.numKernel);
    printf("Number of channel in kernel               : %9d\n", commentInfo.numPuPerKernel);
    printf("Batch size in kernel                      : %9d\n", commentInfo.sz_bat);
    printf("Duplicate graph Mode                      : %9d\n", commentInfo.duplicate);
}

template <class T>
int PartitionHop<T>::CreatePartitionForKernel(commentInfo commentInfo, // Created
                                              int num_knl_in,
                                              int num_chnl_knl_in,
                                              T limit_nv_byte,
                                              T limit_ne_byte) {
    this->num_knl_used = num_knl_in; // now is the number of board // while num_knl_par is the actually number of kernel
                                     // to cover the graph
    this->num_chnl_knl = num_chnl_knl_in;
    this->limit_v_b = limit_nv_byte;
    this->limit_e_b = limit_ne_byte;
    T nv = this->hop_src->NV;
    T ne = this->hop_src->NE;

    printf("\n");
    printf("*********************************************************\n");
    printf("************* Hardware resources for hopping ************\n");
    printf("*********************************************************\n");
    printf("Number of kernels                         : %9d\n", num_knl_in);
    printf("Number of channel in kernel               : %9d\n", num_chnl_knl);
    printf("Number of total channel in kernel         : %9d\n", num_knl_in * num_chnl_knl);
    printf("Size limitation for storing offsets(Byte) : %9d\n", limit_v_b);
    printf("Size limitation for storing indices(Byte) : %9d\n", limit_e_b);
    printf("*********************************************************\n");
    printf("************* The graph features for hopping ************\n");
    printf("*********************************************************\n");
    printf("Total number of vertex in graph           : %9d\n", nv);
    printf("Total number of edge in graph             : %9d\n", ne);
    printf("Bytes for storing graph data              : %9d\n", sizeof(T));
    printf("Total number of bytes for storing offsets : %9d\n", (nv + 1) * sizeof(T));
    printf("Total number of bytes for storing indices : %9d\n", ne * sizeof(T));
    printf("****************************************************************************************************\n");
    printf("******* To find proper number of channel covering the entire graph and supporting intra-copy********\n");
    printf("****************************************************************************************************\n");
    int num_ch_cover_nv = (nv * sizeof(T) + limit_v_b - 1) / (limit_v_b);
    int num_ch_cover_ne = (ne * sizeof(T) + limit_e_b - 1) / (limit_e_b);
    T limit_v = limit_v_b / sizeof(T);
    T limit_e = limit_e_b / sizeof(T);
    int num_ch_cover = num_ch_cover_nv > num_ch_cover_ne ? num_ch_cover_nv : num_ch_cover_ne;
    if (num_ch_cover >= this->num_chnl_knl) { // Size of graph > capacity of kernel
        num_knl_par = (num_ch_cover + this->num_chnl_knl - 1) / this->num_chnl_knl;
        printf("Number of kernel to cover graph (Estimated)    : %5d\n", num_knl_par);
        printf("Number of channel to cover graph (Estimated)   : %5d\n", num_knl_par * num_chnl_knl);
        this->num_chnl_par =
            CSRPartition_average(commentInfo.duplicate ? (1 * num_ch_cover) : (commentInfo.numKernel * num_chnl_knl),
                                 *hop_src, limit_v, limit_e, par_chnl_csr);
        while (num_chnl_par > num_knl_par * num_chnl_knl) {
            num_knl_par++;
            this->num_chnl_par = CSRPartition_average(
                commentInfo.duplicate ? (1 * num_ch_cover) : (commentInfo.numKernel * num_chnl_knl), *hop_src, limit_v,
                limit_e, par_chnl_csr);
        }
        printf("Number of minimal kernel to cover graph (Final): %5d\n", num_knl_par);
        printf("Number of channel to cover graph (Final)       : %5d\n", num_chnl_par);
    } else { // The graph tends to be stored in one kernel
        while ((0 != this->num_chnl_knl % num_ch_cover))
            num_ch_cover++; // To find proper number of channel of divid factor of num_chnl_knl
        num_knl_par = (num_ch_cover + this->num_chnl_knl - 1) / this->num_chnl_knl;
        printf("Number of kernel to cover graph (Estimated) : %5d\n", num_knl_par);
        assert(num_knl_par == 1);
        num_knl_par = commentInfo.duplicate ? (num_knl_par) : (commentInfo.numKernel);
        this->num_chnl_par =
            CSRPartition_average(commentInfo.duplicate ? (1 * num_ch_cover) : (commentInfo.numKernel * num_chnl_knl),
                                 *hop_src, limit_v, limit_e, par_chnl_csr);
        /*1 * num_ch_cover*/ // todo
        while (num_chnl_par > num_knl_par * num_chnl_knl) {
            this->FreePar();
            while ((0 != this->num_chnl_knl % num_ch_cover))
                num_ch_cover++; // To find proper number of channel of divid factor of num_chnl_knl
            this->num_chnl_par = CSRPartition_average(
                commentInfo.duplicate ? (1 * num_ch_cover) : (commentInfo.numKernel * num_chnl_knl), *hop_src, limit_v,
                limit_e, par_chnl_csr);
        }
        printf("Number of channel to cover graph (Final)    : %5d\n", num_chnl_par);
        printf("Number of kernel to cover graph (Final)     : %5d\n", num_knl_par);
    }

    printf("\n");
    syncDispTab_inter();

    for (int i = 0; i < this->num_chnl_par; i++) {
        par_chnl_csr[i]->SaveToFile(commentInfo.foldername, i);
    }
    std::ofstream infoFile;
    std::string foldername = commentInfo.foldername;
    infoFile.open(foldername + "/" + "partition_info.txt");
    infoFile << this->num_knl_par << " " << this->num_chnl_par << " " << this->hop_src->NV << " " << this->hop_src->NE
             << std::endl;
    infoFile.close();

    return 0;
}

template <class T>
void PartitionHop<T>::PartitionPair(std::string foldername, ap_uint<64>* pairs, int num_pair, int pairGroup) {
    PackBuff<T>* pair_in[this->num_knl_used];
    long sz_in = this->limit_e_b / 16;
    for (int i = 0; i < this->num_knl_used; i++) {
        pair_in[i] = new (PackBuff<T>);
        pair_in[i]->InitBuff(sz_in);
    }
    int num_pair_group = num_pair / pairGroup;
    for (int group = 0; group < pairGroup; group++) {
        for (int i = group * num_pair_group; i < num_pair_group + group * num_pair_group; i++) {
            HopPack<T> hpk;
            hpk.src = (pairs[i])(31, 0);
            hpk.des = (pairs[i])(63, 32);
            hpk.idx = hpk.src; // fixed a bug
            hpk.hop = 0;
            int idk =
                SelectParID(hpk.idx, this->num_knl_par, this->tab_disp_knl, this->tab_copy_knl, this->tab_state_knl);
            pair_in[idk]->push(&hpk);
        }

        if (pairGroup == 1) {
            for (int i = 0; i < this->num_knl_used; i++) {
                std::string l_name = foldername + "/" + "pair_k" + std::to_string(i) + ".txt";
                pair_in[i]->SavePackBuff(pair_in[i]->GetNum(), l_name.c_str());
            }
        } else {
            for (int i = 0; i < this->num_knl_used; i++) {
                std::string pairfoldername = foldername + "/pair_group_" + std::to_string(group);
                mkdir(pairfoldername.c_str(), 0750);
                std::string l_name = pairfoldername + "/" + "pair_k" + std::to_string(i) + ".txt";
                pair_in[i]->SavePackBuff(pair_in[i]->GetNum(), l_name.c_str());
            }
        }
    }
}

template <class T>
void PartitionHop<T>::PartitionPair(PackBuff<T>* pair_in[MAX_NUM_KNL_USED], ap_uint<64>* pairs, int num_pair) {
    syncDispTab_inter();
    long sz_in = this->limit_e_b / 16;
    for (int i = 0; i < this->num_knl_used; i++) {
        pair_in[i] = new (PackBuff<T>);
        pair_in[i]->InitBuff(sz_in);
    }
    for (int i = 0; i < num_pair; i++) {
        HopPack<T> hpk;
        hpk.src = (pairs[i])(31, 0);
        hpk.des = (pairs[i])(63, 32);
        hpk.idx = hpk.src;
        hpk.hop = 0;
        int idk = SelectParID(hpk.idx, this->num_knl_par, this->tab_disp_knl, this->tab_copy_knl, this->tab_state_knl);
        pair_in[idk]->push(&hpk);
    }
}

template <class T>
int PartitionHop<T>::initKernel(
    commentInfo commentInfo, int num_knl_in, int num_chnl_knl_in, T limit_nv_byte, T limit_ne_byte) {
    this->num_knl_used = num_knl_in; // now is the number of board // while num_knl_par is the actually number of kernel
                                     // to cover the graph
    this->num_chnl_knl = num_chnl_knl_in;
    this->limit_v_b = limit_nv_byte;
    this->limit_e_b = limit_ne_byte;

    if (!commentInfo.foldername.empty()) {
        std::fstream infoFile;
        std::string foldername = commentInfo.foldername;
        infoFile.open(foldername + "/" + "partition_info.txt");
        infoFile >> this->num_knl_par >> this->num_chnl_par;
        infoFile.close();
    }
    int id_knl = commentInfo.myId % this->num_knl_par;
    int id_ch_start = id_knl * this->num_chnl_knl;

    if (!commentInfo.foldername.empty()) {
        for (int i = id_ch_start; i < this->num_chnl_knl + id_ch_start; i++) {
            CSR<T>* tmpCSR = new CSR<T>;
            tmpCSR->ReadFromFile(commentInfo.foldername, i);
            this->par_chnl_csr[i] = tmpCSR;
        }
    }

    syncDispTab_inter(commentInfo.myId);
    printf("**************************************************************************\n");
    printf("****** Deploy based on Final minimal number kernel for covering graph ****\n");
    printf("**************************************************************************\n");

    this->hopKnl[commentInfo.myId] = new (HopImp<T>);
    this->hopKnl[commentInfo.myId]->InitCore(this->par_chnl_csr, id_ch_start, this->num_chnl_knl, this->num_chnl_par,
                                             this->num_knl_par, commentInfo.myId, this->tab_disp_knl,
                                             this->tab_copy_knl);
    //}
    printf("\n");
    ShowChnlPar(commentInfo.myId);
    printf("\n");
    ShowDevPar(commentInfo.myId);
    this->hopKnl[commentInfo.myId]->ShowDispTab_intra(commentInfo.myId);
    return 0;
}

inline void getCardTableOffset(const commentInfo& p_commentInfo, std::vector<unsigned int>& p_krnlOffsets) {
    unsigned int l_numKernel = p_commentInfo.numKernel;
    unsigned int l_numPuPerKernel = p_commentInfo.numPuPerKernel;
    for (unsigned int k = 0; k < l_numKernel; ++k) {
        std::string foldername = p_commentInfo.foldername;
        std::string l_infoFileName =
            foldername + "/" + "info_" + std::to_string(k * l_numPuPerKernel + l_numPuPerKernel - 1) + ".txt";
        std::ifstream l_infoFile(l_infoFileName);
        std::vector<unsigned int> l_intVec(2);
        l_infoFile >> l_intVec[0] >> l_intVec[1];
        p_krnlOffsets[k] = l_intVec[1];
        l_infoFile.close();
    }
}

template <class T>
int PartitionHop<T>::LoadPair2Buff(
    T NV, T NE, int numHop, commentInfo commentInfo, timeInfo* p_timeInfo, IndexStatistic* p_stt) {
    // 1) initialize each kernels' buffs according to the size of pair, NV ,NE and numHop
    printf("\n");
    printf("Initialize each kernels' buffs according to the size of pair, NV ,NE and numHop\n");
    long sz_in = this->limit_e_b / 16;
    long sz_pp = this->limit_e_b / 16;
    long sz_out = sz_pp;
    long sz_agg = sz_pp;
    if (commentInfo.useXnik != 0) {
        for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
            this->hopKnl[i]->InitBuffs(sz_in, sz_out, sz_pp, sz_agg);
        }
    } else {
        for (int i = 0; i < this->num_knl_used; i++) this->hopKnl[i]->InitBuffs(sz_in, sz_out, sz_pp, sz_agg);
    }
    printf("Size for intput     : %9d BYTEs\n", sz_in);
    printf("Size for output     : %9d BYTEs\n", sz_out);
    printf("Size for aggregation: %9d BYTEs\n", sz_agg);
    printf("num_knl_used : %d\n", num_knl_used);
    // 2) dispatching pairs into kernels' buff_in
    if (commentInfo.useXnik != 0) {
        for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
            std::string foldername = commentInfo.foldername;
            std::string l_name;
            if (commentInfo.pairGroupId == -1) {
                l_name = foldername + "/" + "pair_k" + std::to_string(i) + ".txt";
            } else {
                l_name = foldername + "/pair_group_" + std::to_string(commentInfo.pairGroupId) + "/pair_k" +
                         std::to_string(i) + ".txt";
            }

            this->hopKnl[i]->p_buff_in->LoadPackBuff(l_name.c_str());
        }
    } else {
        for (int i = 0; i < this->num_knl_used; i++) {
            std::string foldername = commentInfo.foldername;
            std::string l_name;
            if (commentInfo.pairGroupId == -1) {
                l_name = foldername + "/" + "pair_k" + std::to_string(i) + ".txt";
            } else {
                l_name = foldername + "/pair_group_" + std::to_string(commentInfo.pairGroupId) + "/pair_k" +
                         std::to_string(i) + ".txt";
            }
            this->hopKnl[i]->p_buff_in->LoadPackBuff(l_name.c_str());
        }
    }

    // do 3), 4) and 5) until all are buffs empty.
    // 3) Enable batch processing for each kernel: a)first check buff_in empty? b) then do batch hopping ; exception:
    // buff full
    // 4) SW switching: a)check each buff_out and pop the package then push into proper kernel's buff_in;
    // 5) aggregation: pop package and send it into aggregator
    long sz_bat = 32768; // GetSuggestedBatch(sz_pp, numHop, NE/NV);
    long rnd = 0;
    bool allEmpty = true;
    do {
        if (rnd == 0)
            printf("\n========BEFORE HOPPING===========\n");
        else
            printf("\n========AFTER SWITCHING===========\n");
        if (commentInfo.useXnik != 0) {
            for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
                ShowInfo_buffs(rnd, i);
            }
        } else {
            for (int i = 0; i < this->num_knl_used; i++) {
                ShowInfo_buffs(rnd, i);
            }
        }

        printf("\n========DOING  HOPPING===========\n");
        // 3) do hopping
        allEmpty = true;

        std::vector<unsigned int> l_cardTableOffset(commentInfo.numKernel);
        getCardTableOffset(commentInfo, l_cardTableOffset);
        std::vector<HopImp<T>*> Hknls;
        if (commentInfo.useXnik != 0) {
            for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
                Hknls.push_back(this->hopKnl[i]);
            }
            commentInfo.myId = commentInfo.startId;
        } else {
            for (int i = 0; i < this->num_knl_used; i++) {
                Hknls.push_back(this->hopKnl[i]);
            }
        }
        ConsumeKernelTask(Hknls, NV, NE, rnd, sz_bat, numHop, commentInfo, l_cardTableOffset, p_timeInfo, p_stt,
                          m_card);
        // 4 do switching
        printf("\n========AFTER HOPPING===========\n");
        if (commentInfo.useXnik != 0) {
            for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
                ShowInfo_buffs(rnd, i);
            }
        } else {
            for (int i = 0; i < this->num_knl_used; i++) {
                ShowInfo_buffs(rnd, i);
            }
        }
        // printf("\n========DOING SWITCHING===========\n");
        // only v7 sw switching need to call DoSwitching()
        // 5 do aggregation
        rnd++;
    } while (allEmpty == false);

    if (commentInfo.useXnik != 0) {
        for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
            ShowInfo_buffs(rnd, i);
        }
    } else {
        for (int i = 0; i < this->num_knl_used; i++) {
            ShowInfo_buffs(rnd, i);
        }
    }
    for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
        std::string filename = commentInfo.filename;
        this->hopKnl[i]->filename = filename + "_" + to_string(i);
    }
    if (commentInfo.useXnik != 0) {
        if (commentInfo.output) {
            for (auto i = commentInfo.startId; i < (commentInfo.startId + commentInfo.numIds); ++i) {
                this->hopKnl[i]->p_buff_agg->SavePackBuff(this->hopKnl[i]->p_buff_agg->GetNum(),
                                                          this->hopKnl[i]->filename.c_str());
            }
        }
    } else {
        for (int i = 0; i < this->num_knl_used; i++) {
            std::string filename = commentInfo.filename;
            this->hopKnl[i]->filename = filename + "_" + to_string(i);
            if (commentInfo.output) {
                this->hopKnl[i]->p_buff_agg->SavePackBuff(this->hopKnl[i]->p_buff_agg->GetNum(),
                                                          this->hopKnl[i]->filename.c_str());
            }
        }
    }

    return 0;
}

template <class T>
void PartitionHop<T>::MergeResult(commentInfo* commentInfo) {
    this->num_knl_used = commentInfo->numKernel;
    typedef std::pair<long, long> Key_pair;
    std::map<Key_pair, int> result; // unordered_map
    std::map<Key_pair, int>::iterator itr;
    long sz_agg = 268435456;
    PackBuff<T>* pair_out[this->num_knl_used];
    for (int i = 0; i < this->num_knl_used; i++) {
        pair_out[i] = new (PackBuff<T>);
        pair_out[i]->InitBuff(sz_agg);
        std::string filename = commentInfo->filename;
        std::string l_filename = filename + "_" + to_string(i);
        pair_out[i]->LoadPackBuff(l_filename.c_str());
        int num = pair_out[i]->GetNum();
        for (int j = 0; j < num; j++) {
            HopPack<T> out;
            pair_out[i]->pop(&out);
            if ((out.src == 3771775) || (out.des == 936630)) {
                printf("merge pkg: (%d,%d,%d,%d)\n", out.src, out.des, out.idx, out.hop);
            }
            T agg = out.idx;
            Key_pair Keypair(out.src, out.des);
            itr = result.find(Keypair);
            if (itr != result.end()) {
                result.at(Keypair) += (commentInfo->byPass ? 1 : agg);
            } else {
                result.insert(std::pair<Key_pair, int>(Keypair, (commentInfo->byPass ? 1 : agg)));
            }
        }
    }

    commentInfo->numRes = result.size();
    printf("aggnum(after merge) %ld\n", commentInfo->numRes);
    FILE* fp = fopen(commentInfo->filename.c_str(), "w");
    fprintf(fp, "%ld\n", commentInfo->numRes);
    for (itr = result.begin(); itr != result.end(); ++itr) {
        Key_pair Keypair = itr->first;
        fprintf(fp, "%ld %ld %ld \n", Keypair.first + 1, Keypair.second + 1, itr->second);
    }
    fclose(fp);
    printf("************************************************************************************************\n");
    printf("***************************** Hopping aggregation result per kernel ****************************\n");
    printf("************************************************************************************************\n");
    printf("kernel aggregation result                 : %9d\n", commentInfo->numRes);

    printf("hop result file had saved                 :  %s\n", commentInfo->filename.c_str());
    printf("****************************************************************************************************\n");
}

#endif
