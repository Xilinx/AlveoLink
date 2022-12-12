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
#ifndef _NHOP_CSR_H_
#define _NHOP_CSR_H_

#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include "xilinx_apps_common.hpp"

/***********************************/
//*Vertices 16
//*Edges 28
// 0
// 3
//<offset>
// 4 1.000000
// 5 1.000000
//<index> [<weight>]
// Start offset will be relabeled with 0
/***********************************/
template <class T_GDATA>
int LoadGraphFile_Dir( // load <offset> file and <index> [<weight>] files, but just offset and index to mem
    const char* offsetfile,
    const char* indexfile,
    T_GDATA& numVertices,
    T_GDATA& numEdges,
    T_GDATA*& offset,
    T_GDATA*& index
    //, bool hasWeight
    );

/***********************************/
//*Vertices 16
//*Edges 28
// 1 3 1.000000
// 1 4 1.000000
//<head> <tail> [<weight>]
// Start vertex will be relabeled with 0
/***********************************/
template <class T>
int LoadGraphFile_Dir( // load graph file "<head> <tail> [<weight>]" to offset and index to mem, weight is no use in
                       // nhop
    const char* file_name,
    T& NV,
    T& NE,
    T*& offset,
    T*& index,
    bool hasWeight);

template <class T>
void printCSR(T V_start, T NV, T NE, T* vtxPtr, T* vtxInd, float* weight, T begin, T end, T* C, T* M);

template <class T>
void printCSR(
    T NV, T NE, T* vtxPtr, T* vtxInd, float* weight, T begin, T end, T* C, T* M); // warper printCSR, V_start=0;

template <class T>
class CSR {
   public:
    T V_start;
    T V_end_1;
    T E_end;
    T NV;
    T NE;
    T* offset;
    T* index;
    float* weight;
    CSR() {
        V_start = V_end_1 = 0;
        NV = NE = 0;
        offset = index = 0;
        weight = 0;
    }
    ~CSR() {
        if (offset) free(offset);
        if (index) free(index);
        if (weight) free(weight);
    }
    bool isInit() { return (offset != 0 && index != 0); }
    int Init(T start, T end, CSR& src);
    int Init(const char* offsetfile, const char* indexfile); // LoadGraphFile_Dir from "<head>" and "<tail> [<weight>]"
    int Init(const char* file_name, bool hasWeiht);          // LoadGraphFile_Dir from "<head> <tail> [<weight>]"
    void Init(T v_start, T v_end, T nv, T ne, T e_end, std::vector<T>& p_offset, std::vector<T>& p_index);
    
    void Init(T v_start, T v_end, T nv, T ne, T e_end,  xilinx_apps::XVector<T>& p_offset,  xilinx_apps::XVector<T>& p_index);

    void ShowInfo() { printf("V_start = %d,\t V_end_plus1 = %d,\t NV = %d,\t NE = %d\n", V_start, V_end_1, NV, NE); }
    void ShowInfo(T limit_v_byte, T limit_e_byte) {
        float ratio_v = (float)(100 * NV * sizeof(T)) / limit_v_byte;
        float ratio_e = (float)(100 * NE * sizeof(T)) / limit_e_byte;
        printf("V_start = %8d,    V_end = %8d,    NV = %8d (%2.1f%%),    NE = %8d (%2.1f%%), ", V_start, V_end_1 - 1,
               NV, ratio_v, NE, ratio_e);
        // printf("\t %2.3f%% of %d Bytes", (float)(100*NV*sizeof(T))/limit_v_byte, limit_v_byte);
        // printf("\t %2.3f%% of %d Bytes", (float)(100*NE*sizeof(T))/limit_e_byte, limit_e_byte);
    }
    void ShowInfo(const char* head) {
        printf("%s: ", head);
        ShowInfo();
    }
    void ShowInfo(const char* head, int id) {
        printf("%s %2d: ", head, id);
        ShowInfo();
    }
    void ShowInfo(const char* head, int id, T limit_v_byte, T limit_e_byte) {
        printf("%s %2d: ", head, id);
        ShowInfo(limit_v_byte, limit_e_byte);
    }

    void PrintSelf(T begin, T end) { printCSR<T>(V_start, NV, NE, offset, index, weight, begin, end, NULL, NULL); }
    void PrintSelf() { PrintSelf(0, NV); }
    void SaveToFile(std::string foldername, int cnt_par);
    void ReadFromFile(std::string foldername, int cnt_par);
};

template <class T>
void printCSR(CSR<T>* G, T begin, T end) {
    T* C = NULL;
    T* M = NULL;
    printCSR<T>(G->NV, G->NE, G->offset, G->index, G->weight, begin, end, C, M);
};

#endif
