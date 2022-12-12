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

#ifndef __XF_GRAPH_nHop_HPP_
#define __XF_GRAPH_nHop_HPP_

#define DEBUG true
#ifdef DEBUG

//#define DEBUG_LOAD true
#define DEBUG_HOP true
#define DEBUG_AGGR true
#define DEBUG_NET true
#define DEBUG_OUTPUT true

#endif

#include <ap_int.h>
#include <hls_stream.h>
#include "xf_database/hash_lookup3.hpp"

namespace xf {
namespace graph {
namespace internal {
namespace Hop {

template <int IN_NM>
ap_uint<3> mux(ap_uint<IN_NM> rd) {
#pragma HLS inline
    ap_uint<3> o = 0;
    if (IN_NM == 8) {
        o[0] = rd[1] | rd[3] | rd[5] | rd[7];
        o[1] = rd[2] | rd[3] | rd[6] | rd[7];
        o[2] = rd[4] | rd[5] | rd[6] | rd[7];
    } else if (IN_NM == 4) {
        o[0] = rd[1] | rd[3];
        o[1] = rd[2] | rd[3];
    } else if (IN_NM == 2) {
        o[0] = rd[1];
    } else {
        o = 0;
    }
    return o;
}

template <int CH_NM>
ap_uint<CH_NM> mul_ch_read(ap_uint<CH_NM> empty) {
    ap_uint<CH_NM> rd = 0;
#pragma HLS inline
    for (int i = 0; i < CH_NM; i++) {
#pragma HLS unroll
        ap_uint<CH_NM> t_e = 0;
        if (i > 0) t_e = empty(i - 1, 0);
        rd[i] = t_e > 0 ? (bool)0 : (bool)empty[i];
    }
    return rd;
}

template <int KEYW, bool PAD, bool END>
void merge1to1(hls::stream<ap_uint<KEYW> >& i0_key_strm,
               hls::stream<bool>& i0_e_strm,

               ap_uint<32>& mergedCnt,
               hls::stream<ap_uint<KEYW> >& o_key_strm,
               hls::stream<bool>& o_e_strm) {
    ap_uint<KEYW> key_arry;
    bool rd_e = 0;
    bool last = 0;
    ap_uint<32> cnt = 0;
LOOP_MERGE1_1:
    do {
#pragma HLS loop_tripcount min = 1 max = 5000
#pragma HLS PIPELINE II = 1
        rd_e = !i0_e_strm.empty() && !last;
        if (rd_e) {
            key_arry = i0_key_strm.read();
            last = i0_e_strm.read();
        }
        ap_uint<KEYW> key = key_arry;
        bool valid_n = last;
        if (!valid_n && rd_e != 0) {
            cnt++;
            o_key_strm.write(key);
            o_e_strm.write(false);
        }
    } while (last != 1);
    if (END) o_e_strm.write(true);
    if (PAD) o_key_strm.write(0);
    mergedCnt = cnt;
#ifndef __SYNTHESIS__
    std::cout << "merge number=" << cnt << std::endl;
#endif
}

template <int KEYW, bool PAD, bool END>
void merge2to1(hls::stream<ap_uint<KEYW> >& i0_key_strm,
               hls::stream<bool>& i0_e_strm,
               hls::stream<ap_uint<KEYW> >& i1_key_strm,
               hls::stream<bool>& i1_e_strm,

               ap_uint<32>& mergedCnt,
               hls::stream<ap_uint<KEYW> >& o_key_strm,
               hls::stream<bool>& o_e_strm) {
    ap_uint<KEYW> key_arry[2];
#pragma HLS array_partition variable = key_arry dim = 1
    ap_uint<2> empty_e = 0;
    ap_uint<2> rd_e = 0;
    ap_uint<2> last = 0;
    ap_uint<32> cnt = 0;
LOOP_MERGE2_1:
    do {
#pragma HLS loop_tripcount min = 1 max = 5000
#pragma HLS PIPELINE II = 1
        empty_e[0] = !i0_e_strm.empty() && !last[0];
        empty_e[1] = !i1_e_strm.empty() && !last[1];
        rd_e = mul_ch_read(empty_e);
        if (rd_e[0]) {
            key_arry[0] = i0_key_strm.read();
            last[0] = i0_e_strm.read();
        }
        if (rd_e[1]) {
            key_arry[1] = i1_key_strm.read();
            last[1] = i1_e_strm.read();
        }
        ap_uint<1> id = mux<2>(rd_e);
        ap_uint<KEYW> key = key_arry[id];
        bool valid_n = last[id];
        if (!valid_n && rd_e != 0) {
            cnt++;
            o_key_strm.write(key);
            o_e_strm.write(false);
        }
    } while (last != 3);
    if (END) o_e_strm.write(true);
    if (PAD) o_key_strm.write(0);
    mergedCnt = cnt;
#ifndef __SYNTHESIS__
    std::cout << "merge number=" << cnt << std::endl;
#endif
}

template <int KEYW, bool PAD, bool END>
void merge4to1(hls::stream<ap_uint<KEYW> >& i0_key_strm,
               hls::stream<bool>& i0_e_strm,
               hls::stream<ap_uint<KEYW> >& i1_key_strm,
               hls::stream<bool>& i1_e_strm,
               hls::stream<ap_uint<KEYW> >& i2_key_strm,
               hls::stream<bool>& i2_e_strm,
               hls::stream<ap_uint<KEYW> >& i3_key_strm,
               hls::stream<bool>& i3_e_strm,

               ap_uint<32>& mergedCnt,
               hls::stream<ap_uint<KEYW> >& o_key_strm,
               hls::stream<bool>& o_e_strm) {
    ap_uint<KEYW> key_arry[4];
#pragma HLS array_partition variable = key_arry dim = 1
    ap_uint<4> empty_e = 0;
    ap_uint<4> rd_e = 0;
    ap_uint<4> last = 0;
    ap_uint<32> cnt = 0;
LOOP_MERGE4_1:
    do {
#pragma HLS loop_tripcount min = 1 max = 5000
#pragma HLS PIPELINE II = 1
        empty_e[0] = !i0_e_strm.empty() && !last[0];
        empty_e[1] = !i1_e_strm.empty() && !last[1];
        empty_e[2] = !i2_e_strm.empty() && !last[2];
        empty_e[3] = !i3_e_strm.empty() && !last[3];
        rd_e = mul_ch_read(empty_e);
        if (rd_e[0]) {
            key_arry[0] = i0_key_strm.read();
            last[0] = i0_e_strm.read();
        }
        if (rd_e[1]) {
            key_arry[1] = i1_key_strm.read();
            last[1] = i1_e_strm.read();
        }
        if (rd_e[2]) {
            key_arry[2] = i2_key_strm.read();
            last[2] = i2_e_strm.read();
        }
        if (rd_e[3]) {
            key_arry[3] = i3_key_strm.read();
            last[3] = i3_e_strm.read();
        }
        ap_uint<2> id = mux<4>(rd_e);
        ap_uint<KEYW> key = key_arry[id];
        bool valid_n = last[id];
        if (!valid_n && rd_e != 0) {
            cnt++;
            o_key_strm.write(key);
            o_e_strm.write(false);
        }
    } while (last != 15);
    if (END) o_e_strm.write(true);
    if (PAD) o_key_strm.write(0);
    mergedCnt = cnt;
#ifndef __SYNTHESIS__
    std::cout << "merge number=" << cnt << std::endl;
#endif
}

template <int KEYW, bool PAD>
void merge8to1(hls::stream<ap_uint<KEYW> >& i0_key_strm,
               hls::stream<bool>& i0_e_strm,
               hls::stream<ap_uint<KEYW> >& i1_key_strm,
               hls::stream<bool>& i1_e_strm,
               hls::stream<ap_uint<KEYW> >& i2_key_strm,
               hls::stream<bool>& i2_e_strm,
               hls::stream<ap_uint<KEYW> >& i3_key_strm,
               hls::stream<bool>& i3_e_strm,
               hls::stream<ap_uint<KEYW> >& i4_key_strm,
               hls::stream<bool>& i4_e_strm,
               hls::stream<ap_uint<KEYW> >& i5_key_strm,
               hls::stream<bool>& i5_e_strm,
               hls::stream<ap_uint<KEYW> >& i6_key_strm,
               hls::stream<bool>& i6_e_strm,
               hls::stream<ap_uint<KEYW> >& i7_key_strm,
               hls::stream<bool>& i7_e_strm,

               ap_uint<32>& mergedCnt,
               hls::stream<ap_uint<KEYW> >& o_key_strm,
               hls::stream<bool>& o_e_strm) {
    ap_uint<KEYW> key_arry[8];
#pragma HLS array_partition variable = key_arry dim = 1
    ap_uint<8> empty_e = 0;
    ap_uint<8> rd_e = 0;
    ap_uint<8> last = 0;
    ap_uint<32> cnt = 0;
LOOP_MERGE8_1:
    do {
#pragma HLS PIPELINE II = 1
        empty_e[0] = !i0_e_strm.empty() && !last[0];
        empty_e[1] = !i1_e_strm.empty() && !last[1];
        empty_e[2] = !i2_e_strm.empty() && !last[2];
        empty_e[3] = !i3_e_strm.empty() && !last[3];
        empty_e[4] = !i4_e_strm.empty() && !last[4];
        empty_e[5] = !i5_e_strm.empty() && !last[5];
        empty_e[6] = !i6_e_strm.empty() && !last[6];
        empty_e[7] = !i7_e_strm.empty() && !last[7];
        rd_e = mul_ch_read(empty_e);
        if (rd_e[0]) {
            key_arry[0] = i0_key_strm.read();
            last[0] = i0_e_strm.read();
        }
        if (rd_e[1]) {
            key_arry[1] = i1_key_strm.read();
            last[1] = i1_e_strm.read();
        }
        if (rd_e[2]) {
            key_arry[2] = i2_key_strm.read();
            last[2] = i2_e_strm.read();
        }
        if (rd_e[3]) {
            key_arry[3] = i3_key_strm.read();
            last[3] = i3_e_strm.read();
        }
        if (rd_e[4]) {
            key_arry[4] = i4_key_strm.read();
            last[4] = i4_e_strm.read();
        }
        if (rd_e[5]) {
            key_arry[5] = i5_key_strm.read();
            last[5] = i5_e_strm.read();
        }
        if (rd_e[6]) {
            key_arry[6] = i6_key_strm.read();
            last[6] = i6_e_strm.read();
        }
        if (rd_e[7]) {
            key_arry[7] = i7_key_strm.read();
            last[7] = i7_e_strm.read();
        }
        ap_uint<3> id = mux<8>(rd_e);
        ap_uint<KEYW> key = key_arry[id];
        bool valid_n = last[id];
        if (!valid_n && rd_e != 0) {
            cnt++;
            o_key_strm.write(key);
            o_e_strm.write(false);
        }
    } while (last != 255);
    o_e_strm.write(true);
    if (PAD) o_key_strm.write(0);
    mergedCnt = cnt;
#ifndef __SYNTHESIS__
    std::cout << "merge number=" << cnt << std::endl;
#endif
}

template <bool PAD>
void loadBuffer(ap_uint<32> start,
                ap_uint<32> numPairs,
                ap_uint<512>* pair,
                hls::stream<ap_uint<512> >& pairStream,
                hls::stream<bool>& pairStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================loadBuffer===================" << std::endl;
    std::cout << " start=" << start << " numPairs=" << numPairs << std::endl;
#endif

    for (unsigned i = 0; i < numPairs.range(31, 9); i++) {
        ap_uint<32> addr = i * 512 + start;
        for (unsigned j = 0; j < 512; j++) {
#pragma HLS PIPELINE II = 1

            ap_uint<512> tmpIn = pair[addr + j];
            pairStream.write(tmpIn);
            pairStreamEnd.write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_LOAD
            for (unsigned k = 0; k < 4; k++) {
                ap_uint<128> pair_in = tmpIn(128 * k + 127, 128 * k);
                ap_uint<32> src, des, idx;
                ap_uint<16> hop, card;
                src = pair_in(31, 0);
                des = pair_in(63, 32);
                idx = pair_in(95, 64);
                hop = pair_in(111, 96);
                card = pair_in(127, 112);

                if ((src == 6444) || (des == 2401764))
                    std::cout << "loadBase: i=" << i << " j=" << j << " k=" << k << " src=" << src << " des=" << des
                              << " idx=" << idx << " hop=" << hop << " card=" << card << std::endl;
            }
#endif
#endif
        }
    }

    for (unsigned i = 0; i < numPairs.range(8, 0); i++) {
#pragma HLS PIPELINE II = 1
        ap_uint<512> tmpIn = pair[numPairs.range(31, 9) * 512 + start + i];
        pairStream.write(tmpIn);
        pairStreamEnd.write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_LOAD
        for (unsigned k = 0; k < 4; k++) {
            ap_uint<128> pair_in = tmpIn(128 * k + 127, 128 * k);
            ap_uint<32> src, des, idx;
            ap_uint<16> hop, card;
            src = pair_in(31, 0);
            des = pair_in(63, 32);
            idx = pair_in(95, 64);
            hop = pair_in(111, 96);
            card = pair_in(127, 112);

            if ((src == 6444) || (des == 2401764))
                std::cout << "loadResidual: i=" << i << " k=" << k << " src=" << src << " des=" << des << " idx=" << idx
                          << " hop=" << hop << " card=" << card << std::endl;
        }
#endif
#endif
    }
    if (PAD) pairStream.write(0);
    pairStreamEnd.write(true);
}

template <bool PAD>
void loadStrm(ap_uint<32> numPairs,
              bool& batchEnd,
              hls::stream<ap_uint<512> >& pair,
              hls::stream<bool>& pairEnd,
              hls::stream<ap_uint<512> >& pairStream,
              hls::stream<bool>& pairStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================loadStrm===================" << std::endl;
    std::cout << "numPairs=" << numPairs << std::endl;
#endif

    bool end = batchEnd;
    ap_uint<32> cnt = 0;
    while (!end && (cnt < numPairs)) {
#pragma HLS PIPELINE II = 1
        if (!pair.empty()) {
            ap_uint<512> tmpIn = pair.read();
            end = pairEnd.read();

            ap_uint<512> tmpOut;
            for (unsigned k = 0; k < 4; k++) {
                ap_uint<128> pair_in = tmpIn(128 * k + 127, 128 * k);
                ap_uint<32> src, des, idx;
                ap_uint<16> card;
                ap_uint<8> hop;
                ap_uint<4> len, type;

                src = pair_in(127, 96);
                des = pair_in(95, 64);
                idx = pair_in(63, 32);
                hop = pair_in(31, 24);
                type = pair_in(23, 20);
                len = pair_in(19, 16);
                card = pair_in(15, 0);

                ap_uint<128> pair_out;
                pair_out(31, 0) = src;
                pair_out(63, 32) = des;
                pair_out(95, 64) = idx;
                pair_out(111, 96) = hop;
                pair_out(127, 112) = card;
                tmpOut(128 * k + 127, 128 * k) = pair_out;
#ifndef __SYNTHESIS__
#ifdef DEBUG_LOAD
                if ((src == 6444) || (des == 2401764))
                    std::cout << "loadStrm: k=" << k << " src=" << src << " des=" << des << " idx=" << idx
                              << " hop=" << hop << " card=" << card << std::endl;
#endif
#endif
            }

            if (tmpIn != 0) {
                pairStream.write(tmpOut);
                pairStreamEnd.write(false);
            }
        }
        cnt++;
    }
    batchEnd = end;
    if (PAD) pairStream.write(0);
    pairStreamEnd.write(true);
}

void splitPair(bool loadBatch,
               hls::stream<ap_uint<512> >& streamIn,
               hls::stream<bool>& streamInEnd,
               hls::stream<ap_uint<128> > pairStream[4],
               hls::stream<bool> pairStreamEnd[4]) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================splitPair===================" << std::endl;
#endif

    bool end = streamInEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1

        ap_uint<512> tmpIn = streamIn.read();
        end = streamInEnd.read();
        for (unsigned k = 0; k < 4; k++) {
            ap_uint<128> pair_in = tmpIn(128 * k + 127, 128 * k);
            ap_uint<32> src, des, idx;
            ap_uint<16> hop, card;
            src = pair_in(31, 0);
            des = pair_in(63, 32);
            idx = pair_in(95, 64);
            hop = pair_in(111, 96);
            card = pair_in(127, 112);

            ap_uint<128> tmp;
            tmp(63, 0) = pair_in(63, 0);
            if (loadBatch && (hop == 0)) {
                tmp(95, 64) = src;
                tmp(127, 96) = 0;
            } else {
                tmp(95, 64) = idx;
                tmp(127, 96) = hop;
            }

            if (tmp != 0) {
#ifndef __SYNTHESIS__
#ifdef DEBUG_LOAD
                if ((src == 6444) || (des == 2401764))
                    std::cout << "src=" << src << " des=" << des << " idx=" << tmp(95, 64) << " hop=" << tmp(111, 96)
                              << " card=" << tmp(127, 112) << std::endl;
#endif
#endif

                pairStream[k].write(tmp);
                pairStreamEnd[k].write(false);
            }
        }
    }

    for (unsigned k = 0; k < 4; k++) {
#pragma HLS UNROLL
        pairStreamEnd[k].write(true);
    }
}

void loadPair(bool loadBatch,
              bool& batchEnd,
              ap_uint<32> numPairs,
              hls::stream<ap_uint<512> >& pair,
              hls::stream<bool>& pairEnd,
              ap_uint<512>* pingPongBuffer,
              hls::stream<ap_uint<512> >& pairStream,
              hls::stream<bool>& pairStreamEnd) {
#pragma HLS INLINE off

    if (loadBatch) {
        loadStrm<false>(numPairs, batchEnd, pair, pairEnd, pairStream, pairStreamEnd);
    } else {
        loadBuffer<false>(0, numPairs, pingPongBuffer, pairStream, pairStreamEnd);
    }
}

void load(bool loadBatch,
          bool& batchEnd,
          ap_uint<32> numPairs,
          hls::stream<ap_uint<512> >& pair,
          hls::stream<bool>& pairEnd,
          ap_uint<512>* buffer,
          hls::stream<ap_uint<128> > pairStream[4],
          hls::stream<bool> pairStreamEnd[4]) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<512> > streamIn;
#pragma HLS stream variable = streamIn depth = 512
#pragma HLS resource variable = streamIn core = FIFO_BRAM
    hls::stream<bool> streamInEnd;
#pragma HLS stream variable = streamInEnd depth = 512
#pragma HLS resource variable = streamInEnd core = FIFO_SRL

    loadPair(loadBatch, batchEnd, numPairs, pair, pairEnd, buffer, streamIn, streamInEnd);

    splitPair(loadBatch, streamIn, streamInEnd, pairStream, pairStreamEnd);
}

template <int dispatchNum>
void dispatchSplitCSR(hls::stream<ap_uint<128> >& pairStream,
                      hls::stream<bool>& pairStreamEnd,
                      ap_uint<32> offsetTable[dispatchNum + 1],
                      hls::stream<ap_uint<128> > dispatchStream[dispatchNum],
                      hls::stream<bool> dispatchStreamEnd[dispatchNum]) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================dispatchOffset0===================" << std::endl;
#endif

    bool end = pairStreamEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1
        ap_uint<32> src, des, idx, hop;
        ap_uint<128> tmp0 = pairStream.read();
        end = pairStreamEnd.read();
        src = tmp0(31, 0);
        des = tmp0(63, 32);
        idx = tmp0(95, 64);
        hop = tmp0(127, 96);

        for (int j = dispatchNum - 1; j >= 0; j--) {
            if ((idx >= offsetTable[j]) && (idx < offsetTable[j + 1])) {
                ap_uint<128> tmp1;
                tmp1(31, 0) = src;
                tmp1(63, 32) = des;
                tmp1(95, 64) = idx - offsetTable[j];
                tmp1(127, 96) = hop;

                dispatchStream[j].write(tmp1);
                dispatchStreamEnd[j].write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_LOAD
                if ((src == 6444) || (des == 2401764))
                    std::cout << "dispatchID=" << j << " src=" << src << " des=" << des
                              << " idx=" << idx - offsetTable[j] << " offsetTable=" << offsetTable[j] << " hop=" << hop
                              << std::endl;
#endif
#endif
            }
        }
    }
    for (int i = 0; i < dispatchNum; i++) {
#pragma HLS UNROLL
        dispatchStream[i].write(0);
        dispatchStreamEnd[i].write(true);
    }
}

template <int dispatchNum>
void dispatchDuplicateCSR(hls::stream<ap_uint<128> >& pairStream,
                          hls::stream<bool>& pairStreamEnd,
                          ap_uint<32> offsetTable[dispatchNum + 1],
                          hls::stream<ap_uint<128> > dispatchStream[dispatchNum],
                          hls::stream<bool> dispatchStreamEnd[dispatchNum]) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================dispatchOffset1===================" << std::endl;
#endif

    ap_uint<8> j = 0;
    bool end = pairStreamEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1
        ap_uint<32> src, des, idx, hop;
        ap_uint<128> tmp0 = pairStream.read();
        end = pairStreamEnd.read();
        src = tmp0(31, 0);
        des = tmp0(63, 32);
        idx = tmp0(95, 64);
        hop = tmp0(127, 96);

        ap_uint<128> tmp1;
        tmp1(31, 0) = src;
        tmp1(63, 32) = des;
        tmp1(95, 64) = idx - offsetTable[j];
        tmp1(127, 96) = hop;
        dispatchStream[j].write(tmp1);
        dispatchStreamEnd[j].write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_LOAD
        if ((src == 6444) || (des == 2401764))
            std::cout << "dispatchID=" << j << " src=" << src << " des=" << des << " idx=" << idx - offsetTable[j]
                      << " offsetTable=" << offsetTable[j] << " hop=" << hop << std::endl;
#endif
#endif

        if (j == dispatchNum - 1)
            j = 0;
        else
            j++;
    }
    for (int i = 0; i < dispatchNum; i++) {
#pragma HLS UNROLL
        dispatchStream[i].write(0);
        dispatchStreamEnd[i].write(true);
    }
}

template <int PU>
void switchDispatch(ap_uint<32> duplicate,
                    ap_uint<32> offsetTable[PU + 1],
                    hls::stream<ap_uint<128> >& pairStream,
                    hls::stream<bool>& pairStreamEnd,
                    hls::stream<ap_uint<128> > dispatchStream[PU],
                    hls::stream<bool> dispatchStreamEnd[PU]) {
#pragma HLS INLINE off

    if (duplicate == 0) {
        dispatchSplitCSR<PU>(pairStream, pairStreamEnd, offsetTable, dispatchStream, dispatchStreamEnd);
    } else {
        dispatchDuplicateCSR<PU>(pairStream, pairStreamEnd, offsetTable, dispatchStream, dispatchStreamEnd);
    }
}

template <int PU>
void switchPairs(ap_uint<32> duplicate,
                 ap_uint<32>* offsetTable,
                 hls::stream<ap_uint<128> > strmIn[4],
                 hls::stream<bool> strmInEnd[4],

                 hls::stream<ap_uint<128> > strmOut[PU],
                 hls::stream<bool> strmOutEnd[PU]) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<128> > switchStrm[4][PU];
#pragma HLS stream variable = switchStrm depth = 8
#pragma HLS resource variable = switchStrm core = FIFO_SRL
    hls::stream<bool> switchStrmEnd[4][PU];
#pragma HLS stream variable = switchStrmEnd depth = 8
#pragma HLS resource variable = switchStrmEnd core = FIFO_SRL

    for (ap_uint<8> i = 0; i < 4; i++) {
#pragma HLS UNROLL

        switchDispatch<PU>(duplicate, offsetTable, strmIn[i], strmInEnd[i], switchStrm[i], switchStrmEnd[i]);
    }

    ap_uint<32> cnt[PU];
#pragma HLS ARRAY_PARTITION variable = cnt complete

    if (PU >= 1)
        merge4to1<128, false, true>(switchStrm[0][0], switchStrmEnd[0][0], switchStrm[1][0], switchStrmEnd[1][0],
                                    switchStrm[2][0], switchStrmEnd[2][0], switchStrm[3][0], switchStrmEnd[3][0],
                                    cnt[0], strmOut[0], strmOutEnd[0]);
    if (PU >= 2)
        merge4to1<128, false, true>(switchStrm[0][1], switchStrmEnd[0][1], switchStrm[1][1], switchStrmEnd[1][1],
                                    switchStrm[2][1], switchStrmEnd[2][1], switchStrm[3][1], switchStrmEnd[3][1],
                                    cnt[1], strmOut[1], strmOutEnd[1]);

    if (PU >= 4) {
        merge4to1<128, false, true>(switchStrm[0][2], switchStrmEnd[0][2], switchStrm[1][2], switchStrmEnd[1][2],
                                    switchStrm[2][2], switchStrmEnd[2][2], switchStrm[3][2], switchStrmEnd[3][2],
                                    cnt[2], strmOut[2], strmOutEnd[2]);

        merge4to1<128, false, true>(switchStrm[0][3], switchStrmEnd[0][3], switchStrm[1][3], switchStrmEnd[1][3],
                                    switchStrm[2][3], switchStrmEnd[2][3], switchStrm[3][3], switchStrmEnd[3][3],
                                    cnt[3], strmOut[3], strmOutEnd[3]);
    }
}

void loadHopOffset(ap_uint<32> numHop,
                   hls::stream<ap_uint<128> >& pairStream,
                   hls::stream<bool>& pairStreamEnd,
                   ap_uint<64>& accCNT,
                   ap_uint<32> indexShift,
                   unsigned* offset,
                   hls::stream<ap_uint<128> >& offsetStream,
                   hls::stream<bool>& offsetStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================loadHopOffset===================" << std::endl;
    std::cout << "indexShift=" << indexShift << std::endl;
    long cnt = 0;
#endif

    bool end = pairStreamEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 2
        ap_uint<32> src, des, idx, hop, offset_start, offset_end;
        ap_uint<128> tmp0 = pairStream.read();
        end = pairStreamEnd.read();

        src = tmp0(31, 0);
        des = tmp0(63, 32);
        idx = tmp0(95, 64);
        hop = tmp0(127, 96);

        ap_uint<128> tmp1;
        ap_uint<32> addr;
        offset_start = offset[idx];
        offset_end = offset[idx + 1];

        ap_uint<24> nm = offset_end - offset_start;
        tmp1(31, 0) = src;
        tmp1(63, 32) = des;
        tmp1(95, 64) = offset_start - indexShift;
        tmp1(119, 96) = nm;
        tmp1(127, 120) = hop + 1;

        accCNT += nm;

#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
        if ((src == 6444) || (des == 2401764))
            std::cout << "src=" << src << " des=" << des << " idx=" << idx << " hop=" << hop
                      << " offset_start=" << offset[idx] << " offset_end=" << offset[idx + 1]
                      << " offset=" << tmp1(95, 64) << " nm=" << tmp1(119, 96) << std::endl;
#endif
#endif

        if (nm > 0) {
#ifndef __SYNTHESIS__
            cnt += nm;
#endif
            offsetStream.write(tmp1);
            offsetStreamEnd.write(false);
        }
    }
    offsetStreamEnd.write(true);
#ifndef __SYNTHESIS__
    std::cout << "INFO : offset addr per batch = " << cnt << "\n";
#endif
}

void generateIndexAddr(hls::stream<ap_uint<128> >& offsetStream,
                       hls::stream<bool>& offsetStreamEnd,
                       hls::stream<ap_uint<128> >& addrStream,
                       hls::stream<bool>& addrStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================generateIndexAddr===================" << std::endl;
#endif

    ap_uint<32> src, des, hop, offset, addr;
    ap_uint<128> tmp0, tmp1;
    ap_uint<2> idx;

    bool end = offsetStreamEnd.read();
    ap_int<32> nm = 0;
    bool start = true;
    while (!end || nm > 0) {
#pragma HLS PIPELINE II = 1

        if (nm == 0) {
            tmp0 = offsetStream.read();
            end = offsetStreamEnd.read();

            src = tmp0(31, 0);
            des = tmp0(63, 32);
            offset = tmp0(95, 64);
            nm = tmp0(119, 96);
            hop = tmp0(127, 120);
            start = true;

            addr = offset(31, 2);
            idx = offset(1, 0);
        } else {
            ap_uint<4> enable;
            if (start) {
                for (int i = 0; i < 4; i++) {
                    if ((i >= idx) && (i < (nm + idx)))
                        enable[i] = 1;
                    else
                        enable[i] = 0;
                }
                if ((nm + idx) < 4) {
                    nm = 0;
                } else {
                    nm = nm + idx - 4;
                }
            } else {
                if (nm > 4) {
                    enable = 15;
                    nm = nm - 4;
                } else {
                    for (int i = 0; i < 4; i++) {
                        if (i < nm)
                            enable[i] = 1;
                        else
                            enable[i] = 0;
                    }
                    nm = 0;
                }
            }

            tmp1(31, 0) = src;
            tmp1(63, 32) = des;
            tmp1(95, 64) = addr;
            tmp1(99, 96) = enable;
            tmp1(127, 120) = hop;

#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
            if ((src == 6444) || (des == 2401764))
                std::cout << "IndexAddr src=" << src << " des=" << des << " offset=" << offset << " idx=" << idx
                          << " addr=" << addr << " enable=" << enable << " hop=" << hop << std::endl;
#endif
#endif

            addr++;
            start = false;

            addrStream.write(tmp1);
            addrStreamEnd.write(false);
        }
    }
    addrStreamEnd.write(true);
}

void loadIndex(ap_uint<32> numHop,
               ap_uint<32> intermediate,
               ap_uint<32> byPass,
               ap_uint<32> offsetStart,
               ap_uint<32> offsetEnd,

               hls::stream<ap_uint<128> >& offsetStream,
               hls::stream<bool>& offsetStreamEnd,
               ap_uint<128>* index,

               hls::stream<ap_uint<128> > localIndexStream[4],
               hls::stream<bool> localIndexStreamEnd[4],
               hls::stream<ap_uint<128> > netIndexStream[4],
               hls::stream<bool> netIndexStreamEnd[4],
               hls::stream<ap_uint<64> > aggrStream[4],
               hls::stream<bool> aggrStreamEnd[4],
               hls::stream<ap_uint<128> > outStream[4],
               hls::stream<bool> outStreamEnd[4]) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "========================loadHopIndex===================" << std::endl;
    std::cout << "offsetStart=" << offsetStart << " offsetEnd=" << offsetEnd << std::endl;
    int cnt = 0;
#endif

    bool end = offsetStreamEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1
        ap_uint<32> src, des, hop, addr;
        ap_uint<128> idx_tmp;
        ap_uint<32> idx[4];
        ap_uint<4> enable;
        ap_uint<128> tmp0;
        ap_uint<128> tmp1;
        ap_uint<64> tmp2;
        ap_uint<128> tmp3;

        tmp0 = offsetStream.read();
        end = offsetStreamEnd.read();
        src = tmp0(31, 0);
        des = tmp0(63, 32);
        addr = tmp0(95, 64);
        enable = tmp0(99, 96);
        hop = tmp0(127, 120);

        idx_tmp = index[addr];
        for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL

            idx[i] = idx_tmp(32 * i + 31, 32 * i);

            tmp1(31, 0) = src;
            tmp1(63, 32) = des;
            tmp1(95, 64) = idx[i];
            tmp1(127, 96) = hop;

            tmp2(31, 0) = src;
            tmp2(63, 32) = des;
 
            if (enable[i]) {
#ifndef __SYNTHESIS__
                cnt++;
#endif
                if ((hop < numHop) && (idx[i] >= offsetStart) && (idx[i] < offsetEnd)) {
                    // for local ping-pong hop
                    localIndexStream[i].write(tmp1);
                    localIndexStreamEnd[i].write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
                    if ((src == 6444) || (des == 2401764))
                        std::cout << "localResidual src=" << src << " des=" << des << " idx=" << idx[i]
                                  << " hop=" << hop << std::endl;
#endif
#endif
                }

                if ((idx[i] < offsetStart) || (idx[i] >= offsetEnd)) {
                    if ((hop < numHop) || ((hop == numHop) && (intermediate != 0))) {
                        // for index need send to network
                        netIndexStream[i].write(tmp1);
                        netIndexStreamEnd[i].write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
                        if ((src == 6444) || (des == 2401764))
                            std::cout << "netSwitch src=" << src << " des=" << des << " idx=" << idx[i]
                                      << " hop=" << hop << std::endl;
#endif
#endif
                    }
                }

                if ((idx[i] == des) && (byPass == 0)) {
                    // for internal aggr
                    aggrStream[i].write(tmp2);
                    aggrStreamEnd[i].write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
                    if ((src == 6444) || (des == 2401764))
                        std::cout << "internalAggr src=" << src << " des=" << des << " idx=" << idx[i] << " hop=" << hop
                                  << std::endl;
#endif
#endif
                }

                if ((hop == numHop) && (intermediate != 0) && (idx < offsetEnd) && (idx >= offsetStart)) {
                    tmp3 = tmp1;
                } else if (((idx[i] == des) && (byPass != 0))) {
                    tmp3(63, 0) = tmp2;
                    tmp3(95, 64) = 1;
                    tmp3(127, 96) = 0;
                } else {
                    tmp3 = 0;
                }

                if (((idx[i] == des) && (byPass != 0)) ||
                    ((hop == numHop) && (intermediate != 0) && (idx[i] < offsetEnd) && (idx[i] >= offsetStart))) {
                    // for output to local buffer
                    // (idx == des) && (byPass != 0) output aggr pair
                    // (hop == numHop) && (intermediate != 0) output intermediate hop
                    outStream[i].write(tmp3);
                    outStreamEnd[i].write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
                    if ((src == 6444) || (des == 2401764))
                        std::cout << "localResult src=" << src << " des=" << des << " idx=" << tmp3(85, 64)
                                  << " hop=" << tmp3(127, 96) << std::endl;
#endif
#endif
                }
            }
        }
    }
#ifndef __SYNTHESIS__
    std::cout << "INFO : access edge per batch = " << cnt << "\n";
#endif
    for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL

        outStream[i].write(0);
        outStreamEnd[i].write(true);
        aggrStream[i].write(0);
        aggrStreamEnd[i].write(true);
        netIndexStream[i].write(0);
        netIndexStreamEnd[i].write(true);
        localIndexStream[i].write(0);
        localIndexStreamEnd[i].write(true);
    }
}

template <int WIDTH, bool PAD, bool END>
void transfer2to1(hls::stream<ap_uint<WIDTH> >& inStream0,
                  hls::stream<bool>& inStreamEnd0,
                  hls::stream<ap_uint<WIDTH> >& inStream1,
                  hls::stream<bool>& inStreamEnd1,
                  ap_uint<32>& cnt,
                  hls::stream<ap_uint<2 * WIDTH> >& outStream,
                  hls::stream<bool>& outStreamEnd) {
#pragma HLS INLINE off

    ap_uint<32> count = 0;
    ap_uint<WIDTH> arry[2] = {0, 0};
#pragma HLS array_partition variable = arry dim = 1
    ap_uint<2> empty = 0;
    ap_uint<2> store = 0;
    ap_uint<2> last = 0;
    bool write = true;
LOOP_MERGE2_1:
    do {
#pragma HLS PIPELINE II = 1
        empty[0] = !inStreamEnd0.empty() && !last[0];
        empty[1] = !inStreamEnd1.empty() && !last[1];

        if (store == 3) {
            // wait for write
        } else if (store == 2) {
            // read for arry[0]
            if (empty == 3) {
                arry[0] = inStream0.read();
                last[0] = inStreamEnd0.read();
                store = 3;
            } else if (empty == 2) {
                arry[0] = inStream1.read();
                last[1] = inStreamEnd1.read();
                store = 3;
            } else if (empty == 1) {
                arry[0] = inStream0.read();
                last[0] = inStreamEnd0.read();
                store = 3;
            } else {
                store = 2;
            }
        } else if (store == 1) {
            // read for arry[1]
            if (empty == 3) {
                arry[1] = inStream1.read();
                last[1] = inStreamEnd1.read();
                store = 3;
            } else if (empty == 2) {
                arry[1] = inStream1.read();
                last[1] = inStreamEnd1.read();
                store = 3;
            } else if (empty == 1) {
                arry[1] = inStream0.read();
                last[0] = inStreamEnd0.read();
                store = 3;
            } else {
                store = 1;
            }
        } else {
            // store == 0, read for arry[0] and arry[1]
            if (empty == 3) {
                arry[0] = inStream0.read();
                last[0] = inStreamEnd0.read();
                arry[1] = inStream1.read();
                last[1] = inStreamEnd1.read();
                store = 3;
            } else if (empty == 2) {
                arry[1] = inStream1.read();
                last[1] = inStreamEnd1.read();
                store = 2;
            } else if (empty == 1) {
                arry[0] = inStream0.read();
                last[0] = inStreamEnd0.read();
                store = 1;
            } else {
                store = 0;
            }
        }

        ap_uint<2 * WIDTH> key = (arry[1], arry[0]);
        if ((store == 3) && !outStream.full() && !outStreamEnd.full()) {
            // write out
            if (key != 0) {
                outStream.write(key);
                outStreamEnd.write(false);
                count++;
#ifndef __SYNTHESIS__
#ifdef DEBUG_HOP
// std::cout << "Merge arry[0]=" << arry[0] << " arry[1]=" << arry[1] << std::endl;
#endif
#endif
            }
            arry[0] = 0;
            arry[1] = 0;
            store = 0;
        }
    } while (last != 3);
    if (store != 0) {
        // write out residual
        ap_uint<2 * WIDTH> key = (arry[1], arry[0]);
        outStream.write(key);
        outStreamEnd.write(false);
        count++;
    }
    cnt = count;
    if (PAD) outStream.write(0);
    if (END) outStreamEnd.write(true);
}

template <bool PAD, bool END>
void merge128to512(hls::stream<ap_uint<128> > inStream[4],
                   hls::stream<bool> inStreamEnd[4],
                   ap_uint<32>& cnt,
                   hls::stream<ap_uint<512> >& outStream,
                   hls::stream<bool>& outStreamEnd) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<256> > mergeStream[2];
#pragma HLS stream variable = mergeStream depth = 8
#pragma HLS resource variable = mergeStream core = FIFO_SRL
    hls::stream<bool> mergeStreamEnd[2];
#pragma HLS stream variable = mergeStreamEnd depth = 8
#pragma HLS resource variable = mergeStreamEnd core = FIFO_SRL

    ap_uint<32> tmp0;
    transfer2to1<128, true, true>(inStream[0], inStreamEnd[0], inStream[1], inStreamEnd[1], tmp0, mergeStream[0],
                                  mergeStreamEnd[0]);

    ap_uint<32> tmp1;
    transfer2to1<128, true, true>(inStream[2], inStreamEnd[2], inStream[3], inStreamEnd[3], tmp1, mergeStream[1],
                                  mergeStreamEnd[1]);

    transfer2to1<256, PAD, END>(mergeStream[0], mergeStreamEnd[0], mergeStream[1], mergeStreamEnd[1], cnt, outStream,
                                outStreamEnd);
}

template <bool PAD>
void loadHopMultipleIndex(ap_uint<32> numHop,
                          ap_uint<32> intermediate,
                          ap_uint<32> byPass,
                          ap_uint<32> offsetStart,
                          ap_uint<32> offsetEnd,
                          hls::stream<ap_uint<128> >& offsetStream,
                          hls::stream<bool>& offsetStreamEnd,
                          ap_uint<128>* index,
                          hls::stream<ap_uint<512> >& localIndexStream,
                          hls::stream<bool>& localIndexStreamEnd,
                          hls::stream<ap_uint<128> >& netIndexStream,
                          hls::stream<bool>& netIndexStreamEnd,
                          hls::stream<ap_uint<64> >& aggrOutStream,
                          hls::stream<bool>& aggrOutStreamEnd,
                          hls::stream<ap_uint<128> >& localResultStream,
                          hls::stream<bool>& localResultStreamEnd) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

#ifndef __SYNTHESIS__
    std::cout << "================Load Multiple Hop Index==============" << std::endl;
#endif

    hls::stream<ap_uint<128> > addrStream("addrStream");
#pragma HLS stream variable = addrStream depth = 8
#pragma HLS resource variable = addrStream core = FIFO_SRL
    hls::stream<bool> addrStreamEnd("addrStreamEnd");
#pragma HLS stream variable = addrStreamEnd depth = 8
#pragma HLS resource variable = addrStreamEnd core = FIFO_SRL

    generateIndexAddr(offsetStream, offsetStreamEnd, addrStream, addrStreamEnd);

    hls::stream<ap_uint<128> > localStream[4];
#pragma HLS stream variable = localStream depth = 8
#pragma HLS resource variable = localStream core = FIFO_SRL
    hls::stream<bool> localStreamEnd[4];
#pragma HLS stream variable = localStreamEnd depth = 8
#pragma HLS resource variable = localStreamEnd core = FIFO_SRL
    hls::stream<ap_uint<128> > netStream[4];
#pragma HLS stream variable = netStream depth = 8
#pragma HLS resource variable = netStream core = FIFO_SRL
    hls::stream<bool> netStreamEnd[4];
#pragma HLS stream variable = netStreamEnd depth = 8
#pragma HLS resource variable = netStreamEnd core = FIFO_SRL
    hls::stream<ap_uint<64> > aggrStream[4];
#pragma HLS stream variable = aggrStream depth = 8
#pragma HLS resource variable = aggrStream core = FIFO_SRL
    hls::stream<bool> aggrStreamEnd[4];
#pragma HLS stream variable = aggrStreamEnd depth = 8
#pragma HLS resource variable = aggrStreamEnd core = FIFO_SRL
    hls::stream<ap_uint<128> > resultStream[4];
#pragma HLS stream variable = resultStream depth = 8
#pragma HLS resource variable = resultStream core = FIFO_SRL
    hls::stream<bool> resultStreamEnd[4];
#pragma HLS stream variable = resultStreamEnd depth = 8
#pragma HLS resource variable = resultStreamEnd core = FIFO_SRL

    loadIndex(numHop, intermediate, byPass, offsetStart, offsetEnd, addrStream, addrStreamEnd, index, localStream,
              localStreamEnd, netStream, netStreamEnd, aggrStream, aggrStreamEnd, resultStream, resultStreamEnd);

#ifndef __SYNTHESIS__
    std::cout << "================Merge Local Index=============" << std::endl;
#endif

    ap_uint<32> tmp0;
    merge128to512<true, true>(localStream, localStreamEnd, tmp0, localIndexStream, localIndexStreamEnd);

#ifndef __SYNTHESIS__
    std::cout << "================Merge Net Stream==============" << std::endl;
#endif

    ap_uint<32> tmp1;
    merge4to1<128, false, true>(netStream[0], netStreamEnd[0], netStream[1], netStreamEnd[1], netStream[2],
                                netStreamEnd[2], netStream[3], netStreamEnd[3], tmp1, netIndexStream,
                                netIndexStreamEnd);

#ifndef __SYNTHESIS__
    std::cout << "===============Merge Result Stream============" << std::endl;
#endif

    ap_uint<32> tmp2;
    merge4to1<128, false, false>(resultStream[0], resultStreamEnd[0], resultStream[1], resultStreamEnd[1],
                                 resultStream[2], resultStreamEnd[2], resultStream[3], resultStreamEnd[3], tmp2,
                                 localResultStream, localResultStreamEnd);

#ifndef __SYNTHESIS__
    std::cout << "================Merge aggr Stream=============" << std::endl;
#endif

    ap_uint<32> tmp3;
    merge4to1<64, PAD, true>(aggrStream[0], aggrStreamEnd[0], aggrStream[1], aggrStreamEnd[1], aggrStream[2],
                             aggrStreamEnd[2], aggrStream[3], aggrStreamEnd[3], tmp3, aggrOutStream, aggrOutStreamEnd);
}

template <int PU>
void hashProcess(ap_uint<32> byPass,
                 ap_uint<32> hashSize,
                 hls::stream<ap_uint<64> >& hopStream,
                 hls::stream<bool>& hopStreamEnd,
                 hls::stream<ap_uint<96> >& aggrStream,
                 hls::stream<bool>& aggrStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================hashProcess===================" << std::endl;
#endif
    bool end = hopStreamEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1

        ap_uint<32> src, des;
        ap_uint<64> tmp0 = hopStream.read();
        end = hopStreamEnd.read();

        src = tmp0(31, 0);
        des = tmp0(63, 32);

        ap_uint<96> tmp1;
        ap_uint<64> key = (des, src);
        ap_uint<64> hash;
        database::details::hashlookup3_seed_core<64>(key, 0xbeef, hash);
        tmp1(63, 0) = tmp0(63, 0);
        if (hashSize <= 4096) {
            tmp1(75 + PU, 64) = hash(11 + PU, 0);
            tmp1(95, 76 + PU) = 0;
        } else if (hashSize <= 16384) {
            tmp1(77 + PU, 64) = hash(13 + PU, 0);
            tmp1(95, 80 + PU) = 0;
        } else if (hashSize <= 65536) {
            tmp1(79 + PU, 64) = hash(15 + PU, 0);
            tmp1(95, 80 + PU) = 0;
        } else
            tmp1(95, 64) = hash(31, 0);

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
        if ((src == 6444) || (des == 2401764))
            std::cout << "aggrStream: src=" << src << " des=" << des << " hash=" << tmp1(95, 64) << std::endl;
#endif
#endif
        if (byPass == 0) {
            aggrStream.write(tmp1);
            aggrStreamEnd.write(false);
        }
    }
    aggrStreamEnd.write(true);
}

template <int depth, int PU>
void aggrCounter(hls::stream<ap_uint<96> >& aggrStream,
                 hls::stream<bool>& aggrStreamEnd,
                 ap_uint<32>& numAggr,
#ifndef __SYNTHESIS__
                 ap_uint<96>* aggrURAM[1 << PU],
#else
                 ap_uint<96> aggrURAM[1 << PU][1 << depth],
#endif
                 hls::stream<ap_uint<64> >& overflowStream,
                 hls::stream<bool>& overflowStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================aggrCounter===================" << std::endl;
#endif
    const int maxBatch = 1 << depth;
    ap_uint<96> elem = 0;
    ap_uint<96> elem_temp[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ap_uint<depth + PU> idx_temp[12] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
#pragma HLS array_partition variable = elem_temp complete
#pragma HLS array_partition variable = idx_temp complete

    bool end = aggrStreamEnd.read();

    ap_uint<32> src, des, hash;
    ap_uint<depth + PU> arry_idx;
aggrCountLoop:
    while (!end) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = aggrURAM pointer inter false

        ap_uint<96> tmp0 = aggrStream.read();
        end = aggrStreamEnd.read();

        src = tmp0(31, 0);
        des = tmp0(63, 32);
        hash = tmp0(95, 64);
        arry_idx = hash(depth + PU - 1, 0);

        if (arry_idx == idx_temp[0]) {
            elem = elem_temp[0];
        } else if (arry_idx == idx_temp[1]) {
            elem = elem_temp[1];
        } else if (arry_idx == idx_temp[2]) {
            elem = elem_temp[2];
        } else if (arry_idx == idx_temp[3]) {
            elem = elem_temp[3];
        } else if (arry_idx == idx_temp[4]) {
            elem = elem_temp[4];
        } else if (arry_idx == idx_temp[5]) {
            elem = elem_temp[5];
        } else if (arry_idx == idx_temp[6]) {
            elem = elem_temp[6];
        } else if (arry_idx == idx_temp[7]) {
            elem = elem_temp[7];
        } else if (arry_idx == idx_temp[8]) {
            elem = elem_temp[8];
        } else if (arry_idx == idx_temp[9]) {
            elem = elem_temp[9];
        } else if (arry_idx == idx_temp[10]) {
            elem = elem_temp[10];
        } else if (arry_idx == idx_temp[11]) {
            elem = elem_temp[11];
        } else {
            elem = aggrURAM[arry_idx(PU - 1, 0)][arry_idx(depth + PU - 1, PU)];
        }

        ap_uint<64> key = elem(63, 0);
        ap_uint<32> pld = elem(95, 64);
        ap_uint<96> new_elem;

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
        if ((src == 6444) || (des == 2401764))
            std::cout << "base: src=" << src << " des=" << des << " chip_idx=" << arry_idx(PU - 1, 0)
                      << " arry_idx=" << arry_idx(depth + PU - 1, PU) << " (des, src)=" << (des, src) << " key=" << key
                      << " pld=" << pld << std::endl;
#endif
#endif

        if ((key == (des, src)) || (elem == 0)) {
            new_elem(95, 64) = pld + 1;
            new_elem(63, 32) = des;
            new_elem(31, 0) = src;
            aggrURAM[arry_idx(PU - 1, 0)][arry_idx(depth + PU - 1, PU)] = new_elem;
        } else {
            overflowStream.write((des, src));
            overflowStreamEnd.write(false);
        }
        numAggr++;

        for (int i = 11; i > 0; i--) {
#pragma HLS unroll
            idx_temp[i] = idx_temp[i - 1];
            elem_temp[i] = elem_temp[i - 1];
        }
        idx_temp[0] = arry_idx;
        elem_temp[0] = new_elem;
    }
    overflowStreamEnd.write(true);
}

template <int PU>
void overflowCounter(hls::stream<ap_uint<64> >& overflowStream,
                     hls::stream<bool>& overflowStreamEnd,
#ifndef __SYNTHESIS__
                     ap_uint<96>* overflowURAM[1 << PU]
#else
                     ap_uint<96> overflowURAM[1 << PU][4096]
#endif
                     ) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================overflowCounter===================" << std::endl;
#endif

    bool end = overflowStreamEnd.read();
    ap_uint<32> max_cnt = 0;
    bool write_success = true;
    ap_uint<64> key_in = 0;
    ap_uint<13> idx = 1;
    ap_uint<3> chip = 0;

overflowCountLoop:
    while (!end || !write_success) {
#pragma HLS PIPELINE II = 1

        if (write_success) {
            key_in = overflowStream.read();
            end = overflowStreamEnd.read();

            chip = 0;
            idx = 1;
        } else {
            if (PU == 3) {
                if (chip == 7) {
                    idx++;
                    chip = 0;
                } else {
                    chip++;
                }
            } else if (PU == 2) {
                if (chip == 3) {
                    idx++;
                    chip = 0;
                } else {
                    chip++;
                }
            } else if (PU == 1) {
                if (chip == 1) {
                    idx++;
                    chip = 0;
                } else {
                    chip++;
                }
            } else {
                idx++;
                chip = 0;
            }
        }

        ap_uint<96> elem = overflowURAM[chip][idx];
        ap_uint<64> key = elem(63, 0);
        ap_uint<32> pld = elem(95, 64);

        if ((key == key_in) || (elem == 0)) {
            write_success = true;

            ap_uint<96> new_elem;
            new_elem(63, 0) = key_in;
            new_elem(95, 64) = pld + 1;
            overflowURAM[chip][idx] = new_elem;
        } else {
            write_success = false;
        }
        if (idx > max_cnt) max_cnt = idx;

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
        ap_uint<32> src, des;
        src = key_in(31, 0);
        des = key_in(63, 32);

        if ((src == 6444) || (des == 2401764))
            std::cout << "overflow: src=" << src << " des=" << des << " chip_idx=" << chip << " arry_idx=" << idx
                      << " (des, src)=" << key_in << " key=" << key << " pld=" << pld << std::endl;
#endif
#endif
    }
    for (int i = 0; i < PU; i++) overflowURAM[i][0] = max_cnt;
}

template <int depth, int PU>
void hashAggrCounter(ap_uint<32> byPass,
                     ap_uint<32> hashSize,
                     hls::stream<ap_uint<64> >& hopStream,
                     hls::stream<bool>& hopStreamEnd,
                     ap_uint<32>& numAggr,
#ifndef __SYNTHESIS__
                     ap_uint<96>* aggrURAM[1 << PU],
                     ap_uint<96>* overflowURAM[1 << PU]
#else
                     ap_uint<96> aggrURAM[1 << PU][1 << depth],
                     ap_uint<96> overflowURAM[1 << PU][4096]
#endif

                     ) {
#pragma HLS DATAFLOW
#pragma HLS INLINE off

    hls::stream<ap_uint<96> > aggrStream("aggrStream");
#pragma HLS stream variable = aggrStream depth = 512
#pragma HLS resource variable = aggrStream core = FIFO_BRAM
    hls::stream<bool> aggrStreamEnd("aggrStreamEnd");
#pragma HLS stream variable = aggrStreamEnd depth = 512
#pragma HLS resource variable = aggrStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<64> > overflowStream("overflowStream");
#pragma HLS stream variable = overflowStream depth = 512
#pragma HLS resource variable = overflowStream core = FIFO_BRAM
    hls::stream<bool> overflowStreamEnd("overflowStreamEnd");
#pragma HLS stream variable = overflowStreamEnd depth = 512
#pragma HLS resource variable = overflowStreamEnd core = FIFO_SRL

    hashProcess<PU>(byPass, hashSize, hopStream, hopStreamEnd, aggrStream, aggrStreamEnd);

    aggrCounter<depth, PU>(aggrStream, aggrStreamEnd, numAggr, aggrURAM, overflowStream, overflowStreamEnd);

    overflowCounter<PU>(overflowStream, overflowStreamEnd, overflowURAM);
}

template <int PU>
void multiHashProcess(ap_uint<32> byPass,
                      ap_uint<32> hashSize,
                      hls::stream<ap_uint<64> >& hopStream,
                      hls::stream<bool>& hopStreamEnd,
                      hls::stream<ap_uint<96> > aggrStream[1 << PU],
                      hls::stream<bool> aggrStreamEnd[1 << PU]) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================hashProcess===================" << std::endl;
#endif
    bool end = hopStreamEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1

        ap_uint<32> src, des;
        ap_uint<64> tmp0 = hopStream.read();
        end = hopStreamEnd.read();

        src = tmp0(31, 0);
        des = tmp0(63, 32);

        ap_uint<96> tmp1;
        ap_uint<64> key = (des, src);
        ap_uint<64> hash;
        database::details::hashlookup3_seed_core<64>(key, 0xbeef, hash);
        tmp1(63, 0) = tmp0(63, 0);
        if (hashSize <= 4096) {
            tmp1(75, 64) = hash(11, 0);
            tmp1(95, 76) = 0;
        } else if (hashSize <= 16384) {
            tmp1(77, 64) = hash(13, 0);
            tmp1(95, 80) = 0;
        } else if (hashSize <= 65536) {
            tmp1(79, 64) = hash(15, 0);
            tmp1(95, 80) = 0;
        } else
            tmp1(95, 64) = hash(31, 0);

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
        if ((src == 6444) || (des == 2401764))
            std::cout << "aggrStream: src=" << src << " des=" << des << " hash=" << tmp1(95, 64) << std::endl;
#endif
#endif
        if (byPass == 0) {
            aggrStream[hash(31 + PU, 32)].write(tmp1);
            aggrStreamEnd[hash(31 + PU, 32)].write(false);
        }
    }
    for (int i = 0; i < (1 << PU); i++) {
        aggrStream[i].write(0);
        aggrStreamEnd[i].write(true);
    }
}

template <int depth>
void multiAggrCounter(hls::stream<ap_uint<96> >& aggrStream,
                      hls::stream<bool>& aggrStreamEnd,
                      ap_uint<32>& numAggr,
#ifndef __SYNTHESIS__
                      ap_uint<96>* aggrURAM,
#else
                      ap_uint<96> aggrURAM[1 << depth],
#endif
                      hls::stream<ap_uint<64> >& overflowStream,
                      hls::stream<bool>& overflowStreamEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================aggrCounter===================" << std::endl;
#endif
    const int maxBatch = 1 << depth;
    ap_uint<96> elem = 0;
    ap_uint<96> elem_temp[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ap_uint<depth> idx_temp[12] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                   0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
#pragma HLS array_partition variable = elem_temp complete
#pragma HLS array_partition variable = idx_temp complete

    bool end = aggrStreamEnd.read();
    numAggr = 0;
    ap_uint<32> src, des, hash;
    ap_uint<depth> arry_idx;
aggrCountLoop:
    while (!end) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = aggrURAM pointer inter false

        ap_uint<96> tmp0 = aggrStream.read();
        end = aggrStreamEnd.read();

        src = tmp0(31, 0);
        des = tmp0(63, 32);
        hash = tmp0(95, 64);
        arry_idx = hash(depth - 1, 0);

        if (arry_idx == idx_temp[0]) {
            elem = elem_temp[0];
        } else if (arry_idx == idx_temp[1]) {
            elem = elem_temp[1];
        } else if (arry_idx == idx_temp[2]) {
            elem = elem_temp[2];
        } else if (arry_idx == idx_temp[3]) {
            elem = elem_temp[3];
        } else if (arry_idx == idx_temp[4]) {
            elem = elem_temp[4];
        } else if (arry_idx == idx_temp[5]) {
            elem = elem_temp[5];
        } else if (arry_idx == idx_temp[6]) {
            elem = elem_temp[6];
        } else if (arry_idx == idx_temp[7]) {
            elem = elem_temp[7];
        } else if (arry_idx == idx_temp[8]) {
            elem = elem_temp[8];
        } else if (arry_idx == idx_temp[9]) {
            elem = elem_temp[9];
        } else if (arry_idx == idx_temp[10]) {
            elem = elem_temp[10];
        } else if (arry_idx == idx_temp[11]) {
            elem = elem_temp[11];
        } else {
            elem = aggrURAM[arry_idx];
        }

        ap_uint<64> key = elem(63, 0);
        ap_uint<32> pld = elem(95, 64);
        ap_uint<96> new_elem;

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
        if ((src == 6444) || (des == 2401764))
            std::cout << "base: src=" << src << " des=" << des << " arry_idx=" << arry_idx
                      << " (des, src)=" << (des, src) << " key=" << key << " pld=" << pld << std::endl;
#endif
#endif

        if ((key == (des, src)) || (elem == 0)) {
            new_elem(95, 64) = pld + 1;
            new_elem(63, 32) = des;
            new_elem(31, 0) = src;
            aggrURAM[arry_idx] = new_elem;
        } else {
            new_elem = elem;
            overflowStream.write((des, src));
            overflowStreamEnd.write(false);
        }

        if ((key != (des, src)) || (elem == 0)) numAggr++;

        for (int i = 11; i > 0; i--) {
#pragma HLS unroll
            idx_temp[i] = idx_temp[i - 1];
            elem_temp[i] = elem_temp[i - 1];
        }
        idx_temp[0] = arry_idx;
        elem_temp[0] = new_elem;
    }
    overflowStreamEnd.write(true);
}

void multiOverflowCounter(hls::stream<ap_uint<64> >& overflowStream,
                          hls::stream<bool>& overflowStreamEnd,
#ifndef __SYNTHESIS__
                          ap_uint<96>* overflowURAM
#else
                          ap_uint<96> overflowURAM[4096]
#endif
                          ) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================overflowCounter===================" << std::endl;
#endif

    bool end = overflowStreamEnd.read();
    ap_uint<32> max_cnt = overflowURAM[0];
    bool write_success = true;
    ap_uint<64> key_in = 0;
    ap_uint<13> idx = 1;

overflowCountLoop:
    while (!end || !write_success) {
#pragma HLS PIPELINE II = 1

        if (write_success) {
            key_in = overflowStream.read();
            end = overflowStreamEnd.read();
            idx = 1;
        } else {
            idx++;
        }

        ap_uint<96> elem = overflowURAM[idx];
        ap_uint<64> key = elem(63, 0);
        ap_uint<32> pld = elem(95, 64);

        if ((key == key_in) || (elem == 0)) {
            write_success = true;

            ap_uint<96> new_elem;
            new_elem(63, 0) = key_in;
            new_elem(95, 64) = pld + 1;
            overflowURAM[idx] = new_elem;
        } else {
            write_success = false;
        }
        if (idx > max_cnt) max_cnt = idx;

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
        ap_uint<32> src, des;
        src = key_in(31, 0);
        des = key_in(63, 32);

        if ((src == 6444) || (des == 2401764))
            std::cout << "overflow: src=" << src << " des=" << des << " arry_idx=" << idx << " (des, src)=" << key_in
                      << " key=" << key << " pld=" << pld << std::endl;
#endif
#endif
    }
    overflowURAM[0] = max_cnt;
}

template <int PU>
void maxAggrNum(ap_uint<32> num[1 << PU], ap_uint<32>& numAggr) {
#pragma HLS INLINE off

    numAggr = 0;
    for (int i = 0; i < (1 << PU); i++) {
        if (num[i] > numAggr) numAggr = num[i];
    }
}

template <int depth, int hopPU, int aggrPU>
void multiHashAggrCounter(ap_uint<32> byPass,
                          ap_uint<32> hashSize,
                          hls::stream<ap_uint<64> > hopStream[hopPU],
                          hls::stream<bool> hopStreamEnd[hopPU],
                          ap_uint<32>& numAggr,
#ifndef __SYNTHESIS__
                          ap_uint<96>* aggrURAM[1 << aggrPU],
                          ap_uint<96>* overflowURAM[1 << aggrPU]
#else
                          ap_uint<96> aggrURAM[1 << aggrPU][1 << depth],
                          ap_uint<96> overflowURAM[1 << aggrPU][4096]
#endif

                          ) {
#pragma HLS DATAFLOW
#pragma HLS INLINE off

    ap_uint<32> num[1 << aggrPU];
#pragma HLS ARRAY_PARTITION variable = num complete
    ap_uint<32> tmp[1 << aggrPU];
#pragma HLS ARRAY_PARTITION variable = tmp complete

    hls::stream<ap_uint<96> > aggrStream[hopPU][1 << aggrPU];
#pragma HLS stream variable = aggrStream depth = 8
#pragma HLS resource variable = aggrStream core = FIFO_SRL
    hls::stream<bool> aggrStreamEnd[hopPU][1 << aggrPU];
#pragma HLS stream variable = aggrStreamEnd depth = 8
#pragma HLS resource variable = aggrStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<96> > mergeStream[1 << aggrPU];
#pragma HLS stream variable = mergeStream depth = 8
#pragma HLS resource variable = mergeStream core = FIFO_SRL
    hls::stream<bool> mergeStreamEnd[1 << aggrPU];
#pragma HLS stream variable = mergeStreamEnd depth = 8
#pragma HLS resource variable = mergeStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<64> > overflowStream[1 << aggrPU];
#pragma HLS stream variable = overflowStream depth = 8
#pragma HLS resource variable = overflowStream core = FIFO_SRL
    hls::stream<bool> overflowStreamEnd[1 << aggrPU];
#pragma HLS stream variable = overflowStreamEnd depth = 8
#pragma HLS resource variable = overflowStreamEnd core = FIFO_SRL

    for (int i = 0; i < hopPU; i++) {
#pragma HLS UNROLL

        multiHashProcess<aggrPU>(byPass, hashSize, hopStream[i], hopStreamEnd[i], aggrStream[i], aggrStreamEnd[i]);
    }

    if ((hopPU == 2) && ((1 << aggrPU) == 2)) {
        merge2to1<96, false, true>(aggrStream[0][0], aggrStreamEnd[0][0], aggrStream[1][0], aggrStreamEnd[1][0], tmp[0],
                                   mergeStream[0], mergeStreamEnd[0]);

        merge2to1<96, false, true>(aggrStream[0][1], aggrStreamEnd[0][1], aggrStream[1][1], aggrStreamEnd[1][1], tmp[1],
                                   mergeStream[1], mergeStreamEnd[1]);
    } else if ((hopPU == 4) && ((1 << aggrPU) == 2)) {
        merge4to1<96, false, true>(aggrStream[0][0], aggrStreamEnd[0][0], aggrStream[1][0], aggrStreamEnd[1][0],
                                   aggrStream[2][0], aggrStreamEnd[2][0], aggrStream[3][0], aggrStreamEnd[3][0], tmp[0],
                                   mergeStream[0], mergeStreamEnd[0]);

        merge4to1<96, false, true>(aggrStream[0][1], aggrStreamEnd[0][1], aggrStream[1][1], aggrStreamEnd[1][1],
                                   aggrStream[2][1], aggrStreamEnd[2][1], aggrStream[3][1], aggrStreamEnd[3][1], tmp[1],
                                   mergeStream[1], mergeStreamEnd[1]);
    } else if ((hopPU == 4) && ((1 << aggrPU) == 4)) {
        merge4to1<96, false, true>(aggrStream[0][0], aggrStreamEnd[0][0], aggrStream[1][0], aggrStreamEnd[1][0],
                                   aggrStream[2][0], aggrStreamEnd[2][0], aggrStream[3][0], aggrStreamEnd[3][0], tmp[0],
                                   mergeStream[0], mergeStreamEnd[0]);

        merge4to1<96, false, true>(aggrStream[0][1], aggrStreamEnd[0][1], aggrStream[1][1], aggrStreamEnd[1][1],
                                   aggrStream[2][1], aggrStreamEnd[2][1], aggrStream[3][1], aggrStreamEnd[3][1], tmp[1],
                                   mergeStream[1], mergeStreamEnd[1]);

        merge4to1<96, false, true>(aggrStream[0][2], aggrStreamEnd[0][2], aggrStream[1][2], aggrStreamEnd[1][2],
                                   aggrStream[2][2], aggrStreamEnd[2][2], aggrStream[3][2], aggrStreamEnd[3][2], tmp[2],
                                   mergeStream[2], mergeStreamEnd[2]);

        merge4to1<96, false, true>(aggrStream[0][3], aggrStreamEnd[0][3], aggrStream[1][3], aggrStreamEnd[1][3],
                                   aggrStream[2][3], aggrStreamEnd[2][3], aggrStream[3][3], aggrStreamEnd[3][3], tmp[3],
                                   mergeStream[3], mergeStreamEnd[3]);
    }

    for (int i = 0; i < (1 << aggrPU); i++) {
#pragma HLS UNROLL

        multiAggrCounter<depth>(mergeStream[i], mergeStreamEnd[i], num[i], aggrURAM[i], overflowStream[i],
                                overflowStreamEnd[i]);

        multiOverflowCounter(overflowStream[i], overflowStreamEnd[i], overflowURAM[i]);
    }

    maxAggrNum<aggrPU>(num, numAggr);
}

template <bool PAD, bool END>
void streamTransfer128to512(hls::stream<ap_uint<128> >& strmIn,
                            hls::stream<bool>& strmInEnd,
                            hls::stream<ap_uint<512> >& strmOut,
                            hls::stream<bool>& strmOutEnd) {
#pragma HLS INLINE off

    bool end = strmInEnd.read();
    ap_uint<512> tmpOut = 0;
    ap_uint<2> j = 0;
    while (!end) {
#pragma HLS PIPELINE II = 1

        ap_uint<128> tmpIn = strmIn.read();
        end = strmInEnd.read();

        if (j == 0) {
            tmpOut(127, 0) = tmpIn;
            tmpOut(511, 128) = 0;
        } else if (j == 1) {
            tmpOut(255, 128) = tmpIn;
            tmpOut(511, 256) = 0;
        } else if (j == 2) {
            tmpOut(383, 256) = tmpIn;
            tmpOut(511, 384) = 0;
        } else {
            tmpOut(511, 384) = tmpIn;
            strmOut.write(tmpOut);
            strmOutEnd.write(false);
        }
        j++;
    }
    if (j != 0) {
        strmOut.write(tmpOut);
        strmOutEnd.write(false);
    }
    if (PAD) strmOut.write(0);
    if (END) strmOutEnd.write(true);
}

template <bool PAD, bool END>
void streamTransfer64to512(hls::stream<ap_uint<128> >& strmIn,
                           hls::stream<bool>& strmInEnd,
                           hls::stream<ap_uint<512> >& strmOut,
                           hls::stream<bool>& strmOutEnd) {
#pragma HLS INLINE off

    bool end = strmInEnd.read();
    ap_uint<512> tmpOut = 0;
    ap_uint<3> j = 0;
    while (!end) {
#pragma HLS PIPELINE II = 1

        ap_uint<128> tmp = strmIn.read();
        ap_uint<64> tmpIn = tmp(63, 0);
        end = strmInEnd.read();

        if (j == 0) {
            tmpOut(63, 0) = tmpIn;
            tmpOut(511, 64) = 0;
        } else if (j == 1) {
            tmpOut(127, 64) = tmpIn;
            tmpOut(511, 127) = 0;
        } else if (j == 2) {
            tmpOut(195, 128) = tmpIn;
            tmpOut(511, 196) = 0;
        } else if (j == 3) {
            tmpOut(255, 192) = tmpIn;
            tmpOut(511, 256) = 0;
        } else if (j == 4) {
            tmpOut(319, 256) = tmpIn;
            tmpOut(511, 320) = 0;
        } else if (j == 5) {
            tmpOut(383, 320) = tmpIn;
            tmpOut(511, 384) = 0;
        } else if (j == 6) {
            tmpOut(447, 384) = tmpIn;
            tmpOut(511, 448) = 0;
        } else {
            tmpOut(511, 448) = tmpIn;
            strmOut.write(tmpOut);
            strmOutEnd.write(false);
        }
        j++;
    }
    if (j != 0) {
        strmOut.write(tmpOut);
        strmOutEnd.write(false);
    }
    if (PAD) strmOut.write(0);
    if (END) strmOutEnd.write(true);
}

void nHopStreamTransfer(ap_uint<32> intermediate,
                        ap_uint<32> byPass,
                        hls::stream<ap_uint<128> >& strmIn,
                        hls::stream<bool>& strmInEnd,
                        hls::stream<ap_uint<512> >& strmOut,
                        hls::stream<bool>& strmOutEnd) {
#pragma HLS INLINE off

    if ((byPass != 0) && (intermediate == 0)) {
#ifndef __SYNTHESIS__
        std::cout << "transfer 64bit stream to 512bit" << std::endl;
#endif
        streamTransfer64to512<true, true>(strmIn, strmInEnd, strmOut, strmOutEnd);
    } else {
#ifndef __SYNTHESIS__
        std::cout << "transfer 128bit stream to 512bit" << std::endl;
#endif
        streamTransfer128to512<true, true>(strmIn, strmInEnd, strmOut, strmOutEnd);
    }
}

template <bool multiAggr>
void hopProcessingUnit(ap_uint<32> numHop,
                       ap_uint<32> intermediate,
                       ap_uint<32> byPass,
                       hls::stream<ap_uint<128> >& dispatchStream,
                       hls::stream<bool>& dispatchStreamEnd,

                       ap_uint<32> offsetStart,
                       ap_uint<32> offsetEnd,
                       ap_uint<32> indexTable,
                       ap_uint<64>& accCNT,

                       unsigned* offset,
                       ap_uint<128>* index,

                       hls::stream<ap_uint<512> >& localStream,
                       hls::stream<bool>& localStreamEnd,
                       hls::stream<ap_uint<128> >& networkStream,
                       hls::stream<bool>& networkStreamEnd,
                       hls::stream<ap_uint<64> >& aggrStream,
                       hls::stream<bool>& aggrStreamEnd,
                       hls::stream<ap_uint<128> >& outStream,
                       hls::stream<bool>& outStreamEnd) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<128> > offsetStream;
#pragma HLS stream variable = offsetStream depth = 512
#pragma HLS resource variable = offsetStream core = FIFO_BRAM
    hls::stream<bool> offsetStreamEnd;
#pragma HLS stream variable = offsetStreamEnd depth = 512
#pragma HLS resource variable = offsetStreamEnd core = FIFO_SRL

    loadHopOffset(numHop, dispatchStream, dispatchStreamEnd, accCNT, indexTable, offset, offsetStream, offsetStreamEnd);

    loadHopMultipleIndex<!multiAggr>(numHop, intermediate, byPass, offsetStart, offsetEnd, offsetStream,
                                     offsetStreamEnd, index, localStream, localStreamEnd, networkStream,
                                     networkStreamEnd, aggrStream, aggrStreamEnd, outStream, outStreamEnd);
}

template <int numSubGraph>
void getGraphID(hls::stream<ap_uint<128> >& indexStreamIn,
                hls::stream<bool>& indexStreamInEnd,
                ap_uint<64> cardTable[numSubGraph],
                hls::stream<ap_uint<128> >& indexStreamOut,
                hls::stream<bool>& indexStreamOutEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
    std::cout << "=======================GetGraphID===================" << std::endl;
#endif

    bool end = indexStreamInEnd.read();
    while (!end) {
#pragma HLS PIPELINE II = 1

        ap_uint<32> src, des, idx;
        ap_uint<16> id;
        ap_uint<4> hop;
        ap_uint<12> copy;
        ap_uint<128> tmp0, tmp1;
        tmp0 = indexStreamIn.read();
        end = indexStreamInEnd.read();

        src = tmp0(31, 0);
        des = tmp0(63, 32);
        idx = tmp0(95, 64);
        hop = tmp0(111, 96);

        if (idx >= cardTable[numSubGraph - 1](31, 0)) {
            id = cardTable[numSubGraph - 1](47, 32);
        } else {
            for (int i = 0; i < numSubGraph - 1; i++) {
#pragma HLS UNROLL
                ap_uint<32> offsetStart = cardTable[i](31, 0);
                ap_uint<32> offsetEnd = cardTable[i + 1](31, 0);
                ap_uint<16> graphId = i;
                ap_uint<16> nm = cardTable[i](63, 32);
                if ((idx < offsetEnd) && (idx >= offsetStart)) {
                    id = graphId;
                    copy = nm;
                }
            }
        }
#ifndef __SYNTHESIS__
#ifdef DEBUG_NET
        if ((src == 6444) || (des == 2401764))
            std::cout << "src=" << src << " des=" << des << " idx=" << idx << " hop=" << hop << " id=" << id
                      << std::endl;
#endif
#endif

        tmp1(31, 0) = src;
        tmp1(63, 32) = des;
        tmp1(95, 64) = idx;
        tmp1(99, 96) = hop;
        tmp1(111, 100) = copy;
        tmp1(127, 112) = id;

        if (tmp0 != 0) {
            indexStreamOut.write(tmp1);
            indexStreamOutEnd.write(false);
        }
    }
    indexStreamOutEnd.write(true);
}

template <int numSubGraph>
void gatherGraphID(ap_uint<32> numKnlPar,
                   hls::stream<ap_uint<128> >& strmIn,
                   hls::stream<bool>& strmInEnd,
                   hls::stream<ap_uint<512> >& strmOut,
                   hls::stream<bool>& strmOutEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
#ifdef DEBUG_NET
    std::cout << "=====================GatherID=================" << std::endl;
#endif
#endif

    const int depth = 64;
    ap_uint<128> netURAM[depth * numSubGraph];
#pragma HLS bind_storage variable = netURAM type = ram_2p impl = bram
    ap_uint<8> gatherNum[numSubGraph];
#pragma HLS bind_storage variable = gatherNum type = ram_2p impl = LUTRAM
    ap_uint<8> copyCnt[numSubGraph];
#pragma HLS bind_storage variable = copyCnt type = ram_2p impl = LUTRAM

initialLoop:
    for (int i = 0; i < numSubGraph; i++) {
#pragma HLS PIPELINE II = 1
        gatherNum[i] = 0;
        copyCnt[i] = 0;
        netURAM[depth * i] = 0;
    }

    ap_uint<32> src, des, idx;
    ap_uint<16> id;
    ap_uint<8> hop;
    ap_uint<8> cnt;
    ap_uint<8> new_cnt;
    ap_uint<12> copy;
    ap_uint<8> currentCopy;
    ap_uint<8> newCopy;
    ap_uint<128> tmp0;
    ap_uint<128> tmp1;
    ap_uint<512> tmp2;
    ap_uint<4> type = 0; // DATA_WORKLOAD

    bool end = strmInEnd.read();
    bool read = true;
gatherLoop:
    while (!end || !read) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = netURAM pointer inter false

        if (read) {
            tmp0 = strmIn.read();
            end = strmInEnd.read();

            src = tmp0(31, 0);
            des = tmp0(63, 32);
            idx = tmp0(95, 64);
            hop = tmp0(99, 96);
            copy = tmp0(111, 100);
            id = tmp0(127, 112);
        }

        // update cnt
        cnt = gatherNum[id];
        currentCopy = copyCnt[id];

        if (read) {
            if (cnt < (depth - 1))
                new_cnt = cnt + 1;
            else
                new_cnt = cnt;
            newCopy = currentCopy;
        } else {
            if (cnt > 0) {
                new_cnt = cnt - 1;
                newCopy = currentCopy;
            } else {
                new_cnt = 0;
                if ((copy > 0) && (currentCopy < (copy - 1)))
                    newCopy = currentCopy + 1;
                else
                    newCopy = currentCopy;
            }
        }
        gatherNum[id] = new_cnt;
        copyCnt[id] = newCopy;

        // read or write
        if (read) {
            tmp1(15, 0) = id * numKnlPar + currentCopy;
            tmp1(19, 16) = 0;
            tmp1(23, 20) = type;
            tmp1(31, 24) = hop;
            tmp1(63, 32) = idx;
            tmp1(95, 64) = des;
            tmp1(127, 96) = src;

            ap_uint<32> addr = depth * id + cnt;
            netURAM[addr] = tmp1;

#ifndef __SYNTHESIS__
#ifdef DEBUG_NET
            if ((src == 6444) || (des == 2401764))
                std::cout << "write graphID=" << id << " cardID=" << tmp1(15, 0) << " cnt=" << cnt << " src=" << src
                          << " des=" << des << " copy=" << currentCopy << std::endl;
#endif
#endif
        } else {
            ap_uint<32> addr = depth * id + cnt;
            tmp2(128 * cnt(1, 0) + 127, 128 * cnt(1, 0)) = netURAM[addr];
            netURAM[addr] = 0;

            if ((cnt(1, 0) == 0) && (tmp2 != 0)) {
                tmp2(19, 16) = cnt / 4;
                strmOut.write(tmp2);
                strmOutEnd.write(false);
                tmp2 = 0;
            }

#ifndef __SYNTHESIS__
#ifdef DEBUG_NET
            if ((netURAM[addr](127, 96) == 6444) || (netURAM[addr](95, 64) == 2401764))
                std::cout << "Read graphID=" << id << " cardID=" << netURAM[addr](15, 0) << " cnt=" << cnt
                          << " src=" << netURAM[addr](127, 96) << " des=" << netURAM[addr](95, 64) << " len=" << cnt / 4
                          << " copy=" << currentCopy << std::endl;
#endif
#endif
        }

        // update read status
        if (read) {
            if (cnt < (depth - 1)) {
                if (!end) {
                    read = true;
                } else {
                    read = false;
                }
            } else {
                read = false;
            }
            if (end) id = 0;
        } else {
            if (cnt == 0) {
                if (!end)
                    read = true;
                else {
                    if (id < numSubGraph - 1) {
                        id++;
                        read = false;
                    } else {
                        read = true;
                    }
                }
            } else {
                read = false;
            }
        }
    }
    strmOut.write(0);
    strmOutEnd.write(true);
}

template <int maxDevice, int PAD>
void mappingCardID(hls::stream<ap_uint<512> >& strmIn,
                   hls::stream<bool>& strmInEnd,
                   ap_uint<32> mapTable[maxDevice],
                   ap_uint<32>& cnt,
                   hls::stream<ap_uint<512> >& strmOut,
                   hls::stream<bool>& strmOutEnd) {
#pragma HLS INLINE off

#ifndef __SYNTHESIS__
#ifdef DEBUG_NET
    std::cout << "=====================mappingCardID=================" << std::endl;
#endif
#endif

    cnt = 0;
    bool end = false;
    do {
#pragma HLS PIPELINE II = 1
        ap_uint<512> tmp0 = strmIn.read();
        end = strmInEnd.read();
        ap_uint<16> id = tmp0(15, 0);
        ap_uint<16> mappingID = mapTable[id];

        ap_uint<512> tmp1;
        //tmp1(511, 16) = tmp0(511, 16);
        tmp1(15, 0) = mappingID;
        tmp1(127,16) = tmp0(127,16);

        tmp1(143,128) = mappingID;
        tmp1(255,144) = tmp0(255,144);

        tmp1(271,256) = mappingID;
        tmp1(383,272) = tmp0(383,272);

        tmp1(399,384) = mappingID;
        tmp1(511,400) = tmp0(511,400);

        if (tmp0 != 0) {
#ifndef __SYNTHESIS__
#ifdef DEBUG_NET
            for (int i = 0; i < 4; i++) {
                ap_uint<128> tmp = tmp0(128 * i + 127, 128 * i);
                ap_uint<32> src, des;
                src = tmp(127, 96);
                des = tmp(95, 64);
                if ((src == 6444) || (des == 2401764))
                    std::cout << "cnt=" << cnt << " id=" << id << " mapID=" << mappingID << " i=" << i << " src=" << src
                              << " des=" << des << std::endl;
            }
#endif
#endif
            cnt++;
            strmOut.write(tmp1);
            strmOutEnd.write(false);
        }
    } while (!end);
    // pad net data
    for (int i = 0; i < PAD; i++) {
#pragma HLS PIPELINE II = 1
        strmOut.write(0);
        strmOutEnd.write(false);
    }
}

template <int numSubGraph, int maxDevice, int PU>
void gatherNetPacket(hls::stream<ap_uint<128> > strmIn[PU],
                     hls::stream<bool> strmInEnd[PU],
                     ap_uint<32> numKnlPar,
                     ap_uint<64> cardTable[PU][numSubGraph],
                     ap_uint<32> mapTable[maxDevice],
                     ap_uint<32>& cnt,
                     hls::stream<ap_uint<512> >& strmOut,
                     hls::stream<bool>& strmOutEnd) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<128> > graphIDStrm[PU];
#pragma HLS stream variable = graphIDStrm depth = 8
#pragma HLS resource variable = graphIDStrm core = FIFO_SRL
    hls::stream<bool> graphIDStrmEnd[PU];
#pragma HLS stream variable = graphIDStrmEnd depth = 8
#pragma HLS resource variable = graphIDStrmEnd core = FIFO_SRL

    hls::stream<ap_uint<512> > netStrm[PU];
#pragma HLS stream variable = netStrm depth = 8
#pragma HLS resource variable = netStrm core = FIFO_SRL
    hls::stream<bool> netStrmEnd[PU];
#pragma HLS stream variable = netStrmEnd depth = 8
#pragma HLS resource variable = netStrmEnd core = FIFO_SRL

    hls::stream<ap_uint<512> > netMergeStrm("netMergeStrm");
#pragma HLS stream variable = netMergeStrm depth = 512
#pragma HLS resource variable = netMergeStrm core = FIFO_BRAM
    hls::stream<bool> netMergeStrmEnd("netMergeStrmEnd");
#pragma HLS stream variable = netMergeStrmEnd depth = 512
#pragma HLS resource variable = netMergeStrmEnd core = FIFO_SRL

    for (int i = 0; i < PU; i++) {
#pragma HLS UNROLL
        getGraphID<numSubGraph>(strmIn[i], strmInEnd[i], cardTable[i], graphIDStrm[i], graphIDStrmEnd[i]);
    }

    for (int i = 0; i < PU; i++) {
#pragma HLS UNROLL
        gatherGraphID<numSubGraph>(numKnlPar, graphIDStrm[i], graphIDStrmEnd[i], netStrm[i], netStrmEnd[i]);
    }

    ap_uint<32> tmp;
    if (PU == 4) {
        merge4to1<512, true, true>(netStrm[0], netStrmEnd[0], netStrm[1], netStrmEnd[1], netStrm[2], netStrmEnd[2],
                                   netStrm[3], netStrmEnd[3], tmp, netMergeStrm, netMergeStrmEnd);
    } else if (PU == 2) {
        merge2to1<512, true, true>(netStrm[0], netStrmEnd[0], netStrm[1], netStrmEnd[1], tmp, netMergeStrm,
                                   netMergeStrmEnd);
    } else {
        merge1to1<512, true, true>(netStrm[0], netStrmEnd[0], tmp, netMergeStrm, netMergeStrmEnd);
    }

    mappingCardID<maxDevice, 8>(netMergeStrm, netMergeStrmEnd, mapTable, cnt, strmOut, strmOutEnd);
}

template <int _WAxi, int _WStrm, int _BurstLen>
void countForBurst(hls::stream<ap_uint<_WStrm> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<_WAxi> >& axi_strm,
                   hls::stream<ap_uint<8> >& nb_strm) {
    const int N = _WAxi / _WStrm;
    ap_uint<_WAxi> tmp;
    bool isLast;
    int nb = 0;
    int bs = 0;

    isLast = e_istrm.read();
doing_loop:
    while (!isLast) {
#pragma HLS PIPELINE II = 1
        isLast = e_istrm.read();
        int offset = bs * _WStrm;
        ap_uint<_WStrm> t = istrm.read();
        tmp.range(offset + _WStrm - 1, offset) = t(_WStrm - 1, 0);
        if (bs == (N - 1)) {
            axi_strm.write(tmp);
            if (nb == (_BurstLen - 1)) {
                nb_strm.write(_BurstLen);
                nb = 0;
            } else
                ++nb;
            bs = 0;
        } else
            ++bs;
    }
    // not enough one axi
    if (bs != 0) {
    doing_not_enough:
        for (; bs < N; ++bs) {
#pragma HLS unroll
            int offset = bs * _WStrm;
            tmp.range(offset + _WStrm - 1, offset) = 0;
        }
        axi_strm.write(tmp);
        ++nb;
    }
    if (nb != 0) {
        nb_strm.write(nb);
    }
    nb_strm.write(0);
}

template <int _WAxi, int _WStrm, int _BurstLen>
void burstWrite(ap_uint<_WAxi>* wbuf, hls::stream<ap_uint<_WAxi> >& axi_strm, hls::stream<ap_uint<8> >& nb_strm) {
    int total = 0;
    ap_uint<_WAxi> tmp;
    int n = nb_strm.read();
doing_burst:
    while (n) {
    doing_one_burst:
        for (int i = 0; i < n; i++) {
#pragma HLS PIPELINE II = 1
            tmp = axi_strm.read();
            wbuf[total * _BurstLen + i] = tmp;

#ifndef __SYNTHESIS__
#ifdef DEBUG_OUTPUT
            for (int j = 0; j < 4; j++) {
                ap_uint<32> src, des, idx, hop;
                src = tmp(128 * j + 31, 128 * j);
                des = tmp(128 * j + 63, 128 * j + 32);
                idx = tmp(128 * j + 95, 128 * j + 64);
                hop = tmp(128 * j + 127, 128 * j + 96);

                if ((src == 6444) || (des == 2401764))
                    std::cout << "writeOut: addr=" << total * _BurstLen + i << " src=" << src << " des=" << des
                              << " idx=" << idx << " hop=" << hop << std::endl;
            }
#endif
#endif
        }
        n = nb_strm.read();
        total++;
    }
}

void writeOut(hls::stream<ap_uint<512> >& streamIn, hls::stream<bool>& streamInEnd, ap_uint<512>* buffer) {
#pragma HLS INLINE off
    const int fifo_buf = 512;

#pragma HLS dataflow

    hls::stream<ap_uint<512> > axi_strm;
    hls::stream<ap_uint<8> > nb_strm;
#pragma HLS stream variable = nb_strm depth = 2
#pragma HLS stream variable = axi_strm depth = fifo_buf

    countForBurst<512, 512, 64>(streamIn, streamInEnd, axi_strm, nb_strm);

    burstWrite<512, 512, 64>(buffer, axi_strm, nb_strm);
}

template <int depth>
void initSingleURAM(ap_uint<32> maxUpdateSize, ap_uint<96> aggrURAM[1 << depth], ap_uint<96> overflowURAM[4096]) {
#pragma HLS INLINE off

    for (int j = 0; j < maxUpdateSize; j++) {
#pragma HLS PIPELINE II = 1
        aggrURAM[j] = 0;
    }
    for (int j = 0; j < 4096; j++) {
#pragma HLS PIPELINE II = 1
        overflowURAM[j] = 0;
    }
}

template <int depth, int PU>
void initURAM(ap_uint<32> byPass,
              ap_uint<32> hashSize,
#ifndef __SYNTHESIS__
              ap_uint<96>* aggrURAM[PU],
              ap_uint<96>* overflowURAM[PU]
#else
              ap_uint<96> aggrURAM[PU][1 << depth],
              ap_uint<96> overflowURAM[PU][4096]
#endif
              ) {
#pragma HLS INLINE off

    const int maxBatch = 1 << depth;

    ap_uint<32> maxUpdateSize;
    if (hashSize <= 4096)
        maxUpdateSize = 4096;
    else if (hashSize <= 16384)
        maxUpdateSize = 16484;
    else if (hashSize <= 65536)
        maxUpdateSize = 65536 > maxBatch ? (ap_uint<32>)maxBatch : (ap_uint<32>)65536;
    else
        maxUpdateSize = maxBatch;

    if (byPass == 0) {
        for (int i = 0; i < PU; i++) {
#pragma HLS UNROLL

            initSingleURAM<depth>(maxUpdateSize, aggrURAM[i], overflowURAM[i]);
        }
    }
}

template <int depth>
void readURAM0(ap_uint<32> maxUpdateSize,
               ap_uint<96> aggrURAM[1 << depth],
               hls::stream<ap_uint<128> >& aggrOutStream,
               hls::stream<bool>& aggrOutStreamEnd) {
#pragma HLS INLINE off

    for (int j = 0; j < maxUpdateSize; j++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = aggrURAM pointer inter false
        ap_uint<96> tmp = aggrURAM[j];
        aggrURAM[j] = 0;

        if (tmp != 0) {
            aggrOutStream.write(tmp);
            aggrOutStreamEnd.write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
            ap_uint<32> src, des, count;
            src = tmp(31, 0);
            des = tmp(63, 32);
            count = tmp(95, 64);
            if ((src == 6444) || (des == 2401764))
                std::cout << "j=" << j << " src=" << src << " des=" << des << " count=" << count << std::endl;
#endif
#endif
        }
    }
}

void readURAM1(ap_uint<96> overflowURAM[4096],
               hls::stream<ap_uint<128> >& aggrOutStream,
               hls::stream<bool>& aggrOutStreamEnd) {
#pragma HLS INLINE off

    ap_uint<32> overflow_cnt = overflowURAM[0];
    overflowURAM[0] = 0;
#ifndef __SYNTHESIS__
    std::cout << "overflowCNT=" << overflow_cnt << std::endl;
#endif

    for (int j = 0; j <= overflow_cnt; j++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = overflowURAM pointer inter false
        ap_uint<96> tmp = overflowURAM[j + 1];
        overflowURAM[j + 1] = 0;

        if (tmp != 0) {
            aggrOutStream.write(tmp);
            aggrOutStreamEnd.write(false);

#ifndef __SYNTHESIS__
#ifdef DEBUG_AGGR
            ap_uint<32> src, des, count;
            src = tmp(31, 0);
            des = tmp(63, 32);
            count = tmp(95, 64);
            if ((src == 6444) || (des == 2401764))
                std::cout << "j=" << j + 1 << " src=" << src << " des=" << des << " count=" << count << std::endl;
#endif
#endif
        }
    }
}

template <int depth, int PU>
void outputURAM(bool batchEnd,
                bool pairEnd,
                ap_uint<32> byPass,
                ap_uint<32> hashSize,
                ap_uint<32>& numAggr,
                ap_uint<32> aggrThreshold,
#ifndef __SYNTHESIS__
                ap_uint<96>* aggrURAM[PU],
                ap_uint<96>* overflowURAM[PU],
#else
                ap_uint<96> aggrURAM[PU][1 << depth],
                ap_uint<96> overflowURAM[PU][4096],
#endif
                hls::stream<ap_uint<128> > aggrOutStream[8],
                hls::stream<bool> aggrOutStreamEnd[8]) {
#pragma HLS INLINE off

    const int maxBatch = 1 << depth;
    ap_uint<32> maxUpdateSize;
    if (hashSize <= 4096)
        maxUpdateSize = 4096;
    else if (hashSize <= 16384)
        maxUpdateSize = 16484;
    else if (hashSize <= 65536)
        maxUpdateSize = 65536 > maxBatch ? (ap_uint<32>)maxBatch : (ap_uint<32>)65536;
    else
        maxUpdateSize = maxBatch;

    if ((byPass == 0)) {
        if (batchEnd && ((numAggr >= aggrThreshold) || (pairEnd && (numAggr > 0)))) {
#ifndef __SYNTHESIS__
            std::cout << "=======================OutputAggrURAM===================" << std::endl;
#endif

            for (int i = 0; i < PU; i++) {
#pragma HLS UNROLL

                readURAM0<depth>(maxUpdateSize, aggrURAM[i], aggrOutStream[i], aggrOutStreamEnd[i]);
            }
            numAggr = 0;
        }

        if (batchEnd) {
#ifndef __SYNTHESIS__
            std::cout << "==================OutputAggrOverflowURAM=================" << std::endl;
#endif

            for (int i = 0; i < PU; i++) {
#pragma HLS UNROLL

                readURAM1(overflowURAM[i], aggrOutStream[i], aggrOutStreamEnd[i]);
            }
        }
    }
}

template <int numSubGraph, int maxDevice, int depth, int hopPU, int aggrPU>
void nHopCore(bool loadBatch,
              bool& batchEnd,
              ap_uint<32> numHop,
              ap_uint<32> intermediate,
              ap_uint<32> byPass,
              ap_uint<32> duplicate,
              ap_uint<32> hashSize,
              ap_uint<32> numPairs,
              ap_uint<32> numKnlPar,

              hls::stream<ap_uint<512> >& pair,
              hls::stream<bool>& pairEnd,

              ap_uint<32> offsetStart,
              ap_uint<32> offsetEnd,
              ap_uint<32> offsetTable[hopPU + 1],
              ap_uint<32> indexTable[hopPU + 1],
              ap_uint<64> cardTable[hopPU][numSubGraph],
              ap_uint<32> mapTable[maxDevice],

              unsigned* offset0,
              ap_uint<128>* index0,
              unsigned* offset1,
              ap_uint<128>* index1,
              unsigned* offset2,
              ap_uint<128>* index2,
              unsigned* offset3,
              ap_uint<128>* index3,
#ifndef __SYNTHESIS__
              ap_uint<96>* aggrURAM[aggrPU],
              ap_uint<96>* overflowURAM[aggrPU],
#else
              ap_uint<96> aggrURAM[aggrPU][1 << depth],
              ap_uint<96> overflowURAM[aggrPU][4096],
#endif
              ap_uint<32>& numAggr,
              ap_uint<32>& numResidual,
              ap_uint<32>& numSwitch,
              ap_uint<64> accCNT[hopPU],

              ap_uint<512>* bufferPing,
              ap_uint<512>* bufferPong,
              hls::stream<ap_uint<512> >& netStream,
              hls::stream<bool>& netStreamEnd,
              hls::stream<ap_uint<128> > outStream[8],
              hls::stream<bool> outStreamEnd[8]) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    const bool multipleAggr = true;

    hls::stream<ap_uint<128> > pairStream[4];
#pragma HLS stream variable = pairStream depth = 512
#pragma HLS resource variable = pairStream core = FIFO_BRAM
    hls::stream<bool> pairStreamEnd[4];
#pragma HLS stream variable = pairStreamEnd depth = 512
#pragma HLS resource variable = pairStreamEnd core = FIFO_SRL

    load(loadBatch, batchEnd, numPairs, pair, pairEnd, bufferPing, pairStream, pairStreamEnd);

    hls::stream<ap_uint<128> > switchStream[8];
#pragma HLS stream variable = switchStream depth = 512
#pragma HLS resource variable = switchStream core = FIFO_BRAM
    hls::stream<bool> switchStreamEnd[8];
#pragma HLS stream variable = switchStreamEnd depth = 512
#pragma HLS resource variable = switchStreamEnd core = FIFO_SRL

    switchPairs<hopPU>(duplicate, offsetTable, pairStream, pairStreamEnd, switchStream, switchStreamEnd);

    hls::stream<ap_uint<512> > indexStream[8];
#pragma HLS stream variable = indexStream depth = 8
#pragma HLS resource variable = indexStream core = FIFO_SRL
    hls::stream<bool> indexStreamEnd[8];
#pragma HLS stream variable = indexStreamEnd depth = 8
#pragma HLS resource variable = indexStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<64> > aggrStream[8];
#pragma HLS stream variable = aggrStream depth = 512
#pragma HLS resource variable = aggrStream core = FIFO_BRAM
    hls::stream<bool> aggrStreamEnd[8];
#pragma HLS stream variable = aggrStreamEnd depth = 512
#pragma HLS resource variable = aggrStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<128> > switchOutStream[8];
#pragma HLS stream variable = switchOutStream depth = 8
#pragma HLS resource variable = switchOutStream core = FIFO_BRAM
    hls::stream<bool> switchOutStreamEnd[8];
#pragma HLS stream variable = switchOutStreamEnd depth = 8
#pragma HLS resource variable = switchOutStreamEnd core = FIFO_SRL

    if (hopPU >= 1)
        hopProcessingUnit<multipleAggr>(numHop, intermediate, byPass, switchStream[0], switchStreamEnd[0], offsetStart,
                                        offsetEnd, indexTable[0], accCNT[0], offset0, index0, indexStream[0],
                                        indexStreamEnd[0], switchOutStream[0], switchOutStreamEnd[0], aggrStream[0],
                                        aggrStreamEnd[0], outStream[0], outStreamEnd[0]);

    if (hopPU >= 2)
        hopProcessingUnit<multipleAggr>(numHop, intermediate, byPass, switchStream[1], switchStreamEnd[1], offsetStart,
                                        offsetEnd, indexTable[1], accCNT[1], offset1, index1, indexStream[1],
                                        indexStreamEnd[1], switchOutStream[1], switchOutStreamEnd[1], aggrStream[1],
                                        aggrStreamEnd[1], outStream[1], outStreamEnd[1]);

    if (hopPU >= 4) {
        hopProcessingUnit<multipleAggr>(numHop, intermediate, byPass, switchStream[2], switchStreamEnd[2], offsetStart,
                                        offsetEnd, indexTable[2], accCNT[2], offset2, index2, indexStream[2],
                                        indexStreamEnd[2], switchOutStream[2], switchOutStreamEnd[2], aggrStream[2],
                                        aggrStreamEnd[2], outStream[2], outStreamEnd[2]);

        hopProcessingUnit<multipleAggr>(numHop, intermediate, byPass, switchStream[3], switchStreamEnd[3], offsetStart,
                                        offsetEnd, indexTable[3], accCNT[3], offset3, index3, indexStream[3],
                                        indexStreamEnd[3], switchOutStream[3], switchOutStreamEnd[3], aggrStream[3],
                                        aggrStreamEnd[3], outStream[3], outStreamEnd[3]);
    }

    hls::stream<ap_uint<512> > localStream;
#pragma HLS stream variable = localStream depth = 512
#pragma HLS resource variable = localStream core = FIFO_BRAM
    hls::stream<bool> localStreamEnd;
#pragma HLS stream variable = localStreamEnd depth = 512
#pragma HLS resource variable = localStreamEnd core = FIFO_SRL

    hls::stream<ap_uint<64> > aggrInternalStream;
#pragma HLS stream variable = aggrInternalStream depth = 512
#pragma HLS resource variable = aggrInternalStream core = FIFO_BRAM
    hls::stream<bool> aggrInternalStreamEnd;
#pragma HLS stream variable = aggrInternalStreamEnd depth = 512
#pragma HLS resource variable = aggrInternalStreamEnd core = FIFO_SRL
    ap_uint<32> tmp;

    if (hopPU == 4) {
        merge4to1<512, false, true>(indexStream[0], indexStreamEnd[0], indexStream[1], indexStreamEnd[1],
                                    indexStream[2], indexStreamEnd[2], indexStream[3], indexStreamEnd[3], numResidual,
                                    localStream, localStreamEnd);
    } else if (hopPU == 2) {
        merge2to1<512, false, true>(indexStream[0], indexStreamEnd[0], indexStream[1], indexStreamEnd[1], numResidual,
                                    localStream, localStreamEnd);
    } else {
        merge1to1<512, false, true>(indexStream[0], indexStreamEnd[0], numResidual, localStream, localStreamEnd);
    }

    if (multipleAggr) {
        if (aggrPU == 4) {
            multiHashAggrCounter<depth, hopPU, 2>(byPass, hashSize, aggrStream, aggrStreamEnd, numAggr, aggrURAM,
                                                  overflowURAM);
        } else if (aggrPU == 2) {
            multiHashAggrCounter<depth, hopPU, 1>(byPass, hashSize, aggrStream, aggrStreamEnd, numAggr, aggrURAM,
                                                  overflowURAM);
        } else {
            multiHashAggrCounter<depth, hopPU, 0>(byPass, hashSize, aggrStream, aggrStreamEnd, numAggr, aggrURAM,
                                                  overflowURAM);
        }
    } else {
        if (hopPU == 4) {
            merge4to1<64, false, true>(aggrStream[0], aggrStreamEnd[0], aggrStream[1], aggrStreamEnd[1], aggrStream[2],
                                       aggrStreamEnd[2], aggrStream[3], aggrStreamEnd[3], tmp, aggrInternalStream,
                                       aggrInternalStreamEnd);

            hashAggrCounter<depth, 2>(byPass, hashSize, aggrInternalStream, aggrInternalStreamEnd, numAggr, aggrURAM,
                                      overflowURAM);
        } else if (hopPU == 2) {
            merge2to1<64, false, true>(aggrStream[0], aggrStreamEnd[0], aggrStream[1], aggrStreamEnd[1], tmp,
                                       aggrInternalStream, aggrInternalStreamEnd);

            hashAggrCounter<depth, 1>(byPass, hashSize, aggrInternalStream, aggrInternalStreamEnd, numAggr, aggrURAM,
                                      overflowURAM);
        } else {
            hashAggrCounter<depth, 0>(byPass, hashSize, aggrInternalStream, aggrInternalStreamEnd, numAggr, aggrURAM,
                                      overflowURAM);
        }
    }

    gatherNetPacket<numSubGraph, maxDevice, hopPU>(switchOutStream, switchOutStreamEnd, numKnlPar, cardTable, mapTable,
                                                   numSwitch, netStream, netStreamEnd);

    writeOut(localStream, localStreamEnd, bufferPong);
}

template <int numSubGraph, int maxDevice, int hopPU>
void nHopPingPong(ap_uint<32> numHop,
                  ap_uint<32> intermediate,
                  ap_uint<32> localBatchSize,
                  ap_uint<32> switchBatchSize,
                  ap_uint<32> pingPongBatchSize,
                  ap_uint<32> hashSize,
                  ap_uint<32> aggrThreshold,
                  ap_uint<32> pingPongThreshold,
                  ap_uint<32> byPass,
                  ap_uint<32> duplicate,
                  ap_uint<32> numKnlPar,

                  hls::stream<ap_uint<512> >& localPair,
                  hls::stream<bool>& localPairEnd,
                  hls::stream<ap_uint<512> >& switchPair,
                  hls::stream<bool>& switchPairEnd,

                  ap_uint<32> offsetTable[hopPU + 1],
                  ap_uint<32> indexTable[hopPU + 1],
                  ap_uint<64> cardTable[hopPU][numSubGraph],
                  ap_uint<32> mapTable[maxDevice],

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

                  ap_uint<32>& numPingPong,
                  ap_uint<32>& maxPingPongSize,
                  ap_uint<32>& maxPingPongSwitch,
                  ap_uint<32>& maxAggr,
                  ap_uint<64>& accCNTAll,
                  ap_uint<32>& numSwitch,
                  hls::stream<ap_uint<512> >& switchStream,
                  hls::stream<bool>& switchStreamEnd,
                  hls::stream<ap_uint<128> > outStream[8],
                  hls::stream<bool> outStreamEnd[8]) {
#pragma HLS INLINE off

    const int depth = 17;
    const int maxBatch = 1 << depth;
    const int aggrPU = 4;

    numPingPong = 0;
    maxPingPongSize = 0;
    maxPingPongSwitch = 0;
    maxAggr = 0;
    accCNTAll = 0;
    numSwitch = 0;

#ifndef __SYNTHESIS__
    ap_uint<96>* aggrURAM[aggrPU];
    ap_uint<96>* overflowURAM[aggrPU];

    for (int i = 0; i < aggrPU; i++) {
        aggrURAM[i] = (ap_uint<96>*)malloc(maxBatch * sizeof(ap_uint<96>));
        overflowURAM[i] = (ap_uint<96>*)malloc(4096 * sizeof(ap_uint<96>));
    }
#else
    ap_uint<96> aggrURAM[aggrPU][maxBatch];
#pragma HLS bind_storage variable = aggrURAM type = ram_2p impl = uram
#pragma HLS ARRAY_PARTITION variable = aggrURAM block factor = 8
    ap_uint<96> overflowURAM[aggrPU][4096];
#pragma HLS bind_storage variable = overflowURAM type = ram_2p impl = uram
#pragma HLS ARRAY_PARTITION variable = overflowURAM block factor = 8
#endif

#ifndef __SYNTHESIS__
    std::cout << "========================InitializeURAM===================" << std::endl;
#endif

    xf::graph::internal::Hop::initURAM<depth, aggrPU>(byPass, hashSize, aggrURAM, overflowURAM);

    ap_uint<32> numAggr = 0;
    ap_uint<32> accumAggr = 0;

    bool ping_pong_flag = true;
    bool localEnd = false;
    bool switchEnd = false;
    bool sendIdle = true;
    ap_uint<32> offsetStart = offsetTable[0];
    ap_uint<32> offsetEnd = offsetTable[hopPU];
    ap_uint<32> numResidual = 0;
    ap_uint<32> pingResidual = 0;
    ap_uint<32> pongResidual = 0;
    ap_uint<32> pingStart = 0;
    ap_uint<32> pongStart = 0;
    ap_uint<64> accCNT[hopPU] = {0, 0, 0, 0};
#pragma HLS ARRAY_PARTITION variable = accCNT complete

HopLoop:
    while ((pingResidual > 0) || (pongResidual > 0) || !localEnd || !switchEnd) {
#pragma HLS PIPELINE off
#pragma HLS loop_tripcount min = 1

#ifndef __SYNTHESIS__
        std::cout << "========================nHopCoreStart===================" << std::endl;
#endif

        bool loadBatch = (pingResidual == 0) && (pongResidual == 0);
        bool loadLocal;
        ap_uint<32> numPair;
        ap_uint<32> switchCnt;
        if (loadBatch) {
            if (!switchPair.empty()) {
                numPair = switchBatchSize;
                loadLocal = false;
            } else {
                numPair = localBatchSize;
                loadLocal = true;
            }
            pingStart = 0;
            pongStart = 0;
        } else {
            if (ping_pong_flag) {
                if ((pingResidual > pingPongThreshold) && (numHop > 2)) {
                    numPair = pingPongBatchSize;
                    pingStart = pingResidual - pingPongBatchSize;
                } else {
                    numPair = pingResidual;
                    pingStart = 0;
                }
                pongStart = pongResidual;
                pingResidual = pingStart;
            } else {
                if ((pongResidual > pingPongThreshold) && (numHop > 2)) {
                    numPair = pingPongBatchSize;
                    pongStart = pongResidual - pingPongBatchSize;
                } else {
                    numPair = pongResidual;
                    pongStart = 0;
                }
                pingStart = pingResidual;
                pongResidual = pongStart;
            }
        }

        if ((pingResidual > 0) || (pongResidual > 0) || !localPair.empty() || !switchPair.empty()) {
            sendIdle = true;
            numPingPong++;
        }

        if (ping_pong_flag) {
            if (loadLocal) {
                nHopCore<numSubGraph, maxDevice, depth, hopPU, aggrPU>(
                    loadBatch, localEnd, numHop, intermediate, byPass, duplicate, hashSize, numPair, numKnlPar,
                    localPair, localPairEnd, offsetStart, offsetEnd, offsetTable, indexTable, cardTable, mapTable,
                    offset0, index0, offset1, index1, offset2, index2, offset3, index3, aggrURAM, overflowURAM, numAggr,
                    numResidual, switchCnt, accCNT, bufferPing + pingStart, bufferPong + pongStart, switchStream,
                    switchStreamEnd, outStream, outStreamEnd);
            } else {
                nHopCore<numSubGraph, maxDevice, depth, hopPU, aggrPU>(
                    loadBatch, switchEnd, numHop, intermediate, byPass, duplicate, hashSize, numPair, numKnlPar,
                    switchPair, switchPairEnd, offsetStart, offsetEnd, offsetTable, indexTable, cardTable, mapTable,
                    offset0, index0, offset1, index1, offset2, index2, offset3, index3, aggrURAM, overflowURAM, numAggr,
                    numResidual, switchCnt, accCNT, bufferPing + pingStart, bufferPong + pongStart, switchStream,
                    switchStreamEnd, outStream, outStreamEnd);
            }

            pongResidual += numResidual;
            if (pongResidual > maxPingPongSize) maxPingPongSize = pongResidual;
            if ((numResidual > 0) || ((numResidual == 0) && (pingResidual == 0))) ping_pong_flag = false;
        } else {
            if (loadLocal) {
                nHopCore<numSubGraph, maxDevice, depth, hopPU, aggrPU>(
                    loadBatch, localEnd, numHop, intermediate, byPass, duplicate, hashSize, numPair, numKnlPar,
                    localPair, localPairEnd, offsetStart, offsetEnd, offsetTable, indexTable, cardTable, mapTable,
                    offset0, index0, offset1, index1, offset2, index2, offset3, index3, aggrURAM, overflowURAM, numAggr,
                    numResidual, switchCnt, accCNT, bufferPong + pongStart, bufferPing + pingStart, switchStream,
                    switchStreamEnd, outStream, outStreamEnd);
            } else {
                nHopCore<numSubGraph, maxDevice, depth, hopPU, aggrPU>(
                    loadBatch, switchEnd, numHop, intermediate, byPass, duplicate, hashSize, numPair, numKnlPar,
                    switchPair, switchPairEnd, offsetStart, offsetEnd, offsetTable, indexTable, cardTable, mapTable,
                    offset0, index0, offset1, index1, offset2, index2, offset3, index3, aggrURAM, overflowURAM, numAggr,
                    numResidual, switchCnt, accCNT, bufferPong + pongStart, bufferPing + pingStart, switchStream,
                    switchStreamEnd, outStream, outStreamEnd);
            }

            pingResidual += numResidual;
            if (pingResidual > maxPingPongSize) maxPingPongSize = pingResidual;
            if ((numResidual > 0) || ((numResidual == 0) && (pongResidual == 0))) ping_pong_flag = true;
        }

        accumAggr += numAggr;
        if (accumAggr > maxAggr) maxAggr = accumAggr;
        numSwitch += switchCnt;
        if (switchCnt > maxPingPongSwitch) maxPingPongSwitch = switchCnt;

#ifndef __SYNTHESIS__
        std::cout << "numRes=" << numResidual << " pingRes=" << pingResidual << " pongRes=" << pongResidual
                  << std::endl;
        std::cout << "numAggr=" << numAggr << " accumAggr=" << accumAggr << std::endl;
        std::cout << "========================nHopCoreEnd=====================" << std::endl;
#endif

        bool batchEnd = (pingResidual == 0) && (pongResidual == 0);
        xf::graph::internal::Hop::outputURAM<depth, aggrPU>(batchEnd, (localEnd && switchEnd), byPass, hashSize, accumAggr,
                                                            aggrThreshold, aggrURAM, overflowURAM, outStream,
                                                            outStreamEnd);

        if (localPair.empty() && switchPair.empty() && sendIdle && (pingResidual == 0) && (pongResidual == 0)) {
#ifndef __SYNTHESIS__
            std::cout << "=======================Kernel Idle=====================" << std::endl;
#endif
            switchStream.write(3 << 20); // DATA_STATUS_OVER
            switchStreamEnd.write(false);
            sendIdle = false;
        }

        if((pingResidual > (16 << 20)) || (pongResidual > (16 << 20))) {
            if(sendIdle) {
                switchStream.write(3 << 20); // DATA_STATUS_OVER
                switchStreamEnd.write(false);
                sendIdle = false;
            }
            break;
        }
    }

    for (int i = 0; i < hopPU; i++) {
        accCNTAll += accCNT[i];
    }
    for (int i = 0; i < hopPU; i++) {
#pragma HLS UNROLL
        outStream[i].write(0);
        outStreamEnd[i].write(true);
    }
    switchStream.write(0);
    switchStreamEnd.write(true);

#ifndef __SYNTHESIS__
    for (int i = 0; i < aggrPU; i++) {
        free(aggrURAM[i]);
        free(overflowURAM[i]);
    }
#endif
}

template <int numSubGraph, int maxDevice, int PU>
void nHopTop(ap_uint<32> numHop,
             ap_uint<32> intermediate,
             ap_uint<32> localBatchSize,
             ap_uint<32> switchBatchSize,
             ap_uint<32> pingPongBatchSize,
             ap_uint<32> hashSize,
             ap_uint<32> aggrThreshold,
             ap_uint<32> pingPongThreshold,
             ap_uint<32> byPass,
             ap_uint<32> duplicate,
             ap_uint<32> numKnlPar,

             hls::stream<ap_uint<512> >& localPair,
             hls::stream<bool>& localPairEnd,
             hls::stream<ap_uint<512> >& switchPair,
             hls::stream<bool>& switchPairEnd,

             ap_uint<32> offsetTable[PU + 3],
             ap_uint<32> indexTable[PU + 1],
             ap_uint<64> cardTable[PU][numSubGraph],
             ap_uint<32> mapTable[maxDevice],

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

             ap_uint<32>& numLocal,
             ap_uint<32>& numSwitch,
             ap_uint<64>& accCNTAll,
             ap_uint<32>& numPingPong,
             ap_uint<32>& maxPingPongSize,
             ap_uint<32>& maxPingPongSwitch,
             ap_uint<32>& maxAggr,
             ap_uint<512>* bufferLocal,
             hls::stream<ap_uint<512> >& switchOut,
             hls::stream<bool>& switchOutEnd) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

    hls::stream<ap_uint<128> > outStream[8];
#pragma HLS stream variable = outStream depth = 512
#pragma HLS resource variable = outStream core = FIFO_BRAM
    hls::stream<bool> outStreamEnd[8];
#pragma HLS stream variable = outStreamEnd depth = 512
#pragma HLS resource variable = outStreamEnd core = FIFO_SRL

#ifndef __SYNTHESIS__
    std::cout << "=======================Ping-Pong Start==================" << std::endl;
#endif

    nHopPingPong<numSubGraph, maxDevice, PU>(
        numHop, intermediate, localBatchSize, switchBatchSize, pingPongBatchSize, hashSize, aggrThreshold,
        pingPongThreshold, byPass, duplicate, numKnlPar, localPair, localPairEnd, switchPair, switchPairEnd,
        offsetTable, indexTable, cardTable, mapTable, offset0, index0, offset1, index1, offset2, index2, offset3,
        index3, bufferPing, bufferPong, numPingPong, maxPingPongSize, maxPingPongSwitch, maxAggr, accCNTAll, numSwitch,
        switchOut, switchOutEnd, outStream, outStreamEnd);

#ifndef __SYNTHESIS__
    std::cout << "========================Ping-Pong End===================" << std::endl;
#endif

    hls::stream<ap_uint<512> > outMergeStream("outMergeStream");
#pragma HLS stream variable = outMergeStream depth = 512
#pragma HLS resource variable = outMergeStream core = FIFO_BRAM
    hls::stream<bool> outMergeStreamEnd("outMergeStreamEnd");
#pragma HLS stream variable = outMergeStreamEnd depth = 512
#pragma HLS resource variable = outMergeStreamEnd core = FIFO_SRL

#ifndef __SYNTHESIS__
    std::cout << "====================Output Result to Local=============" << std::endl;
#endif

    merge128to512<false, true>(outStream, outStreamEnd, numLocal, outMergeStream, outMergeStreamEnd);

    writeOut(outMergeStream, outMergeStreamEnd, bufferLocal);
}

} // namespace Hop
} // namespace internal

/**
 * @brief nHop this API can find the how many 2-hop pathes between two vertices. The input graph is the matrix in
 * CSR format. And a list of src and destination pairs whose 2-hop pathes will be counted.
 *
 * @param numPairs  How many pairs of source and destination vertices to be counted.
 * @param pair  The source and destination of pairs are stored in this pointer.
 * @param offsetOneHop The CSR offset is stored in this pointer.
 * @param indexOneHop The CSR index is stored in this pointer.
 * @param offsetnHop The CSR offset is stored in this pointer. The graph should be replicated and stored here. This
 * pointer is for an independent AXI port to increase performance.
 * @param indexTwoop The CSR index is stored in this pointer. The graph should be replicated and stored here. This
 * pointer is for an independent AXI port to increase performance.
 * @param cnt_res The result of the nHop API. The order of the result matches the order of the input source and
 * destination pairs.
 *
 */
void nHop(unsigned numHop,
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

          hls::stream<ap_uint<512> >& localPair,
          hls::stream<bool>& localPairEnd,
          hls::stream<ap_uint<512> >& switchIn,
          hls::stream<bool>& switchInEnd,

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

          unsigned* numOut,
          ap_uint<512>* bufferLocal,

          hls::stream<ap_uint<512> >& switchOut,
          hls::stream<bool>& switchOutEnd) {
#pragma HLS INLINE off

    const int numHopPU = 4;
    const int numSubGraph = 32;
    const int maxDevice = 4096;

#ifndef __SYNTHESIS__
    std::cout << "=========================nHopTopStart====================" << std::endl;
#endif

    ap_uint<32> localBufferSize;
    ap_uint<32> switchBufferSize;
    ap_uint<64> accCNTAll;
    ap_uint<32> numPingPong;
    ap_uint<32> maxPingPongSize;
    ap_uint<32> maxPingPongSwitch;
    ap_uint<32> maxAggr;

    xf::graph::internal::Hop::nHopTop<numSubGraph, maxDevice, numHopPU>(
        numHop, intermediate, localBatchSize, switchBatchSize, pingPongBatchSize, hashSize, aggrThreshold,
        pingPongThreshold, byPass, duplicate, numKnlPar, localPair, localPairEnd, switchIn, switchInEnd, offsetTable,
        indexTable, cardTable, mapTable, offset0, index0, offset1, index1, offset2, index2, offset3, index3, bufferPing,
        bufferPong, localBufferSize, switchBufferSize, accCNTAll, numPingPong, maxPingPongSize, maxPingPongSwitch,
        maxAggr, bufferLocal, switchOut, switchOutEnd);

    numOut[0] = localBufferSize;
    numOut[1] = switchBufferSize;
    numOut[2] = accCNTAll(31, 0);
    numOut[3] = accCNTAll(63, 32);
    numOut[4] = numPingPong;
    numOut[5] = maxPingPongSize;
    numOut[6] = maxPingPongSwitch;
    numOut[7] = maxAggr;

#ifndef __SYNTHESIS__
    std::cout << "numLocal=" << numOut[0] << " numSwitch=" << numOut[1] << " accCNTAll=" << (numOut[3], numOut[2])
              << " numPingPong=" << numOut[4] << " maxPingPongSize=" << numOut[5] << " maxAggr=" << numOut[6]
              << std::endl;
    std::cout << "===========================nHopTopEnd=====================" << std::endl;
#endif
}

} // namespace graph
} // namespace xf

#endif
