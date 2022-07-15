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
#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

typedef ap_axiu<512, 0, 0, 13> t_PktType;

void krnl_latency(ap_uint<1>& start,
                            ap_uint<1>& gen_col,
                            hls::stream<t_PktType>& outStr,
                            hls::stream<t_PktType>& inStr,
                            ap_uint<13>& id,
                            ap_uint<64>& mean) {
#pragma HLS INTERFACE mode = ap_ctrl_none port = return
#pragma HLS INTERFACE mode = s_axilite port = gen_col register
#pragma HLS INTERFACE mode = s_axilite port = start register
#pragma HLS INTERFACE mode = s_axilite port = id register
#pragma HLS INTERFACE mode = s_axilite port = mean register
#pragma HLS PIPELINE II = 1

    static ap_uint<64> counter = 0;
    static ap_uint<64> mean_l = 0;
    static ap_uint<64> total = 0;
    static ap_uint<64> count = 0;
    static enum { sendData, wait } state = sendData;

    if (gen_col == 0) {
        t_PktType fromHive;
        if (inStr.read_nb(fromHive)) {
            fromHive.dest = id;
            outStr.write(fromHive);
        }
    } else {
        mean = mean_l;
        counter++;
        if (start == 1) {
            switch (state) {
                case sendData: {
                    t_PktType dat;
                    dat.data = counter;
                    dat.dest = id;
                    dat.keep = 0xFFFFFFFFFFFFFFFF;
                    dat.last = 1;
                    outStr.write(dat);
                    state = wait;
                    break;
                }
                case wait: {
                    t_PktType read_data;
                    if (inStr.read_nb(read_data)) {
                        total += counter - read_data.data(63, 0);
                        count++;
                        mean_l = total / count;
                        state = sendData;
                    } else {
                        state = wait;
                    }
                    break;
                }
            }
        }
    }
}
