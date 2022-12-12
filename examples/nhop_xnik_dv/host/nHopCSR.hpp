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
#ifndef _NHOP_CSR_HPP_
#define _NHOP_CSR_HPP_

#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include "nHopCSR.h"

/***********************************/
// CSR class Methods
/***********************************/
template <class T>
int CSR<T>::Init(T start, T end, CSR<T>& src) {
    V_start = start;
    V_end_1 = end;
    NV = V_end_1 - V_start;
    // Prepare Offset
    offset = (T*)malloc((NV + 1) * sizeof(T));
    if (offset == 0) return -1;
    for (int i = 0; i < (NV + 1); i++) offset[i] = src.offset[i + V_start];
    // Prepare Index
    T tmp_start = src.offset[V_start];
    T tmp_end = src.offset[V_end_1];
    E_end = tmp_end;
    NE = tmp_end - tmp_start;
    index = (T*)malloc(NE * sizeof(T));
    if (index == 0) {
        free(offset);
        return -1;
    }
    for (int i = 0; i < NE; i++) {
        index[i] = src.index[i + tmp_start];
    }
    if (src.weight) {
        weight = (float*)malloc(NE * sizeof(float));
        if (weight == 0) {
            free(offset);
            free(index);
            return -1;
        }
        for (int i = 0; i < NE; i++) {
            weight[i] = src.weight[i + tmp_start];
        }
    }
    return 0;
}

template <class T>
int CSR<T>::Init(const char* offsetfile, const char* indexfile) {
    if (LoadGraphFile_Dir<T>(offsetfile, indexfile, this->NV, this->NE, this->offset, this->index) != -1) {
        this->V_start = 0;
        this->V_end_1 = NV;
    }
    return -1;
}

template <class T>
int CSR<T>::Init(const char* file_name, bool hasWeiht) {
    if (LoadGraphFile_Dir<T>(file_name, this->NV, this->NE, this->offset, this->index, hasWeiht) != -1) {
        this->V_start = 0;
        this->V_end_1 = NV;
    }
    return -1;
}


template <class T>
void CSR<T>::Init(T v_start, T v_end, T nv, T ne, T e_end,  xilinx_apps::XVector<T>& p_offset,  xilinx_apps::XVector<T>& p_index) {
    this->V_start = v_start;
    this->V_end_1 = v_end;
    this->NV = nv;
    this->NE = ne;
    this->E_end = e_end;

    this->offset = (T*)malloc((NV + 1) * sizeof(T));
    this->index = (T*)malloc(NE * sizeof(T));
    memcpy(this->offset, p_offset.data(), (NV + 1) * sizeof(T));
    memcpy(this->index, p_index.data(), NE * sizeof(T));
}


template <class T>
void CSR<T>::Init(T v_start, T v_end, T nv, T ne, T e_end, std::vector<T>& p_offset, std::vector<T>& p_index) {
    this->V_start = v_start;
    this->V_end_1 = v_end;
    this->NV = nv;
    this->NE = ne;
    this->E_end = e_end;

    this->offset = (T*)malloc((NV + 1) * sizeof(T));
    this->index = (T*)malloc(NE * sizeof(T));
    memcpy(this->offset, p_offset.data(), (NV + 1) * sizeof(T));
    memcpy(this->index, p_index.data(), NE * sizeof(T));
}

inline void readBin(std::string name, void* mat, unsigned int totalSize) {
    std::ifstream inFile;
    inFile.open(name, std::ifstream::binary);
    if (inFile.is_open()) {
        inFile.read((char*)mat, totalSize);
        inFile.close();
    } else {
        std::cerr << "Could not find " << name << std::endl;
        exit(1);
    }
}

inline void saveBin(std::string name, void* mat, unsigned int totalSize) {
    std::ofstream outFile(name, std::ios::binary);
    outFile.write((char*)mat, totalSize);
    outFile.close();
}

template <class T>
void CSR<T>::SaveToFile(std::string foldername, int cnt_par) {
    std::ofstream infoFile;
    infoFile.open(foldername + "/" + "info_" + std::to_string(cnt_par) + ".txt");
    infoFile << this->V_start << " " << this->V_end_1 << " " << this->NV << " " << this->NE << " " << this->E_end
             << std::endl;
    infoFile.close();

    // if(NE != 0){
    saveBin(foldername + "/" + "offset_" + std::to_string(cnt_par) + ".bin", this->offset, (NV + 1) * sizeof(T));
    saveBin(foldername + "/" + "index_" + std::to_string(cnt_par) + ".bin", this->index, NE * sizeof(T));

    //}
}

template <class T>
void CSR<T>::ReadFromFile(std::string foldername, int cnt_par) {
    std::fstream infoFile;
    infoFile.open(foldername + "/" + "info_" + std::to_string(cnt_par) + ".txt", std::ios_base::in);
    infoFile >> this->V_start >> this->V_end_1 >> this->NV >> this->NE >> this->E_end;
    this->offset = (T*)malloc((NV + 1) * sizeof(T));
    this->index = (T*)malloc(NE * sizeof(T));
    // if (this->NE!=0){
    readBin(foldername + "/" + "offset_" + std::to_string(cnt_par) + ".bin", this->offset, (NV + 1) * sizeof(T));
    readBin(foldername + "/" + "index_" + std::to_string(cnt_par) + ".bin", this->index, NE * sizeof(T));
}

/***********************************/
// load and print method
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
    ) {
    int err = 0;

    char line[1024] = {0};
    T_GDATA fileIdx = 0;

    std::fstream offsetfstream(offsetfile, std::ios::in);
    if (!offsetfstream) {
        std::cout << "Error : " << offsetfile << " file doesn't exist !" << std::endl;
        return -1;
    }

    offsetfstream.getline(line, sizeof(line));
    std::stringstream numOdata(line);
    numOdata >> numVertices;

    offset = aligned_alloc<T_GDATA>(numVertices + 1);
    while (offsetfstream.getline(line, sizeof(line))) {
        std::stringstream data(line);
        data >> offset[fileIdx];
        fileIdx++;
    }
    offsetfstream.close();

    fileIdx = 0;
    std::fstream indexfstream(indexfile, std::ios::in);
    if (!indexfstream) {
        std::cout << "Error : " << indexfile << " file doesn't exist !" << std::endl;
        return -1;
    }

    indexfstream.getline(line, sizeof(line));
    std::stringstream numCdata(line);
    numCdata >> numEdges;

    index = aligned_alloc<T_GDATA>(numEdges);
    while (indexfstream.getline(line, sizeof(line))) {
        std::stringstream data(line);
        data >> index[fileIdx];
        // if(hasWeight){
        float tmp;
        data >> tmp;
        //}
        fileIdx++;
    }
    indexfstream.close();

    return 0;
}

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
    bool hasWeight) {
    int err = 0;

    char line[1024] = {0};

    std::fstream indexfstream(file_name, std::ios::in);
    if (!indexfstream) {
        std::cout << "Error : " << file_name << " file doesn't exist !" << std::endl;
        return -1;
    }

    {
        indexfstream.getline(line, sizeof(line));
        std::stringstream numCdata(line);
        std::string head;
        numCdata >> head;
        numCdata >> NV;
    }
    {
        indexfstream.getline(line, sizeof(line));
        std::stringstream numCdata(line);
        std::string head;
        numCdata >> head;
        numCdata >> NE;
    }
    T cnt_off = 0;
    T v_old;
    T v_new;
    T v_off;
    T v_start = 1;
    T v_end;
    T cnt_e = 0;

    offset = aligned_alloc<T>(NV + 2); // not sure start with 0 or 1
    memset(offset, 0, (NV + 2) * sizeof(T));
    T* arr_head = aligned_alloc<T>(NE);
    T* arr_tail = aligned_alloc<T>(NE);
    index = aligned_alloc<T>(NE);
    while (indexfstream.getline(line, sizeof(line))) {
        std::stringstream data(line);
        data >> v_new;
        T e_tail;
        data >> e_tail;
        if (v_new > NV + 1) {
            printf("Error lable not ordered %d\n", v_new);
            free(offset);
            free(arr_tail);
            free(index);
            return -1;
        }
        offset[v_new]++;
        arr_head[cnt_e] = v_new;
        arr_tail[cnt_e] = e_tail;

        if (hasWeight) {
            float tmp;
            data >> tmp;
        }
        if (e_tail == 0 || v_new == 0) v_start = 0;
        cnt_e++;
    }
    if (cnt_e != NE) {
        printf("Warrnning!! number of NE(%d)!= %d\n", NE, cnt_e);
        NE = cnt_e;
        // free(offset);
        // free(arr_tail);
        // free(index);
        // return -1;
    }
    T add_off = 0;
    int degree_max = 0;
    for (int i = 0; i < NV; i++) {
        T degree = offset[i + v_start];
        offset[i] = add_off;
        add_off += degree;
        if (degree > degree_max) degree_max = degree;
    }
    offset[NV] = add_off;

    T* tmp_cnt = aligned_alloc<T>(NE);
    memset(tmp_cnt, 0, NE * sizeof(T));
    for (int i = 0; i < NE; i++) {
        T e_head = arr_head[i] - v_start;
        T e_tail = arr_tail[i] - v_start;
        index[offset[e_head] + tmp_cnt[e_head]] = e_tail;
        if (tmp_cnt[e_head] > offset[e_head + 1] - offset[e_head]) {
            printf("Error degree tmp(%d) > offset%d\n", tmp_cnt[e_head], offset[e_head + 1] - offset[e_head]);
            free(offset);
            free(arr_tail);
            free(index);
            return -1;
        }
        tmp_cnt[e_head]++;
    }

    free(arr_head);
    free(arr_tail);
    indexfstream.close();

    return degree_max;
};

template <class T>
void printCSR(T V_start, T NV, T NE, T* vtxPtr, T* vtxInd, float* weight, T begin, T end, T* C, T* M) {
    printf("NV = %ld\t;  NE = %ld \n", NV, NE);
    if (begin < 0) begin = 0;
    if (end > NV) end = NV;
    // printf("|==C==|==V==|==M==|=OFF=|=Dgr=|\n");
    T off_start = vtxPtr[0];
    for (int v = begin; v < end; v++) {
        T adj1 = vtxPtr[v];
        T adj2 = vtxPtr[v + 1];
        T degree = adj2 - adj1;

        T m = M == NULL ? v : M[v];
        T c = C == NULL ? v : C[v];
        // printf(" c=%-4d, v=%-4d,", c, v, m, adj1, degree);
        if (M == NULL || C == NULL)
            printf(" v=%-5d ", v + V_start);
        else {
            if (m < 0)
                printf(" \033[1;31;40mc=%-5d v=%-5d m=%-5d\033[0m", c, v, m);
            else
                printf(" c=%-5d v=%-5d m=%-5d", c, v, m);
        }
        printf(" off=%-5d d=%-4d |", adj1, degree);
        for (int d = 0; d < degree; d++) {
            //\033[1;31;40mERROR\033[0m
            T t = vtxInd[adj1 - off_start + d];
            double w = weight != 0 ? weight[adj1 - off_start + d] : 1.0;
            if (M != NULL && M[t] < 0)
                printf("\033[1;31;40m%5d\033[0m\/%1.0f ", t, w);
            else
                printf("%5d\/%1.0f ", t, w);
        }
        printf("\n");
    }
};

template <class T>
void printCSR(T NV, T NE, T* vtxPtr, T* vtxInd, float* weight, T begin, T end, T* C, T* M) {
    T V_start = 0;
    printCSR(V_start, NV, NE, vtxPtr, vtxInd, weight, begin, end, C, M);
};

#endif
