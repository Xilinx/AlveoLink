
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

#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

// #include <fstream>
// #include <iostream>
// #include <sstream>
// #include "xNativeFPGA.hpp"
#include "xilinx_apps_common.hpp"

struct commentInfo {
    int num_hop = 2;
    int devId = 0;
    int myId = 0; // logic id
    // int numDevs = 2;
    int hostId = 0;
    int startId = 0;
    int numIds = 1;
    // int batchPkts = 128;   // 4;
    // int timeOutCycles = 0; // 26214450; // (400 << 16) + 50
    // int tmId = 2;
    int numKernel = 1;
    int numPuPerKernel = 4;
    int sz_bat = 512;
    int byPass = 0;
    int duplicate = 0;
    int useXnik = 0;
    XString xclbin_path;
    XString foldername;
    XString filename;
    XString netfile;
    int pairGroupId = -1;
    bool output = true;
    long numRes = 0;        // number of result
    long accCntAllEdge = 0; // all counts of edge accessed, double type for deal with the timing
};

#endif
