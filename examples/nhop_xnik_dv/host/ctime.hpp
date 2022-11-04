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
/**
 * @file ctime.hpp
 *
 * @brief This file contains top function of test case.
 */

#ifndef _CTIME_HPP_
#define _CTIME_HPP_

#include <sys/time.h>
#include <iostream>
#include <string>
//#include <omp.h>
#include <vector>
#include "ctime.hpp"

using namespace std;

template <typename DT>
class CTimeModule {
   public:
    CTimeModule() {}  // constructor
    ~CTimeModule() {} // destructor

    // Copy constructor
    CTimeModule(const CTimeModule& ctime) { this->arrayTime = ctime.arrayTime; }
    // Assign constructor
    CTimeModule& operator=(const CTimeModule& ctime) { return *this; }

    // add time point
    void addTime(string name) {
        // double time = omp_get_wtime();
        // arrayTime.emplace(name, time);
        struct timeval time;
        gettimeofday(&time, 0);
        DT curr_time = time.tv_sec * 1000000 + time.tv_usec; // us
        DT ftime = curr_time / 1000;                         // ms
        arrayName.push_back(name);
        arrayTime.push_back(ftime);
    }

    void addTime(string name, int loop) {
        struct timeval time;
        gettimeofday(&time, 0);
        DT curr_time = time.tv_sec * 1000000 + time.tv_usec; // us
        DT ftime = curr_time / 1000;                         // ms
        arrayName.push_back(name + ":loop:" + to_string(loop));
        arrayTime.push_back(ftime);
    }

    // print time array
    void printTime() {
        for (int i = 1; i < arrayName.size(); i++) {
            DT t = arrayTime[i] - arrayTime[0];
            std::cout << arrayName[i] << ": " << t << " ms\n";
        }
    }

   private:
    vector<string> arrayName; // name array
    vector<DT> arrayTime;     // time array
};

#endif