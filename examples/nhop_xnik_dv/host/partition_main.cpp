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
#include <sys/stat.h>
#include <unordered_map>

int Demo_1(ArgParser& parser) {
    // 1. get commentInfo, include the default value of the input comment
    commentInfo commentInfo;
    std::string graphfile;
    std::string offsetfile;
    std::string indexfile;
    std::string pairfile;
    std::string goldenfile;
    std::string args;

    if (!parser.getCmdOption("--graph", graphfile)) {
        parser.getCmdOption("--offset", offsetfile);
        parser.getCmdOption("--index", indexfile);
        if (offsetfile.empty() && indexfile.empty()) {
            std::cout << "ERROR: graph file path is not set!\n";
            return -1;
        } else {
            std::cout << "offset file path: " << offsetfile << "\n";
            std::cout << "index  file path: " << indexfile << "\n";
        }
    } else {
        std::cout << "graph file path: " << graphfile << "\n";
    }

    if (!parser.getCmdOption("--pair", pairfile)) {
        std::cout << "WARNING: pair file path is not set!\n";
    } else {
        std::cout << "pair  file path: " << pairfile << "\n";
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

    int partition_pair = 1;
    int partition_graph = 1;
    if (!parser.getCmdOption("--partitionOption", args)) {
        std::cout << "partition both pair and graph" << std::endl;
    } else {
        if (args == "pair") {
            partition_graph = 0;
            std::cout << "partition pair" << std::endl;
        } else if (args == "graph") {
            partition_pair = 0;
            std::cout << "partition graph" << std::endl;
        } else {
            std::cout << "partition both pair and graph" << std::endl;
        }
    }

    int pairGroup = 1;

    if (!parser.getCmdOption("--pairGroup", args)) {
        std::cout << "Use default group = 1\n";
    } else {
        pairGroup = stoi(args);
        std::cout << "pair group =  " << pairGroup << "\n";
    }

    // for debug
    double tmp = 128; //  64 M vertex or edge

    std::string fn = pairfile.substr(pairfile.find_last_of('/') + 1);
    commentInfo.foldername = fn.substr(0, fn.rfind("."));
    std::string foldername = commentInfo.foldername;
    mkdir(commentInfo.foldername.c_str(), 0750);
    commentInfo.filename = fn.substr(0, fn.rfind(".")) + ".hop";
    if (!parser.getCmdOption("--test", args)) {
        std::cout << "The results will output to the ./" << commentInfo.filename << " file " << std::endl;
    } else {
        commentInfo.output = false;
        std::cout << "Using test mode and will not generate the output *.hop file " << std::endl;
    }

    // commentInfo.xclbin_path = xclbin_path;
    PartitionHop<unsigned> par1;
    double Limit_MB_v = tmp;
    double Limit_MB_e = tmp;
    if (partition_graph) {
        // 2. get graph and start partition
        std::cout << "INFO: Loading files ... " << std::endl;
        CSR<unsigned> csr0;
        if (!graphfile.empty()) {
            csr0.Init(graphfile.c_str(), true);
        } else {
            csr0.Init(offsetfile.c_str(), indexfile.c_str());
        }
        csr0.ShowInfo("csr0 Graph");

        par1.hop_src = &csr0;

        par1.CreatePartitionForKernel(commentInfo, commentInfo.numKernel, commentInfo.numPuPerKernel,
                                      Limit_MB_v * 4 * (1 << 20), Limit_MB_e * 4 * (1 << 20));
    }
    ap_uint<64>* pair;
    if (partition_pair) {
        if (partition_graph == 0) {
            std::fstream infoFile;
            infoFile.open(foldername + "/" + "partition_info.txt");
            infoFile >> par1.num_knl_par >> par1.num_chnl_par;
            for (int i = 0; i < commentInfo.numKernel; i++) {
                std::string l_infoFileName =
                    foldername + "/" + "info_" + std::to_string(i * commentInfo.numPuPerKernel) + ".txt";
                std::ifstream l_infoFile(l_infoFileName);
                std::vector<unsigned int> l_intVec(2);
                l_infoFile >> l_intVec[0] >> l_intVec[1];
                par1.tab_disp_knl[i] = l_intVec[0];
                printf("Par_ID_knl:%3d:  %8d,      %3d\n", i, par1.tab_disp_knl[i]);
                par1.limit_v_b = Limit_MB_v * 4 * (1 << 20);
                par1.limit_e_b = Limit_MB_e * 4 * (1 << 20);
                par1.num_knl_used = commentInfo.numKernel;
                par1.num_chnl_knl = commentInfo.numPuPerKernel;
            }
        }
        // partition pair
        long num_pair;
        pair = GetPair(pairfile.c_str(), &num_pair);
        par1.PartitionPair(commentInfo.foldername, pair, num_pair, pairGroup);
    }

    if (partition_pair) {
        free(pair);
    }
    return 0;
}

int main(int argc, const char* argv[]) {
    std::cout << "\n---------------------N Hop-------------------\n";

    // cmd parser
    ArgParser parser(argc, argv);

    int l_res = Demo_1(parser);
    return l_res;
}
