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

#ifndef _PACKBUFF_HPP_
#define _PACKBUFF_HPP_

template <class T>
struct AccessPoint {
    T* p_idx;
    int degree;
};

template <class T>
struct HopPack {
    T src;
    T des;
    T idx;
    T hop;
    T typ;
    T len;
    T id;
    void print() { printf("[s(%ld)d(%ld)i(%ld)h(%ld)]", src, des, idx, hop); }
    void print(long i, int idk) { printf("%d PackNO(%d):[s(%ld)d(%ld)i(%ld)h(%ld)]\n", idk, i, src, des, idx, hop); }
};

// for net package
template <class T>
class PackBuff {
    HopPack<T>* pBuff;
    long size;
    long p_write; // alwyas points to a empty place can be wrote
    long p_read;  // points to a place can be read if p_write not points same place
    bool Empty;

   public:
    PackBuff() {
        pBuff = NULL;
        size = 0;
        ResetBuff();
    }
    PackBuff(long size_buff) { InitBuff(size_buff); }
    ~PackBuff() {
        if (pBuff) free(pBuff);
    }
    int InitBuff(long size_buff) {
        size = size_buff;
        pBuff = (HopPack<T>*)malloc(sizeof(HopPack<T>) * size);
        assert(pBuff);
        p_write = p_read = 0;
        Empty = true;
        return 0;
    }
    void ResetBuff() {
        p_write = p_read = 0;
        Empty = true;
    }

    int push(AccessPoint<T>* p_accp, HopPack<T>* p_hpk) { // return how many package stored
        if (isFull(p_accp->degree)) {
            printf("ERROR: BUFF FULL!!!\n");
            return 0;
        }
        T idx_old = p_hpk->idx;
        for (int i = 0; i < p_accp->degree; i++) {
            p_hpk->idx = p_accp->p_idx[i];
            memcpy((void*)(pBuff + p_write), (void*)p_hpk, sizeof(HopPack<T>));
            p_write++;
            if (p_write == size) p_write = 0;
        }
        p_hpk->idx = idx_old; // recover the orignal value
        Empty = false;
        return p_accp->degree;
    }
    int push(HopPack<T>* p_hpk) {
        if (isFull()) {
            printf("ERROR: BUFF FULL!!!\n");
            return 0;
        }
        memcpy((void*)(pBuff + p_write), (void*)p_hpk, sizeof(HopPack<T>));
        p_write++;
        Empty = false;
        if (p_write == size) p_write = 0;
        return 1;
    }
    int pop(HopPack<T>* p_hpk) {
        if (isEmpty()) return 0;
        memcpy((void*)p_hpk, (void*)(pBuff + p_read), sizeof(HopPack<T>));
        p_read++;
        if (p_read == size) p_read = 0;
        if (p_read == p_write) Empty = true;
        return 1;
    }

    long GetNum() { return Empty ? 0 : (p_write > p_read) ? (p_write - p_read) : size - (p_read - p_write); }
    bool isFull() { return GetNum() == size ? true : false; }
    bool isFull(int toAdd) {
        assert(toAdd <= size);
        return (GetNum() + toAdd < size) ? true : false;
    }
    bool isEmpty() { return Empty; }

    float Ratio_used() { return 100.0 * (float)(GetNum()) / (float)size; }
    float Ratio_use() { return 1.0 - Ratio_used(); }
    void ShowInfo_buff() { ShowInfo_buff(true); }
    long getSize() { return size; }
    void ShowInfo_buff(bool isReturn) {
        printf("%9ld used (%2.1f%% of %9ld) p_write:%9ld, p_read:%9ld", GetNum(), Ratio_used(), size, p_write, p_read);
        if (isReturn) printf("\n");
    }
    void print() {
        for (int i = 0; i < GetNum(); i++) {
            long pi = (p_read + i) % size;
            HopPack<T>* ppk = pBuff + pi;
            ppk->print();
        }
    }
    void ShowInfo_buff(const char* nm, bool isReturn) {
        printf("%s:%6ld(%2.1f%%) \t", nm, GetNum(), Ratio_used());
        if (isReturn) printf("\n");
    }

    void SavePackBuff(long num, const char* name) {
        FILE* fp = fopen(name, "w");
        fprintf(fp, "%ld\n", num);
        for (int i = 0; i < num; i++) {
            fprintf(fp, "%ld %ld %ld \n", pBuff[i].src, pBuff[i].des, pBuff[i].idx);
        }
        fclose(fp);
    }

    void LoadPackBuff(const char* name) {
        FILE* fp = fopen(name, "r");
        long num;
        fscanf(fp, "%ld\n", &num);
        for (int i = 0; i < num; i++) {
            HopPack<T> hpk;
            hpk.hop = 0;
            fscanf(fp, "%ld %ld %ld \n", &hpk.src, &hpk.des, &hpk.idx);
            hpk.typ = 0;
            hpk.len = 0;
            hpk.id = 0;
            this->push(&hpk);
        }
        fclose(fp);
    }

    void SavePackBuff(std::vector<T>& pairs, int num) {
        // FILE* fp=fopen(name, "w");
        for (int i = 0; i < num; i++) {
            pairs.push_back(pBuff[i].src);
            pairs.push_back(pBuff[i].des);
            pairs.push_back(pBuff[i].idx);
        }
    }

    void LoadPackBuff(std::vector<T>& pairs, int num) {
        for (int i = 0; i < num; i++) {
            HopPack<T> hpk;
            hpk.hop = 0;
            hpk.src = pairs[3 * i];
            hpk.des = pairs[3 * i + 1];
            hpk.idx = pairs[3 * i + 2];
            hpk.typ = 0;
            hpk.len = 0;
            hpk.id = 0;
            this->push(&hpk);
        }
    }
};

#endif
