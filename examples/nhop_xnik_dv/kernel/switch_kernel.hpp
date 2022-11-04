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

#ifndef _XF_GRAPH_SWITCH_KERNEL_HPP_
#define _XF_GRAPH_SWITCH_KERNEL_HPP_

#include <ap_int.h>
#include "ap_axi_sdata.h"
#include <hls_stream.h>

extern "C" void switch_kernel(unsigned terminateCnt,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn0,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn1,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut0,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut1);
#endif
