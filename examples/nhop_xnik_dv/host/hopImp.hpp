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

#ifndef _HOPIMP_HPP_
#define _HOPIMP_HPP_

#define MAX_CORE_COPY (128)
#define MAX_CORE (128)
#define MAX_NUM_CHNL_KNL (16)
// MAX_NUM_CHNL_PAR (MAX_CORE * MAX_CHNL)
#define MAX_NUM_CHNL_PAR (1024)
#define MAX_NUM_KNL_PAR (1024)
#define MAX_NUM_KNL_USED (1024)
#define MAX_NUM_HOP (16)

template <class T>
bool CheckRange_v(T v, T V_start, T V_end_1);
// call FindPar return ParID
template <class T>
int SelectParID(T v, int num_par, T* tab_startV, int* tab_copy, int* state_copy);

// 1 FPGA compute unit has NUM_CHAL (8 channels) now
// partition data for hop channel
template <class T>
class HopChnl {
   public:
    CSR<T>* pCSR;
    T indexShift;  // initialized by reading offset[0];
    T offsetShift; // initialized by pCSR->V_start;
    HopChnl() { pCSR = NULL; }
    void Init(CSR<T>* pcsr) {
        assert(pcsr);
        pCSR = pcsr;
        indexShift = pCSR->offset[0];
        offsetShift = pCSR->V_start;
    }
    T getOffset(T v) {
        assert(pCSR);
        return pCSR->offset[v - offsetShift];
    }
    bool isVinRange(T v) { return CheckRange_v<T>(v, pCSR->V_start, pCSR->V_end_1); }

    // For CPU similation, no need to created real data structure like HopPack which will take too more memory
    T* LookUp(int* degree, T v) {
        assert(pCSR);
        isVinRange(v);
        assert(v >= this->pCSR->V_start);
        assert(v < this->pCSR->V_end_1);
        T off1 = getOffset(v);
        T off2 = getOffset(v + 1);
        *degree = off2 - off1;
        return this->pCSR->index + (off1 - indexShift);
    }

    // For CPU similation, no need to created real data structure like HopPack which will take too more memory
    void LookUp(AccessPoint<T>* accp, T v) { accp->p_idx = LookUp(&accp->degree, v); }

    // For CPU similation, no need to created real data
    // structure like HopPack which will take too more memory
    void LookUp(AccessPoint<T>* accp, HopPack<T>* hpk) {
        T v;
        if (hpk->hop == 0)
            v = hpk->des;
        else
            v = hpk->idx;
        accp->p_idx = LookUp(&accp->degree, v);
    }

    // Return the real degree;
    T LookUp(T* des, int size, T v) { // Return the real degree;
        AccessPoint<T> accp;
        LookUp(&accp, v);
        if (size > accp.degree) size = accp.degree;
        for (int i = 0; i < size; i++) des[i] = accp.p_idx[i];
        return accp.degree;
    }
};

struct IndexStatistic { // for index statistic
    long num_v;
    long num_all;   //=0;
    long num_local; //=0;
    long num_send;  //=0;
    long num_agg;   //=0;
    long num_push;
    long num_free;
    IndexStatistic() {
        num_v = 0;
        num_all = 0;
        num_local = 0;
        num_send = 0;
        num_agg = 0;
        num_push = 0;
        num_free = 0;
    }
    float r_local() { return num_all == 0 ? 0 : (100.0 * (float)num_local / (float)num_all); }
    float r_send() { return num_all == 0 ? 0 : (100.0 * (float)num_send / (float)num_all); }
    float r_agg() { return num_all == 0 ? 0 : (100.0 * (float)num_agg / (float)num_all); }
    float r_push() { return num_all == 0 ? 0 : (100.0 * (float)num_push / (float)num_all); }
    float r_free() { return num_all == 0 ? 0 : (100.0 * (float)num_free / (float)num_all); }
    void print() {
        // printf("Stt:V%-4ld)->Idx(%-4ld):Local(%-4ld),Send(%-4ld),Agg(%-4ld); Push(%-4ld),Free(%-4ld)", num_v,
        // num_all, num_local, num_send, num_agg, num_push, num_free);
        // printf("Stt:V%3ld->Idx%3ld::\tLocal:%3ld(%2.1f%%)  +  Send:%3ld(%2.1f%%)  +  Agg:%3ld(%2.1f%%);\t
        // Push:%3ld(%2.1f%%) + Free:%3ld(%2.1f%%)",
        printf(
            "Input package:%-6ld -> Output Idx:%-6ld including:\tLocal:%-6ld(%3d%%)  +  Send:%-6ld(%3d%%)  +  "
            "Agg:%-6ld(%3d%%);\t Push:%-6ld(%3d%%) + Free:%-6ld(%3d%%)",
            num_v, num_all, num_local, (int)r_local(), num_send, (int)r_send(), num_agg, (int)r_agg(), num_push,
            (int)r_push(), num_free, r_free());
    }
};

struct timeInfo {
    double timeWrkCompute = 0;
    double timeKernel = 0;
};

/***********************************/
// HopImp mothed
/***********************************/
// Logic function for judging a package content; There can be a lot of logic funtions for different hop-based
// application

template <class T>
class HopImp { // warpper the hop kernel
   public:
    int start_chnl;
    int num_chnl_knl;
    int num_chnl_par;
    int num_knl_par; // while num_knl_par is the actually number of kernel to cover the graph
    int id_knl_used;
    std::string filename; // for output test
    HopChnl<T> channels[MAX_NUM_CHNL_KNL];
    T tab_disp_ch[MAX_NUM_CHNL_KNL];
    int tab_copy_ch[MAX_NUM_CHNL_KNL];
    int state_copy_ch[MAX_NUM_CHNL_KNL];
    T tab_disp_knl[MAX_NUM_KNL_PAR];
    int tab_copy_knl[MAX_NUM_KNL_PAR];
    int tab_state_knl[MAX_NUM_KNL_USED];

    PackBuff<T>* p_buff_in;
    PackBuff<T>* p_buff_out;
    PackBuff<T>* p_buff_ping;
    PackBuff<T>* p_buff_pang;
    PackBuff<T>* p_buff_agg;
    PackBuff<T>* p_buff_pp[2];

    HopImp();
    ~HopImp();
    // Logic function for judging a package content; There can be a lot of logic funtions for different hop-based
    // application
    bool Judge_equal(HopPack<T>& pkg) {
        if (pkg.des == pkg.idx)
            return true;
        else
            return false;
    }

    // clang-format off
    // hop processing 1 batch of vertex
    int BatchOneHop(PackBuff<T>* p_buff_pop, PackBuff<T>* p_buff_send, PackBuff<T>* p_buff_local,
                    long sz_bat, int numHop, IndexStatistic* p_stt){
        // PackBuff<T>* p_buff_pp_w = this->p_buff_pp[cnt_bat&1];
        long sz_idx = 0;
        long sz_pop = 0;
        for (int i = 0; i < sz_bat; i++) {
                HopPack<T> in;
                p_buff_pop->pop(&in);
                AccessPoint<T> accp;
                this->LookUp(&accp, &in);
                p_stt->num_v++;
                p_stt->num_all += accp.degree;
                for (int d = 0; d < accp.degree; d++) {
                        HopPack<T> out;
                        out.src = in.src;
                        out.des = in.des;
                        out.idx = accp.p_idx[d];
                        out.hop = in.hop + 1;
                        // judge out 1)agg;
                        if (out.des == out.idx) { // Judge_equal<T>(out)
                                p_stt->num_agg++;
                                if (0 == this->p_buff_agg->push(&out)) return -1;
                                p_stt->num_push++;
                        }
                        // hop done
                        if (out.hop == numHop) {
                                p_stt->num_free++;
                                continue;
                        } else { // considering stay locally or send to others
                                int idk = SelectParID<T>(out.idx, this->num_knl_par, this->tab_disp_knl, this->tab_copy_knl,
                                                                                 this->tab_state_knl);
                                if (idk % this->num_knl_par == GetKnlParID()) {
                                        p_stt->num_local++;
                                        if (0 == p_buff_local->push(&out)) return -1;
                                        p_stt->num_push++;
                                } else {
                                        p_stt->num_send++;
                                        if (0 == p_buff_send->push(&out)) return -1;
                                        p_stt->num_push++;
                                }
                        }
                }
        }
        return 0;
    }

    // hop processing 1 batch of vertex on FPGA
    int BatchOneHopOnFPGA(PackBuff<T>* p_buff_pop, PackBuff<T>* p_buff_send, PackBuff<T>* p_buff_local, PackBuff<T>* p_buff_agg,
                          T NV, T NE, T numSubPairs, int num_hop, T estimateBatchSize, commentInfo commentInfo, timeInfo* p_timeInfo,
                          IndexStatistic* p_stt);
    // clang-format on
    long estimateBatchSize(int cnt_hop, long sz_suggest, PackBuff<T>* p_buff_pop) {
        long sz_bat = sz_suggest;
        if (cnt_hop == 0)
            sz_bat = this->p_buff_in->GetNum() > sz_bat ? sz_bat : p_buff_in->GetNum();
        else
            sz_bat = p_buff_pop->GetNum(); // currently we hop the entire batch's middle package can be processed
        // But in future, there should be some method to avoid overflow
        return sz_bat;
    }

    int ConsumeTask(T NV,
                    T NE,
                    int rnd,
                    T numSubpair,
                    long sz_bat,
                    int num_hop,
                    commentInfo commentInfo,
                    timeInfo* p_timeInfo,
                    IndexStatistic* p_stt); // call BatchOneHopOnFPGA()

    void InitBuffs(long sz_in, long sz_out, long sz_pp, long sz_agg) {
        p_buff_in = new (PackBuff<T>);   //->InitBuff(sz_in);
        p_buff_out = new (PackBuff<T>);  //->InitBuff(sz_out);
        p_buff_ping = new (PackBuff<T>); //->InitBuff(sz_pp);
        p_buff_pang = new (PackBuff<T>); //->InitBuff(sz_pp);
        p_buff_agg = new (PackBuff<T>);  //->InitBuff(sz_agg);
        p_buff_in->InitBuff(sz_in);
        p_buff_out->InitBuff(sz_out);
        // p_buff_ping->InitBuff(sz_pp);
        // p_buff_pang->InitBuff(sz_pp);
        p_buff_agg->InitBuff(sz_agg);
        p_buff_pp[0] = p_buff_ping;
        p_buff_pp[1] = p_buff_pang;
    }

    void InitCore(CSR<T>* par_chnl_csr[], int start, int num_ch_knl, int num_ch_par) {
        start_chnl = start;
        this->num_chnl_knl = num_ch_knl;
        this->num_chnl_par = num_ch_par;
        int n_cpy = (num_ch_knl + num_ch_par - 1) / num_ch_par;
        for (int c = 0; c < n_cpy; c++) {
            for (int i = 0; i < num_ch_par; i++) {
                int id_ch = i + num_ch_par * c;
                if (id_ch >= num_ch_knl) break;
                channels[id_ch].Init(par_chnl_csr[start_chnl + i]);
                tab_copy_ch[i] = (c == 0) ? 1 : tab_copy_ch[i] + 1;
                tab_disp_ch[i] = par_chnl_csr[start_chnl + i]->V_start;
                state_copy_ch[i] = 0;
            }
        }
    }

    void InitCore(CSR<T>* par_chnl_csr[],
                  int start,
                  int num_ch_knl,
                  int num_ch_par,
                  int num_knl_p,
                  int id_knl_u,
                  T* tab_disp_k, //[MAX_NUM_KNL_PAR];
                  int* tab_copy_k /*[MAX_NUM_KNL_PAR];*/) {
        start_chnl = start;
        this->num_chnl_knl = num_ch_knl;
        this->num_chnl_par = num_ch_par;
        this->num_knl_par = num_knl_p;
        this->id_knl_used = id_knl_u;
        int n_cpy = (num_ch_knl + num_ch_par - 1) / num_ch_par;
        for (int c = 0; c < n_cpy; c++) {
            for (int i = 0; i < num_ch_par; i++) {
                int id_ch = i + num_ch_par * c;
                if (id_ch >= num_ch_knl) break;
                channels[id_ch].Init(par_chnl_csr[start_chnl + i]);
                tab_copy_ch[i] = (c == 0) ? 1 : tab_copy_ch[i] + 1;
                tab_disp_ch[i] = par_chnl_csr[start_chnl + i]->V_start;
                state_copy_ch[i] = 0;
            }
        }

        for (int i = 0; i < this->num_knl_par; i++) {
            this->tab_disp_knl[i] = tab_disp_k[i];
            this->tab_copy_knl[i] = tab_copy_k[i];
            this->tab_state_knl[i] = 0;
        }
    }

    void InitCore(CSR<T>* par_chnl_csr[], int start, int num) { InitCore(par_chnl_csr, start, num, num); }

    void LookUp(AccessPoint<T>* p_accp, HopPack<T>* p_hpk) { // For CPU similation, no need to created real data
                                                             // structure like HopPack which will take too more
                                                             // memory
        T v;
        if (p_hpk->hop == 0)
            v = p_hpk->des;
        else
            v = p_hpk->idx;
        int num_chnl_knl_par = num_chnl_par < num_chnl_knl ? num_chnl_par : num_chnl_knl;
        int id_ch = SelectParID<T>(v, num_chnl_knl_par, tab_disp_ch, tab_copy_ch, state_copy_ch);
        assert(id_ch < num_chnl_knl);
        HopChnl<T>* pch = &(channels[id_ch]);
        pch->LookUp(p_accp, p_hpk);
    }

    int SelectChnlParID(T v) {
        int i_par;
        for (i_par = 0; i_par < num_chnl_par; i_par++) {
            if (i_par == num_chnl_par - 1) break;
            if (this->tab_disp_ch[i_par + 1])
                continue;
            else
                break;
        }
        int ncp = GetNumChnlCopy();
        if (ncp == 1) return i_par;
        int st_rr = state_copy_ch[i_par]; // 0, 1,..ncp-1;
        int ret = st_rr * num_chnl_par;
        if (st_rr < ncp - 1)
            state_copy_ch[i_par]++;
        else
            state_copy_ch[i_par] = 0;
    }

    int GetChnlParID(int i_ch) { return start_chnl + i_ch % this->num_chnl_par; }

    int GetNumChnlCopy() { return (num_chnl_knl + num_chnl_par - 1) / num_chnl_par; }

    int GetKnlParID() { return this->id_knl_used % this->num_knl_par; }

    void ShowHopImpInfo(int id_core) {
        printf("HopImp(%d) to cover %d of %d channel partitions, including Par_ID_ch:  ", id_core, num_chnl_knl,
               num_chnl_par);
        for (int i = 0; i < this->num_chnl_knl; i++) {
            printf(" %4d,", GetChnlParID(i)); // start_chnl + i%this->num_chnl_par);
        }
        printf("\n");
    }

    void ShowDispTab_intra(int id_core) {
        printf("Updating Channel Dispatching Table in kernel(%d): \n", id_core);
        printf("-------------------V_start------Copy---- \n");
        int size_tab = num_chnl_par < num_chnl_knl ? num_chnl_par : num_chnl_knl;
        for (int i = 0; i < size_tab; i++) {
            printf("Par_ID_ch:%3d:  %8d,      %3d\n", i, tab_disp_ch[i], tab_copy_ch[i]);
        }
    }
};

template <class T>
HopImp<T>::HopImp() {
    start_chnl = 0;
    num_chnl_knl = 0;
    num_chnl_par = 0;
    num_knl_par = 0;
    id_knl_used = 0;
    p_buff_in = NULL;
    p_buff_out = NULL;
    p_buff_ping = NULL;
    p_buff_pang = NULL;
    p_buff_agg = NULL;
    p_buff_pp[0] = NULL;
    p_buff_pp[1] = NULL;
}
template <class T>
HopImp<T>::~HopImp() {
    if (p_buff_in) delete p_buff_in;
    if (p_buff_out) delete p_buff_out;
    if (p_buff_ping) delete p_buff_ping;
    if (p_buff_pang) delete p_buff_pang;
    if (p_buff_agg) delete p_buff_agg;
}

#endif
