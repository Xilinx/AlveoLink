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
#ifndef _CYCLE_V4DEMO_HPP_
#define _CYCLE_V4DEMO_HPP_

#include <unistd.h>
#include "nHopKernel.hpp"

#ifndef HLS_TEST
const int bufferDepth = 4 << 20;
const int FIFODepth = 8 << 20;
#else
const int bufferDepth = 1 << 20;
const int FIFODepth = 8 << 20;
#endif

#if (NETSTREAM == 1)

void nHop_kernel_host(xilinx_apps::nhop::nHopKernel<NHOP_numPUs>& l_nHopCU,
                      unsigned* config,
                      ap_uint<512>* pair,
                      ap_uint<512>* memFIFO,
                      unsigned* offsetTable,
                      unsigned* indexTable,
                      ap_uint<64>* cardTable,
                      std::vector<unsigned*> offsetBuffer,
                      std::vector<unsigned*> indexBuffer,
                      ap_uint<512>* zeroBuffer0,
                      ap_uint<512>* zeroBuffer1,
                      unsigned* numOut,
                      ap_uint<512>* local,
                      AlveoLink::common::FPGA* p_cardPtr) {
    unsigned duplicate = config[10];
    std::cout << "INFO: duplicate = " << duplicate << std::endl;

    // create buffers for CU0
    l_nHopCU.createBuf(0, sizeof(unsigned) * (1024), config);
    l_nHopCU.createBuf(1, sizeof(ap_uint<512>) * (bufferDepth - 1024), pair);
    l_nHopCU.createBuf(3, sizeof(unsigned) * (1024), offsetTable);
    l_nHopCU.createBuf(4, sizeof(unsigned) * (1024), indexTable);
    l_nHopCU.createBuf(5, sizeof(ap_uint<64>) * (8192), cardTable);

    size_t l_offsetBufBytes[NHOP_numPUs];
    size_t l_indexBufBytes[NHOP_numPUs];
    for (unsigned int i = 0; i < NHOP_numPUs; ++i) {
        l_offsetBufBytes[i] = sizeof(unsigned) * (offsetTable[i + 1] - offsetTable[i] + 4096);
        l_indexBufBytes[i] = sizeof(unsigned) * (indexTable[i + 1] - indexTable[i] + 4096);
    }
    l_nHopCU.createCsrBufs(l_offsetBufBytes, l_indexBufBytes, reinterpret_cast<void**>(offsetBuffer.data()),
                           reinterpret_cast<void**>(indexBuffer.data()));
    l_nHopCU.createBuf(14, sizeof(ap_uint<512>) * (bufferDepth), zeroBuffer0);
    l_nHopCU.createBuf(15, sizeof(ap_uint<512>) * (bufferDepth), zeroBuffer1);
    l_nHopCU.createFifoBuf(sizeof(ap_uint<512>) * (FIFODepth), memFIFO);
    l_nHopCU.createBuf(18, sizeof(unsigned) * (1024), numOut);
    l_nHopCU.createBuf(19, sizeof(ap_uint<512>) * (bufferDepth - 1024), local);
}

void nHop_run(std::vector<xilinx_apps::nhop::nHopKernel<NHOP_numPUs> > l_nHopCUs,
              AlveoLink::common::FPGA* p_cardPtr,
              int useXnik,
              timeInfo* p_timeInfo,
              AlveoLink::common::KERNEL& switch0) {
    for (auto l_nHopCU : l_nHopCUs) {
        l_nHopCU.sendBOs();
    }
    if (useXnik == 0) {
        switch0.sendBO(0);
    }
    // launch kernel and calculate kernel execution time
    std::cout << "kernel start------" << std::endl;
    // TimePointType h_compute_start = chrono::high_resolution_clock::now();
    for (auto l_nHopCU : l_nHopCUs) {
        l_nHopCU.run();
    }

#ifdef DEBUG
    //debug code start
    sleep(120);//sleep 120 seconds
    for (auto l_nHopCU : l_nHopCUs) {
        l_nHopCU.syncBOs();
    }
    //debug code end
#else
    for (auto l_nHopCU : l_nHopCUs) {
        l_nHopCU.getBOs();
    }
#endif
    // TimePointType h_compute_end;
    // double timeWrkCompute;
    // getDiffTime(h_compute_start, h_compute_end, timeWrkCompute);
    // p_timeInfo->timeWrkCompute += timeWrkCompute;
    // p_timeInfo->timeKernel = timeWrkCompute;
    std::cout << "kernel end------" << std::endl;
}

// hop processing 1 batch of vertex on FPGA
template <class T>
int BatchOneHopOnFPGA(std::vector<HopImp<T>*> Hknls,
                      std::vector<PackBuff<T>*> p_buff_pops,
                      std::vector<PackBuff<T>*> p_buff_aggs,
                      T NV,
                      T NE,
                      int numHop,
                      long estimateBatchSize,
                      commentInfo& commentInfo,
                      std::vector<unsigned int> card_table,
                      timeInfo* p_timeInfo,
                      IndexStatistic* p_stt,
                      AlveoLink::common::FPGA* m_card
                     ) {
    // -------------setup FIFO params-------------
    long freqMHz = 200;
    long us_sleep = 10; // 100 for hw_emu, 1000 for hw now
    long us_over = us_sleep;

    long pop_blen_read = 128; // suggested burst reading lenght, it not met, then waiting pop_ii_check cycles and check
                              // again, it check times met pop_max_check then read anyway
    long pop_ii_check = 128;  // watint cycles for next checking
    long pop_max_check = 8;

    long cycle2sleep = us_sleep * freqMHz;
    long times_over_sleep = ((us_over + us_sleep - 1) / us_sleep);
    long cycle_over = cycle2sleep * times_over_sleep;

    std::cout << "us_sleep           = " << us_sleep << "us\n";
    std::cout << "us_over            = " << us_over << "us\n";
    std::cout << "uscycle2sleep_over = " << cycle2sleep << "\n";
    std::cout << "times_over_sleep   = " << times_over_sleep << "\n";
    std::cout << "cycle_over         = " << cycle_over << "\n\n";

    // -------------setup k0 params---------------

    T sz_idx = 0;
    T sz_pop = 0;
    int num_chnl = commentInfo.numPuPerKernel;
    int pu = num_chnl;
    std::vector<int> counts;
    T numPairs = 0;
    for (auto p_buff_pop : p_buff_pops) {
        int count = p_buff_pop->GetNum();
        counts.push_back(count);
        numPairs = numPairs + count;
    }
    T batchSize = commentInfo.sz_bat; // todo estimateBatchSize;//commentInfo.sz_bat;
    int numKernel = commentInfo.numKernel;
    int byPass = commentInfo.byPass;
    int duplicate = commentInfo.duplicate;
    std::string xclbin_path = commentInfo.xclbin_path;
    printf("INFO: final batch size : %d, numPairs=%ld\n", batchSize, numPairs);

    std::vector<unsigned*> numOuts;
    std::vector<ap_uint<512>*> locals;
    std::vector<std::vector<T*> > offsetBuffers;
    std::vector<std::vector<T*> > indexBuffers;

    std::vector<unsigned*> offsetTables;
    std::vector<unsigned*> indexTables;
    std::vector<ap_uint<64>*> cardTables;
    std::vector<unsigned*> configs;
    std::vector<ap_uint<512>*> zeroBuffer0s;
    std::vector<ap_uint<512>*> zeroBuffer1s;
    std::vector<ap_uint<512>*> memFIFOs;
    std::vector<ap_uint<512>*> pairs;

    std::vector<xilinx_apps::nhop::nHopKernel<NHOP_numPUs> > l_nHopCUs;
    AlveoLink::common::KERNEL switch0;

    int k = 0;
    unsigned int l_syncManagerId = 0;
    unsigned int l_tmId[2];
    unsigned int l_kernelStartId = 0;
    unsigned int l_numHosts = 0;
    unsigned int l_portIds[2];
    std::fstream l_netfile;
    std::string l_netfilename = commentInfo.netfile;
    l_netfile.open(l_netfilename);
    l_netfile >> l_syncManagerId >> l_kernelStartId >> l_numHosts;
    for (unsigned int i=0; i<l_numHosts; ++i) {
        unsigned int l_hostId, l_numCards;
        l_netfile >> l_hostId >> l_numCards;
        for (unsigned int j=0; j< l_numCards; ++j) {
            unsigned int l_cardId, l_port0Id, l_port1Id, l_tm0Id, l_tm1Id;
            l_netfile >> l_cardId >> l_port0Id >> l_port1Id >> l_tm0Id >> l_tm1Id;
            if ((l_cardId == commentInfo.devId) && (l_hostId == commentInfo.hostId)){
                l_portIds[0] = l_port0Id;
                l_portIds[1] = l_port1Id;
                l_tmId[0] = l_tm0Id;
                l_tmId[1] = l_tm1Id;
                std::cout <<"INFO: tmId[0] = " << l_tmId[0] << " tmId[1]= " << l_tmId[1] << std::endl;
            }
        } 
    }
    l_netfile.close();

    for (auto Hknl : Hknls) {
        std::vector<T*> offsetBuffer(num_chnl);
        std::vector<T*> indexBuffer(num_chnl);
        unsigned* offsetTable = aligned_alloc<unsigned>(1024);
        unsigned* indexTable = aligned_alloc<unsigned>(1024);
        ap_uint<64>* cardTable = aligned_alloc<ap_uint<64> >(8192);

        for (int i = 0; i < pu; i++) {
            HopChnl<T>* pch = &(Hknl->channels[i]);
            if (i == 0) {
                offsetTable[0] = pch->pCSR->V_start;
                indexTable[0] = pch->pCSR->offset[0];
            }
            offsetTable[i + 1] = pch->pCSR->V_end_1;
            indexTable[i + 1] = pch->pCSR->E_end;
            offsetBuffer[i] = aligned_alloc<unsigned>(pch->pCSR->NV + 1 + 4096);
            indexBuffer[i] = aligned_alloc<unsigned>(pch->pCSR->NE + 4096);
            memcpy((void*)(offsetBuffer[i]), (void*)pch->pCSR->offset, (pch->pCSR->NV + 1) * sizeof(T));
            memcpy((void*)(indexBuffer[i]), (void*)pch->pCSR->index, (pch->pCSR->NE) * sizeof(T));
        }
        for (int i = 0; i < pu + 1; i++) {
            std::cout << "id=" << i << " offsetTable=" << offsetTable[i] << " indexTable=" << indexTable[i]
                      << std::endl;
        }
        offsetBuffers.push_back(offsetBuffer);
        indexBuffers.push_back(indexBuffer);
        offsetTables.push_back(offsetTable);
        indexTables.push_back(indexTable);
        cardTables.push_back(cardTable);
        for (int i = 0; i < 32; i++) {
            ap_uint<32> copy = 1;
            ap_uint<32> tmp;

            if (i == 0) {
                tmp = 0;
            } else if ((i >= 1) && (i <= numKernel)) {
                tmp = card_table[i - 1];
            } else {
                tmp = offsetTable[pu] * i;
            }
            cardTable[i] = (copy, tmp);
            std::cout << "cardTable[" << i << "]=" << tmp << std::endl;
        }
        for (int i = 32; i < 4096 + 32; i++) {
            if (commentInfo.useXnik == 1) {
                if (i%2 == 0) {
                    cardTable[i] = (i - 32)+l_kernelStartId + 1;
                }
                else {
                    cardTable[i] = (i - 32)+l_kernelStartId - 1;
                }
            }
            else {
                cardTable[i] = (i-32);
            }
            if (i<48) {
                std::cout << "cardTable[" << i << "]=" << cardTable[i] << std::endl;
            }
        }
        unsigned* config = aligned_alloc<unsigned>(1024);
        unsigned* numOut = aligned_alloc<unsigned>(1024);
        ap_uint<512>* local = aligned_alloc<ap_uint<512> >(bufferDepth - 1024);
        memset(local, 0, sizeof(ap_uint<512>) * (bufferDepth - 1024));
        ap_uint<512>* zeroBuffer0 = aligned_alloc<ap_uint<512> >(bufferDepth);
        memset(zeroBuffer0, 0, sizeof(ap_uint<512>) * (bufferDepth));
        ap_uint<512>* zeroBuffer1 = aligned_alloc<ap_uint<512> >(bufferDepth);
        memset(zeroBuffer1, 0, sizeof(ap_uint<512>) * (bufferDepth));
        ap_uint<512>* memFIFO = aligned_alloc<ap_uint<512> >(FIFODepth);
        std::memset(memFIFO, 0, FIFODepth*64);
        ap_uint<512>* pair = aligned_alloc<ap_uint<512> >(bufferDepth - 1024);

        for (int i = 0; i < bufferDepth; i++) {
            if (i < (bufferDepth - 1024)) {
                local[i] = 0;
            }
            zeroBuffer0[i] = 0;
            zeroBuffer1[i] = 0;
        }
        for (int i = 0; i < 1024; i++) {
            numOut[i] = 0;
        }
        numOuts.push_back(numOut);
        locals.push_back(local);
        configs.push_back(config);
        zeroBuffer0s.push_back(zeroBuffer0);
        zeroBuffer1s.push_back(zeroBuffer1);
        memFIFOs.push_back(memFIFO);
        pairs.push_back(pair);

        for (int i = 0; i < counts[k]; i++) {
            HopPack<T> in;
            p_buff_pops[k]->pop(&in);
            ap_uint<128> tmp128;
            tmp128.range(127, 96) = in.src;
            tmp128.range(95, 64) = in.des;
            tmp128.range(63, 32) = in.idx;
            tmp128.range(31, 24) = in.hop;
            tmp128.range(23, 20) = in.typ;
            tmp128.range(19, 16) = in.len;
            tmp128.range(15, 0) = in.id;
            pair[i / 4](128 * (i % 4) + 127, 128 * (i % 4)) = tmp128;
            if ((in.src == 2919699) || (in.des == 2000632))
                printf("pair pkg: (%d,%d,%d,%d)\n", in.src, in.des, in.idx, in.hop);
        }
        memFIFO[0] = 0;
        memFIFO[1] = 0; // allign to 512 bit
        printf("pair =%d\n", counts[k]);
        int switchBatchSize = 512;
        int pingPongBatchSize = 8192;
        int hashSize = 65536;
        int hashUpdateSize = 16348;
        int pingPongThreshold = 8192;
        printf("INFO : hashSize =%d\n", hashSize);
        printf("INFO : hashUpdateSize =%d\n", hashUpdateSize);
        printf("INFO : pingPongThreshold =%d\n", pingPongThreshold);
        int j = 0;
        // hop kernel0 config
        config[j++] = numHop;
        config[j++] = 0;
        config[j++] = counts[k];
        config[j++] = batchSize;
        config[j++] = switchBatchSize;
        config[j++] = pingPongBatchSize;
        config[j++] = hashSize;
        config[j++] = hashUpdateSize;
        config[j++] = pingPongThreshold;
        config[j++] = byPass;
        config[j++] = duplicate;
        config[j++] = 1;

        // mem fifo0 config
        config[j++] = FIFODepth - 3;
        config[j++] = us_sleep;
        config[j++] = us_over;
        config[j++] = pop_blen_read;
        config[j++] = pop_ii_check;
        config[j++] = pop_max_check;
        // broadcast id
        if (commentInfo.useXnik != 0) {
            unsigned int l_idx = commentInfo.myId%2;
            config[j++] = l_portIds[l_idx];
            config[j++] = l_portIds[l_idx]; // id of the kernel
            std::cout << "INFO: my logic id is: " << commentInfo.myId << " my network id is: " << l_portIds[l_idx] << std::endl;
        } else {
            config[j++] = k;
            config[j++] = k;
        }
        config[j++] = 64;
        if (commentInfo.useXnik != 0) {
            config[j++] = l_syncManagerId;
            unsigned int l_idx = commentInfo.myId%2;
            config[j++] = l_tmId[l_idx];
            std::cout << "INFO: syncManagerId = " << l_syncManagerId << " tmId = " << l_tmId[l_idx] << std::endl;
            commentInfo.myId = commentInfo.myId + 1;
            //config[j++] = commentInfo.batchPkts;
            //config[j++] = commentInfo.timeOutCycles;
        }
        xilinx_apps::nhop::nHopKernel<NHOP_numPUs> l_nHopCU;

        l_nHopCU.init(m_card);
        l_nHopCU.createKernel(k);

        if (k == 0 && commentInfo.useXnik == 0) {
            int waitclk = (128 << 10);
            switch0.fpga(m_card);
            switch0.createKernel("switch_kernel");
            switch0.setScalarArg(0, (unsigned int)waitclk);
        }
        nHop_kernel_host(l_nHopCU, config, pair, memFIFO, offsetTable, indexTable, cardTable, offsetBuffer, indexBuffer,
                         zeroBuffer0, zeroBuffer1, numOut, local, m_card);
        l_nHopCUs.push_back(l_nHopCU);

        k++;
    }

    TimePointType h_compute_start = chrono::high_resolution_clock::now();
    
    nHop_run(l_nHopCUs, m_card, commentInfo.useXnik, p_timeInfo, switch0);

    for (int n=0; n<commentInfo.numIds; ++n) {
        std::fstream l_memFifoFile;
        std::string l_fileName = "memFifo_"+std::to_string(commentInfo.startId + n)+".bin";
        l_memFifoFile.open(l_fileName, std::ios::app | std::ios::binary);
        l_memFifoFile.write(reinterpret_cast<char*>(memFIFOs[n]), FIFODepth*64);
        l_memFifoFile.close(); 
    }

//#ifdef DEBUG    
    //debug code start

    //debug code end
//#endif

    for (int k = 0; k < Hknls.size(); k++) {
        PackBuff<T>* p_buff_agg = p_buff_aggs[k];

        long accCntAll;
        long tmpcnt = numOuts[k][3];
        accCntAll = (tmpcnt << 32UL) + numOuts[k][2];
        std::cout << "kernel done!" << std::endl;
        std::cout << "kernel numLocal=" << numOuts[k][0] << " numSwitch=" << numOuts[k][1] << " accCntAll=" << accCntAll
                  << std::endl;
        commentInfo.accCntAllEdge = commentInfo.accCntAllEdge + accCntAll;

        for (int i = 0; i < numOuts[k][0]; i++) {
            for (int j = 0; j < 4; j++) {
                // unsigned long tmp = 0UL | (tmp_src + 1) << 32UL | (tmp_des + 1);
                HopPack<T> out;
                out.src = locals[k][i].range(128 * j + 31, 128 * j);
                out.des = locals[k][i].range(128 * j + 63, 128 * j + 32);
                out.idx = locals[k][i].range(128 * j + 95, 128 * j + 64);
                out.hop = locals[k][i].range(128 * j + 127, 128 * j + 96);
                if ((out.src == 3771775) || (out.des == 936630)) {
                    printf("local pkg: (%d,%d,%d,%d)\n", out.src, out.des, out.idx, out.hop);
                }
                if ((out.src != 0) || (out.des != 0)) {
                    p_stt->num_agg++;
                    if (0 == p_buff_agg->push(&out)) return -1;
                    p_stt->num_push++;
                }
            }
        }
        std::cout << " aggnum=" << p_buff_agg->GetNum() << std::endl;
        std::cout << " numSwitch=" << numOuts[k][1] << std::endl;

        for (int i = 0; i < pu; i++) {
            free(offsetBuffers[k][i]);
            free(indexBuffers[k][i]);
        }

        free(numOuts[k]);
        free(locals[k]);

        free(offsetTables[k]);
        free(indexTables[k]);
        free(cardTables[k]);
        free(configs[k]);

        free(zeroBuffer0s[k]);
        free(zeroBuffer1s[k]);
        free(memFIFOs[k]);
        free(pairs[k]);
    }

    TimePointType h_compute_end;
    double timeWrkCompute;
    getDiffTime(h_compute_start, h_compute_end, timeWrkCompute);
    p_timeInfo->timeWrkCompute += timeWrkCompute;
    p_timeInfo->timeKernel = timeWrkCompute;

    return 0;
}

template <class T>
int ConsumeKernelTask(std::vector<HopImp<T>*> Hknls,
                      T NV,
                      T NE,
                      int rnd,
                      long sz_bat,
                      int numHop,
                      commentInfo& commentInfo,
                      std::vector<unsigned int> card_table,
                      timeInfo* p_timeInfo,
                      IndexStatistic* p_stt,
                      AlveoLink::common::FPGA* m_card
                     ) {
    int cnt_hop = 0;
    do {
        std::vector<PackBuff<T>*> p_buff_pops;
        std::vector<PackBuff<T>*> p_buff_aggs;
        for (auto Hknl : Hknls) {
            PackBuff<T>* p_buff_pop = (cnt_hop == 0) ? Hknl->p_buff_in : Hknl->p_buff_pp[cnt_hop & 1];
            PackBuff<T>* p_buff_agg = Hknl->p_buff_agg;
            p_buff_pops.push_back(p_buff_pop);
            p_buff_aggs.push_back(p_buff_agg);
        }

        long sz_bat2 = Hknls[0]->estimateBatchSize(cnt_hop, sz_bat, p_buff_pops[0]);
        long size_pop = p_buff_pops[0]->getSize();
        printf("%s : cnt_hop=%d, sz_bat=%ld, sz_bat2=%ld, numHop=%d, size_in_pop=%d\n", __FUNCTION__, cnt_hop, sz_bat,
               sz_bat2, numHop, size_pop);

        if (-1 == BatchOneHopOnFPGA(Hknls, p_buff_pops, p_buff_aggs, NV, NE, numHop, sz_bat2, commentInfo, card_table,
                                    p_timeInfo, p_stt, m_card)) {
            printf("Error: batch size(%ld) can't be consumed out, created %ld at hop(%d)\n", sz_bat2, p_stt->num_all,
                   cnt_hop);
            printf("Please try small size of batch\n");
            return -1;
        }
        cnt_hop += numHop;
        printf("  [Hop(%d):", cnt_hop);
        p_stt->print();
        printf("]\n");
        printf("Judge: cnt_hop=%d, numHop=%d\n", cnt_hop, numHop);
    } while (cnt_hop < numHop);
    return 0;
}
#endif // netstream

#endif // hpp
