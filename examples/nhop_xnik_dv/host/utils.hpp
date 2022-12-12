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

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include <sys/time.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include "ap_int.h"

using namespace std;
// Compute time difference
inline unsigned long diff(const struct timeval* newTime, const struct timeval* oldTime) {
    return (newTime->tv_sec - oldTime->tv_sec) * 1000000 + (newTime->tv_usec - oldTime->tv_usec);
}

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePointType;

#ifndef HLS_TEST
inline double getTime() {
    TimePointType t1 = chrono::high_resolution_clock::now();
    chrono::duration<double> l_durationSec = chrono::duration<double>(t1.time_since_epoch());
    return l_durationSec.count();
}

inline void getDiffTime(TimePointType& t1, TimePointType& t2, double& time) {
    t2 = chrono::high_resolution_clock::now();
    chrono::duration<double> l_durationSec = t2 - t1;
    time = l_durationSec.count();
}
#endif
//--------------------------------------------------------------

#include <new>

#include <cstdlib>
#include <iterator>
#include <vector>

template <typename T>
T* aligned_alloc(std::size_t num) {
    void* ptr = NULL;

    if (posix_memalign(&ptr, 4096, num * sizeof(T))) throw std::bad_alloc();
    // ptr = (void*)malloc(num * sizeof(T));
    return reinterpret_cast<T*>(ptr);
}

inline ap_uint<64>* GetPair(const char* pairfile, long* num) {
    long fileIdx = 0;
    std::fstream pairfstream(pairfile, std::ios::in);
    if (!pairfstream) {
        std::cout << "Error : " << pairfile << " file doesn't exist !" << std::endl;
        exit(1);
    }
    char line[1024] = {0};
    pairfstream.getline(line, sizeof(line));
    std::stringstream numPdata(line);
    int numPairs;
    numPdata >> numPairs;
    *num = numPairs;

    ap_uint<64>* pair = aligned_alloc<ap_uint<64> >(numPairs);

    while (pairfstream.getline(line, sizeof(line))) {
        std::stringstream data(line);
        ap_uint<64> tmp64;
        unsigned src;
        unsigned des;
        data >> src;
        tmp64.range(31, 0) = src - 1;
        data >> des;
        tmp64.range(63, 32) = des - 1;
        pair[fileIdx] = tmp64;
        fileIdx++;
    }
    pairfstream.close();
    return pair;
}

//--------------------------------------------------------------
class ArgParser {
   public:
    ArgParser(int& argc, const char** argv) {
        for (int i = 1; i < argc; ++i) mTokens.push_back(std::string(argv[i]));
    }
    bool getCmdOption(const std::string option, std::string& value) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->mTokens.begin(), this->mTokens.end(), option);
        if (itr != this->mTokens.end() && ++itr != this->mTokens.end()) {
            value = *itr;
            return true;
        }
        return false;
    }

   private:
    std::vector<std::string> mTokens;
};

#endif
