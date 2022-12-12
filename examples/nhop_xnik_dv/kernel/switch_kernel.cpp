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

#include "switch_kernel.hpp"

#ifndef __SYNTHESIS__
#include <iostream>
#endif

void switchWrapper(unsigned terminate,
                   hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn0,
                   hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn1,
                   hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut0,
                   hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut1) {
#pragma HLS INLINE off

    bool kernel0_idle = false;
    bool kernel1_idle = false;
    ap_uint<32> idleCnt = 0;

    ap_axiu<512, 0, 0, 0> in0;
    ap_axiu<512, 0, 0, 0> in1;

switchLoop:
    while (!kernel0_idle || !kernel1_idle || (idleCnt < terminate)) {
#pragma HLS PIPELINE II = 1

        if (!switchIn0.empty()) {
            in0 = switchIn0.read();
            if (in0.data != (3 << 20)) {
                if(in0.data(15, 0) == 1)
                    switchOut1.write(in0);
                kernel0_idle = false;
            } else {
                kernel0_idle = true;
            }
        }

        if (!switchIn1.empty()) {
            in1 = switchIn1.read();
            if (in1.data != (3 << 20)) {
                if(in1.data(15, 0) == 0)
                    switchOut0.write(in1);
                kernel1_idle = false;
            } else {
                kernel1_idle = true;
            }
        }

        if (kernel0_idle && kernel1_idle) {
            idleCnt++;
        } else {
            idleCnt = 0;
        }
    }

    // terminate kernel
    in0.data = 3 << 20;
    in1.data = 3 << 20;
    switchOut0.write(in0);
    switchOut1.write(in1);
}

extern "C" void switch_kernel(unsigned terminateCnt,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn0,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchIn1,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut0,
                              hls::stream<ap_axiu<512, 0, 0, 0> >& switchOut1) {
    const int ext_mem_size = 16 << 20;

// clang-format off

#pragma HLS INTERFACE s_axilite port = terminateCnt bundle = control

#pragma HLS INTERFACE axis port = switchIn0 bundle = control
#pragma HLS INTERFACE axis port = switchOut0 bundle = control
#pragma HLS INTERFACE axis port = switchIn1 bundle = control
#pragma HLS INTERFACE axis port = switchOut1 bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control
// clang-format on

#ifndef __SYNTHESIS__
    std::cout << "switch kernel call success" << std::endl;
#endif

    switchWrapper(terminateCnt, switchIn0, switchIn1, switchOut0, switchOut1);

#ifndef __SYNTHESIS__
    std::cout << "switch kernel call finish" << std::endl;
#endif
}
