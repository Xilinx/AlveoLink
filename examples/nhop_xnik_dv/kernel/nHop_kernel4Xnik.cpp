/*
 * Copyright 2019 Xilinx, Inc.
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

#include "nHop_kernel4Xnik.hpp"
#include "hls_memfifo4Xnik.hpp"
#include "nHop.hpp"
#include "xnikPktDefs.hpp" 
//#include "xf_graph_L2.hpp"

#ifndef __SYNTHESIS__
#include <iostream>
#endif

void consume0(hls::stream<bool>& e) {
#pragma HLS INLINE off

    bool end = e.read();
    while (!end) {
#pragma HLS PIPELINE II = 1
        end = e.read();
    }
}

template <int W>
void consume1(hls::stream<ap_uint<W> >& strm, hls::stream<bool>& e) {
#pragma HLS INLINE off

    bool end = e.read();
    while (!end) {
#pragma HLS PIPELINE II = 1
        strm.read();
        end = e.read();
    }
}

void writeOut(unsigned p_myId,
              unsigned p_numDevs,
              unsigned p_tmId,
              hls::stream<ap_uint<512> >& strmIn,
              hls::stream<bool>& strmInEnd,
              hls::stream<ap_uint<512> >& strmOut) {
#pragma HLS INLINE off

    AlveoLink::kernel::DvHopCtrlPkt<AL_netDataBits, AL_destBits> l_ctrlPkt;
    l_ctrlPkt.setSrcId(p_myId);
    l_ctrlPkt.setNumDevs(p_numDevs);
    l_ctrlPkt.setTmId(p_tmId);
    l_ctrlPkt.setType(AlveoLink::kernel::PKT_TYPE::config);
    l_ctrlPkt.write(strmOut);

    bool end = false;
    do {
#pragma HLS PIPELINE II = 1
        ap_uint<512> tmp;
        tmp = strmIn.read();
        end = strmInEnd.read();

        if (tmp != 0) strmOut.write(tmp);
    } while (!end);
}

void nHopKernelStrmWrapper(unsigned numHop,
                           unsigned intermediate,
                           unsigned numPairs,
                           unsigned localBatchSize,
                           unsigned switchBatchSize,
                           unsigned pingPongBatchSize,
                           unsigned hashSize,
                           unsigned aggrThreshold,
                           unsigned pingPongThreshold,
                           unsigned byPass,
                           unsigned duplicate,
                           unsigned numKnlPar,
                           ap_uint<512>* pair,
                           hls::stream<ap_uint<512> >& switchIn,

                           ap_uint<32>* offsetTable,
                           ap_uint<32>* indexTable,
                           ap_uint<64> cardTable[4][32],
                           ap_uint<32>* mapTable,

                           unsigned* offset0,
                           ap_uint<128>* index0,
                           unsigned* offset1,
                           ap_uint<128>* index1,
                           unsigned* offset2,
                           ap_uint<128>* index2,
                           unsigned* offset3,
                           ap_uint<128>* index3,

                           ap_uint<512>* bufferPing,
                           ap_uint<512>* bufferPong,

                           unsigned userSize,
                           unsigned cycle2sleep,
                           unsigned times_sleep2over,
                           unsigned pop_blen_read,
                           unsigned pop_ii_check,
                           unsigned pop_max_check,
                           unsigned id_broadcast,
                           unsigned id_self,
                           unsigned blen_write,
                           unsigned p_numDevs,
                           unsigned p_tmId,

                           ap_uint<512>* bufferFIFO0,
                           ap_uint<512>* bufferFIFO1,

                           unsigned* numOut,
                           ap_uint<512>* bufferLocal,
                           hls::stream<ap_uint<512> >& switchOut) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<512> > pairStream0;
#pragma HLS stream variable = pairStream0 depth = 512
#pragma HLS resource variable = pairStream0 core = FIFO_BRAM
    hls::stream<bool> pairStreamEnd0;
#pragma HLS stream variable = pairStreamEnd0 depth = 512
#pragma HLS resource variable = pairStreamEnd0 core = FIFO_LUTRAM

    hls::stream<ap_uint<512> > pairStream1;
#pragma HLS stream variable = pairStream1 depth = 512
#pragma HLS resource variable = pairStream1 core = FIFO_BRAM
    hls::stream<bool> pairStreamEnd1;
#pragma HLS stream variable = pairStreamEnd1 depth = 512
#pragma HLS resource variable = pairStreamEnd1 core = FIFO_LUTRAM

    hls::stream<ap_uint<512> > statusStream;
#pragma HLS stream variable = statusStream depth = 8
#pragma HLS resource variable = statusStream core = FIFO_SRL
    hls::stream<bool> statusStreamEnd;
#pragma HLS stream variable = statusStreamEnd depth = 8
#pragma HLS resource variable = statusStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<512> > switchStream;
#pragma HLS stream variable = switchStream depth = 512
#pragma HLS resource variable = switchStream core = FIFO_BRAM
    hls::stream<bool> switchStreamEnd;
#pragma HLS stream variable = switchStreamEnd depth = 512
#pragma HLS resource variable = switchStreamEnd core = FIFO_LUTRAM

    hls::stream<ap_uint<512> > switchMergeStream;
#pragma HLS stream variable = switchMergeStream depth = 512
#pragma HLS resource variable = switchMergeStream core = FIFO_BRAM
    hls::stream<bool> switchMergeStreamEnd;
#pragma HLS stream variable = switchMergeStreamEnd depth = 512
#pragma HLS resource variable = switchMergeStreamEnd core = FIFO_LUTRAM

    xf::graph::internal::Hop::loadBuffer<true>(0, (numPairs + 3) / 4, pair, pairStream0, pairStreamEnd0);

#ifndef __SYNTHESIS__
    std::cout << "calling hls mem FIFO" << std::endl;
#endif

    hls_memfifo(bufferFIFO0, bufferFIFO1, userSize, cycle2sleep, times_sleep2over, pop_blen_read, pop_ii_check,
                pop_max_check, id_broadcast, id_self, blen_write, switchIn, pairStream1, pairStreamEnd1, statusStream,
                statusStreamEnd);

#ifndef __SYNTHESIS__
    std::cout << "calling nHop" << std::endl;
#endif
    xf::graph::nHop(numHop, intermediate, numPairs, localBatchSize, switchBatchSize, pingPongBatchSize, hashSize,
                    aggrThreshold, pingPongThreshold, byPass, duplicate, numKnlPar, pairStream0, pairStreamEnd0,
                    pairStream1, pairStreamEnd1, offsetTable, indexTable, cardTable, mapTable, offset0, index0, offset1,
                    index1, offset2, index2, offset3, index3, bufferPing, bufferPong, numOut, bufferLocal, switchStream,
                    switchStreamEnd);

    ap_uint<32> tmp1;
    xf::graph::internal::Hop::merge2to1<512, true, true>(statusStream, statusStreamEnd, switchStream, switchStreamEnd, tmp1,
                                                     switchMergeStream, switchMergeStreamEnd);

    writeOut(id_self, p_numDevs, p_tmId, switchMergeStream, switchMergeStreamEnd, switchOut);
}

extern "C" void nHop_kernel(unsigned* config,
                            ap_uint<512>* pair,
                            hls::stream<ap_uint<512> >& switchIn,

                            unsigned* offsetTable,
                            unsigned* indexTable,
                            ap_uint<64>* cardTable,

                            unsigned* offset0,
                            ap_uint<128>* index0,
                            unsigned* offset1,
                            ap_uint<128>* index1,
                            unsigned* offset2,
                            ap_uint<128>* index2,
                            unsigned* offset3,
                            ap_uint<128>* index3,

                            ap_uint<512>* bufferPing,
                            ap_uint<512>* bufferPong,

                            ap_uint<512>* bufferFIFO0,
                            ap_uint<512>* bufferFIFO1,

                            unsigned* numOut,
                            ap_uint<512>* bufferLocal,
                            hls::stream<ap_uint<512> >& switchOut) {
    const int ext_mem_size = 16 << 20;

// clang-format off
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem0 port = config depth = 32
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem0 port = offsetTable depth = 32
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem0 port = indexTable depth = 32
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem0 port = cardTable depth = 32
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem0 port = pair depth = ext_mem_size

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem1 port = offset0 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem2 port = index0 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem3 port = offset1 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem4 port = index1 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem5 port = offset2 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem6 port = index2 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem7 port = offset3 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 2 num_read_outstanding = \
    64 max_write_burst_length = 1 max_read_burst_length = 32 bundle = gmem8 port = index3 depth = ext_mem_size

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 64 num_read_outstanding = \
    64 max_write_burst_length = 32 max_read_burst_length = 32 bundle = gmem9 port = bufferPing depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 64 num_read_outstanding = \
    64 max_write_burst_length = 32 max_read_burst_length = 32 bundle = gmem10 port = bufferPong depth = ext_mem_size

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 64 num_read_outstanding = \
    64 max_write_burst_length = 32 max_read_burst_length = 32 bundle = gmem11 port = bufferFIFO0 depth = ext_mem_size
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 64 num_read_outstanding = \
    64 max_write_burst_length = 32 max_read_burst_length = 32 bundle = gmem12 port = bufferFIFO1 depth = ext_mem_size

#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 64 num_read_outstanding = \
    2 max_write_burst_length = 32 max_read_burst_length = 8 bundle = gmem13 port = numOut depth = 32
#pragma HLS INTERFACE m_axi offset = slave latency = 32 num_write_outstanding = 64 num_read_outstanding = \
    2 max_write_burst_length = 32 max_read_burst_length = 8 bundle = gmem13 port = bufferLocal depth = ext_mem_size


#pragma HLS INTERFACE s_axilite port = config bundle = control
#pragma HLS INTERFACE s_axilite port = offsetTable bundle = control
#pragma HLS INTERFACE s_axilite port = indexTable bundle = control
#pragma HLS INTERFACE s_axilite port = cardTable bundle = control
#pragma HLS INTERFACE s_axilite port = pair bundle = control

#pragma HLS INTERFACE s_axilite port = offset0 bundle = control
#pragma HLS INTERFACE s_axilite port = index0 bundle = control
#pragma HLS INTERFACE s_axilite port = offset1 bundle = control
#pragma HLS INTERFACE s_axilite port = index1 bundle = control
#pragma HLS INTERFACE s_axilite port = offset2 bundle = control
#pragma HLS INTERFACE s_axilite port = index2 bundle = control
#pragma HLS INTERFACE s_axilite port = offset3 bundle = control
#pragma HLS INTERFACE s_axilite port = index3 bundle = control

#pragma HLS INTERFACE s_axilite port = bufferPing bundle = control
#pragma HLS INTERFACE s_axilite port = bufferPong bundle = control

#pragma HLS INTERFACE s_axilite port = bufferFIFO0 bundle = control
#pragma HLS INTERFACE s_axilite port = bufferFIFO1 bundle = control

#pragma HLS INTERFACE s_axilite port = numOut bundle = control
#pragma HLS INTERFACE s_axilite port = bufferLocal bundle = control

#pragma HLS INTERFACE axis port = switchIn
#pragma HLS INTERFACE axis port = switchOut

#pragma HLS INTERFACE s_axilite port = return bundle = control
// clang-format on

#ifndef __SYNTHESIS__
    std::cout << "hop kernel call success" << std::endl;
#endif

    int j = 0;
    unsigned numHop = config[j++];
    unsigned intermediate = config[j++];
    unsigned numPairs = config[j++];
    unsigned localBatchSize = config[j++];
    unsigned switchBatchSize = config[j++];
    unsigned pingPongBatchSize = config[j++];
    unsigned hashSize = config[j++];
    unsigned aggrThreshold = config[j++];
    unsigned pingPongThreshold = config[j++];
    unsigned byPass = config[j++];
    unsigned duplicate = config[j++];
    unsigned numKnlPar = config[j++];
    unsigned userSize = config[j++];
    unsigned cycle2sleep = config[j++];
    unsigned times_sleep2over = config[j++];
    unsigned pop_blen_read = config[j++];
    unsigned pop_ii_check = config[j++];
    unsigned pop_max_check = config[j++];
    unsigned id_broadcast = config[j++];
    unsigned id_self = config[j++];
    unsigned blen_write = config[j++];

    unsigned int l_numDevs = config[j++];
    unsigned int l_tmId = config[j++];

    const int numHopPU = 4;
    const int maxSubGraph = 32;
    const int maxDevice = 4096;

    ap_uint<32> table0[16];
#pragma HLS ARRAY_PARTITION variable = table0 complete
    ap_uint<32> table1[16];
#pragma HLS ARRAY_PARTITION variable = table1 complete
    ap_uint<64> table2[numHopPU][maxSubGraph];
#pragma HLS ARRAY_PARTITION variable = table2 complete
    ap_uint<32> table3[maxDevice];
#pragma HLS resource variable = table3 core = FIFO_BRAM

#ifndef __SYNTHESIS__
    std::cout << "loading config" << std::endl;
#endif

LoadOffsetTable:
    for (int i = 0; i < numHopPU + 1; i++) {
#pragma HLS PIPELINE II = 1
        table0[i] = offsetTable[i];
#ifndef __SYNTHESIS__
// std::cout << "offsetTable[" << i << "]=" << offsetTable[i] << std::endl;
#endif
    }
LoadIndexTable:
    for (int i = 0; i < numHopPU + 1; i++) {
#pragma HLS PIPELINE II = 1
        table1[i] = indexTable[i];
#ifndef __SYNTHESIS__
// std::cout << "indexTable[" << i << "]=" << indexTable[i] << std::endl;
#endif
    }
LoadCardTable:
    for (int i = 0; i < maxSubGraph; i++) {
#pragma HLS PIPELINE II = 1
        ap_uint<64> tmp;
        tmp = cardTable[i];
        for (int j = 0; j < numHopPU; j++) table2[j][i] = tmp;
#ifndef __SYNTHESIS__
// std::cout << "cardTable: id=" << cardTable[i](63, 32) << " offsetEnd=" << cardTable[i](31, 0) << std::endl;
#endif
    }
LoadMapTable:
    for (int i = maxSubGraph; i < maxSubGraph + maxDevice; i++) {
#pragma HLS PIPELINE II = 1
        ap_uint<64> tmp = cardTable[i];
        table3[i - maxSubGraph] = tmp;
#ifndef __SYNTHESIS__
// std::cout << "mapTable[" << i << "]=" << mapTable[i] << std::endl;
#endif
    }

    nHopKernelStrmWrapper(numHop, intermediate, numPairs, localBatchSize, switchBatchSize, pingPongBatchSize, hashSize,
                          aggrThreshold, pingPongThreshold, byPass, duplicate, numKnlPar, pair, switchIn, table0,
                          table1, table2, table3, offset0, index0, offset1, index1, offset2, index2, offset3, index3,
                          bufferPing, bufferPong,

                          userSize, cycle2sleep, times_sleep2over, pop_blen_read, pop_ii_check, pop_max_check,
                          id_broadcast, id_self, blen_write, l_numDevs, l_tmId, bufferFIFO0, bufferFIFO1,

                          numOut, bufferLocal, switchOut);

#ifndef __SYNTHESIS__
    std::cout << "hop kernel call finish" << std::endl;
#endif
}
