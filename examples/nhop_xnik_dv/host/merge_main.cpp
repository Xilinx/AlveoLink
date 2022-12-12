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

int runMerge(commentInfo& commentInfo) {
    std::string filepath = commentInfo.foldername;
    std::string fn = filepath.substr(filepath.find_last_of('/') + 1);
    commentInfo.filename = fn.substr(0, fn.rfind(".")) + ".hop";

    PartitionHop<unsigned> par1;

    par1.MergeResult(&commentInfo);

    return 0;
}

int main(int argc, const char* argv[]) {
    std::cout << "\n---------------------N Hop-------------------\n";

    // cmd parser
    ArgParser parser(argc, argv);
    
    commentInfo commentInfo;

    std::string filepath;
    std::string args;

    if (!parser.getCmdOption("--filepath", filepath)) {
        std::cout << "WARNING: pair file path is not set!\n";
    } else {
        std::cout << "pair  file path: " << filepath << "\n";
        commentInfo.foldername = filepath;
    }

    if (!parser.getCmdOption("--numKernel", args)) {
        std::cout << "Using default number of kernel: " << commentInfo.numKernel << std::endl;
    } else {
        commentInfo.numKernel = stoi(args);
    }


    return runMerge(commentInfo);
}
