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

#ifndef ALVEOLINK_UUT_TOP_HPP
#define ALVEOLINK_UUT_TOP_HPP

#include <cstdint>
#include "ap_int.h"
#include "hls_stream.h"
#include "graphPktDefs.hpp"
#include "xnikSyncDV.hpp"

#define MAX_NUMPKTS 1024
void uut_top(uint32_t p_control,
             ap_uint<AL_netDataBits> p_inPkts[MAX_NUMPKTS],
             ap_uint<AL_netDataBits> p_rxPkts[MAX_NUMPKTS],
             ap_uint<AL_netDataBits> p_xnik2nhopPkts[MAX_NUMPKTS]); 
#endif
