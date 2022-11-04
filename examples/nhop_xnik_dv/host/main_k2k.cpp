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

#include "nHopConfig.hpp"
#include "utils.hpp"
#include "nHopUtils.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>

#include <unordered_map>
#include "nHopHost.hpp"

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePointType;

inline void showTimeData(std::string p_Task, TimePointType& t1, TimePointType& t2, double* p_TimeMsOut = 0) {
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> l_durationSec = t2 - t1;
    double l_timeMs = l_durationSec.count() * 1e3;
    if (p_TimeMsOut) {
        *p_TimeMsOut = l_timeMs;
    }
    std::cout << p_Task << "  " << std::fixed << std::setprecision(6) << l_timeMs << " msec\n";
}

int Demo_1(ArgParser& parser) {
    // 1. get commentInfo, include the default value of the input comment
    commentInfo commentInfo;
    std::string xclbin_path;
    AlveoLink::common::FPGA l_card;
    std::string filepath;
    std::string goldenfile;
    std::string args;

    if (!parser.getCmdOption("--xclbin", xclbin_path)) {
        std::cout << "ERROR:xclbin path is not set!\n";
        return 1;
    } else {
        std::cout << "running K2k, xclbin: " << xclbin_path << "\n";
        commentInfo.xclbin_path = xclbin_path;
    }

    if (!parser.getCmdOption("--filepath", filepath)) {
        std::cout << "WARNING: pair file path is not set!\n";
    } else {
        std::cout << "pair  file path: " << filepath << "\n";
    }

    if (!parser.getCmdOption("--numKernel", args)) {
        std::cout << "Using default number of kernel: " << commentInfo.numKernel << std::endl;
    } else {
        commentInfo.numKernel = stoi(args);
    }

    if (!parser.getCmdOption("--numPuPerKernel", args)) {
        std::cout << "Using default number of PU: " << commentInfo.numPuPerKernel << std::endl;
    } else {
        commentInfo.numPuPerKernel = stoi(args);
    }

    ap_uint<32> num_hop = 3;
    if (!parser.getCmdOption("--hop", args)) {
        std::cout << "Using default number of hop: " << num_hop << std::endl;
    } else {
        num_hop = stoi(args);
    }

    if (!parser.getCmdOption("--batch", args)) {
        std::cout << "Using default number of batch size: " << commentInfo.sz_bat << std::endl;
    } else {
        commentInfo.sz_bat = stoi(args);
    }
    std::string fn = filepath.substr(filepath.find_last_of('/') + 1);
    std::cout << filepath << " " << fn << std::endl;

    commentInfo.foldername = fn;
    commentInfo.filename = fn + ".hop";
    if (!parser.getCmdOption("--test", args)) {
        std::cout << "The results will output to the ./" << commentInfo.filename << " file " << std::endl;
    } else {
        commentInfo.output = false;
        std::cout << "Using test mode and will not generate the output *.hop file " << std::endl;
    }

    l_card.setId(commentInfo.devId);
    l_card.load_xclbin(commentInfo.xclbin_path);
    

    PartitionHop<unsigned> par1;
    double Limit_MB_v = 128;
    double Limit_MB_e = 128;
    for (int i = 0; i < commentInfo.numKernel; i++) {
        commentInfo.myId = i;
        par1.initKernel(commentInfo, commentInfo.numKernel, commentInfo.numPuPerKernel, Limit_MB_v * 4 * (1 << 20),
                        Limit_MB_e * 4 * (1 << 20));
    }
    IndexStatistic stt;
    timeInfo timeInfo;
    unsigned skipped;
    unsigned nv;
    unsigned ne;
    std::fstream infoFile;
    infoFile.open(filepath + "/" + "partition_info.txt");
    infoFile >> skipped >> skipped >> nv >> ne;

    par1.m_card = &l_card;
    par1.LoadPair2Buff(nv, ne, num_hop, commentInfo, &timeInfo, &stt);

    par1.PrintRpt(num_hop, commentInfo, timeInfo, stt);

    return 0;
}

int main(int argc, const char* argv[]) {
    std::cout << "\n---------------------N Hop-------------------\n";

    // cmd parser
    ArgParser parser(argc, argv);

    return Demo_1(parser);
}
