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

#include "ap_int.h"
#include "nHopConfig.hpp"
#include "utils.hpp"
#include "nHopUtils.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "graphPktDefs.hpp"

#include <unordered_map>
#include "ctime.hpp"
#include "nHopHost.hpp"

int runNhop(commentInfo& commentInfo) {
    // 1. get commentInfo, include the default value of the input comment

    AlveoLink::common::FPGA l_card;
    unsigned int l_waitCycles = 200;
    unsigned int l_flushCounter = 1;


    l_card.setId(commentInfo.devId);
    l_card.load_xclbin(commentInfo.xclbin_path);

    // for debug
    std::string filepath = commentInfo.foldername;
    std::string fn = filepath.substr(filepath.find_last_of('/') + 1);
    commentInfo.filename = fn.substr(0, fn.rfind(".")) + ".hop";  
    commentInfo.numKernel = commentInfo.numIds;
    commentInfo.useXnik = 1;

    PartitionHop<unsigned> par1;
    par1.m_card = &l_card;
    double Limit_MB_v = 128;
    double Limit_MB_e = 128;
    for (int i = commentInfo.startId; i < commentInfo.numIds; ++i) {
        commentInfo.myId = i;
        par1.initKernel(commentInfo, commentInfo.numKernel, commentInfo.numPuPerKernel, Limit_MB_v * 4 * (1 << 20),
                        Limit_MB_e * 4 * (1 << 20));
    }
    // working process
    // 3. dispatch subgraph to multi kernels
    IndexStatistic stt;
    timeInfo timeInfo;
    unsigned skipped;
    unsigned nv;
    unsigned ne;
    std::fstream infoFile;
    infoFile.open(filepath + "/" + "partition_info.txt");
    infoFile >> skipped >> skipped >> nv >> ne;

    par1.LoadPair2Buff(nv, ne, commentInfo.num_hop, commentInfo, &timeInfo, &stt);
    par1.PrintRpt(commentInfo.num_hop, commentInfo, timeInfo, stt);

    return 0;
}

int main(int argc, const char* argv[]) {
    std::cout << "\n---------------------N Hop-------------------\n";

    // cmd parser
    ArgParser parser(argc, argv);
    
    commentInfo commentInfo;

    std::string xclbin_path;
    std::string filepath;
    std::string args;
    
    if (!parser.getCmdOption("--xclbin", xclbin_path)) {
        std::cout << "ERROR:xclbin path is not set!\n";
        return 1;
    } else {
        std::cout << "running nhop, xclbin: " << xclbin_path << "\n";
        commentInfo.xclbin_path = xclbin_path;
    }

    if (!parser.getCmdOption("--filepath", filepath)) {
        std::cout << "WARNING: pair file path is not set!\n";
    } else {
        std::cout << "pair  file path: " << filepath << "\n";
        commentInfo.foldername = filepath;
    }

    if (!parser.getCmdOption("--devId", args)) {
        std::cout << "Using default devId: " << commentInfo.devId << std::endl;
    } else {
        commentInfo.devId = stoi(args);
    }

    if (!parser.getCmdOption("--numDevs", args)) {
        std::cout << "Using default numDevs: " << commentInfo.numDevs << std::endl;
    } else {
        commentInfo.numDevs = stoi(args);
    }

    if (!parser.getCmdOption("--startId", args)) {
        std::cout << "Using default startId: " << commentInfo.startId << std::endl;
    } else {
        commentInfo.startId = stoi(args);
    }

    if (!parser.getCmdOption("--numIds", args)) {
        std::cout << "Using default numIds: " << commentInfo.numIds << std::endl;
    } else {
        commentInfo.numIds = stoi(args);
    }

    if (!parser.getCmdOption("--hop", args)) {
        std::cout << "Using default number of hop: 2" << std::endl;
    } else {
        commentInfo.num_hop = stoi(args);
    }

    if (!parser.getCmdOption("--batch", args)) {
        std::cout << "Using default number of batch size: " << commentInfo.sz_bat << std::endl;
    } else {
        commentInfo.sz_bat = stoi(args);
    }

    if (!parser.getCmdOption("--timeOutCycles", args)) {
        std::cout << "Using default number of timeOutCycles: " << commentInfo.timeOutCycles << std::endl;
    } else {
        commentInfo.timeOutCycles = stoi(args);
    }

    if (!parser.getCmdOption("--bypass", args)) {
        std::cout << "Using default bypass the aggregation module? " << commentInfo.byPass
                  << " means bypass the aggregation" << std::endl;
    } else {
        commentInfo.byPass = stoi(args);
    }

    if (!parser.getCmdOption("--duplicate", args)) {
        std::cout << "Using default not duplicate for all channel? " << commentInfo.duplicate
                  << " means split the pair package to each channel" << std::endl;
    } else {
        commentInfo.duplicate = stoi(args);
    }

    return runNhop(commentInfo);
}
