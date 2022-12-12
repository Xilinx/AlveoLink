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

#ifndef _XF_GRAPH_nHop_KERNEL_HPP_
#define _XF_GRAPH_nHop_KERNEL_HPP_

#include <ap_int.h>
#include "ap_axi_sdata.h"
#include <hls_stream.h>

extern "C" void nHop_kernel(unsigned* config,
                            ap_uint<512>* pair,
                            hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn,

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
                            hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut);

#endif
