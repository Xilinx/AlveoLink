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
#ifndef WHOLE_FUNC_H_
#define WHOLE_FUNC_H_
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include "xrt/xrt_bo.h"
#include "xrt/xrt_kernel.h"
#include "experimental/xrt_ip.h"
#include "experimental/xrt_xclbin.h"

using namespace std;

#define param_start 0x1
#define param_idle 0x4
#define CTRL_offset 0x0
#define clck_ns 6

#define rs_fecON 0x3
//#define TABLE_SIZE 8192

void cmacLinkStatus(xrt::ip& ip, vector<xrt::xclbin::arg>& args);
// void readARPTable(xrtDeviceHandle device, const unsigned int infId);
void updateID(xrt::ip& ip, vector<xrt::xclbin::arg>& args, unsigned short localID);
void update_genID(xrt::ip& gen_ip, vector<xrt::xclbin::arg>& gen_args, unsigned short genID);
void getBufSize(xrt::ip& ip, vector<xrt::xclbin::arg>& args);
void turnON_fec(xrt::ip& ip, vector<xrt::xclbin::arg>& args, bool enable_fec);
#endif
