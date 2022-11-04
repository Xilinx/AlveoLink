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
#include "nhop_functions.hpp"

int main(int argc, const char* argv[]) {
    std::cout << "\n---------------------N Hop-------------------\n";

    // cmd parser
    ArgParser parser(argc, argv);
    commentInfo commentInfo;
    std::string args;

    std::string xclbin_path;
    std::string filepath;
    std::string netfile;

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

    if (!parser.getCmdOption("--netfile", netfile)) {
        std::cout << "WARNING: network config file is not set!\n";
        return EXIT_FAILURE;
    } else {
        std::cout << "network config file: " << netfile << "\n";
        commentInfo.netfile = netfile;
    }

    if (!parser.getCmdOption("--devId", args)) {
        std::cout << "Using default devId: " << commentInfo.devId << std::endl;
    } else {
        commentInfo.devId = stoi(args);
    }

    if (!parser.getCmdOption("--hostId", args)) {
        std::cout << "Using default hostId: " << commentInfo.hostId << std::endl;
    } else {
        commentInfo.hostId = stoi(args);
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

    if (!parser.getCmdOption("--numKernel", args)) {
        std::cout << "Using default numKernel: " << commentInfo.numKernel << std::endl;
    } else {
        commentInfo.numKernel = stoi(args);
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

    if (!parser.getCmdOption("--pairGroupId", args)) {
        std::cout << "No pair group\n";
    } else {
        commentInfo.pairGroupId = stoi(args);
        std::cout << "pair group id =  " << commentInfo.pairGroupId << "\n";
    }
#ifdef HAS_MANAGER
    runManagerAndNhop(commentInfo, true);
#else
    runManagerAndNhop(commentInfo, false);
//    runMerge(commentInfo);
#endif
    return 0;
}
