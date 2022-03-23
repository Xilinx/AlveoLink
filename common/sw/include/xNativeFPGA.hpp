/*
 * Copyright 2019-2021 Xilinx, Inc.
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

#ifndef XNATIVEFPGA_HPP
#define XNATIVEFPGA_HPP
#include <iostream>
#include <vector>
#include <regex>
#include <unordered_map>

// This file is required for OpenCL C++ wrapper APIs
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_ip.h"
#include "experimental/xrt_kernel.h"

namespace AlveoLink {
namespace common {

class FPGA {
   public:
    FPGA() = default;
    void setId(const int p_id);
    void load_xclbin(const std::string& xclbin_fnm);
    const xrt::device& getDevice() const;
    const xrt::uuid& getUUID() const;

   private:
    int m_id;
    xrt::device m_device;
    xrt::uuid m_uuid;
};

class IP {
   public:
    IP() = default;
    void fpga(FPGA* p_fpga);
    void getIP(const std::string& p_ipName);
    uint32_t readReg(const size_t p_regOffset) const;
    void writeReg(const size_t p_regOffset, const uint32_t p_regVal);

   protected:
    FPGA* m_fpga;
    xrt::ip m_ip;
};

class KERNEL {
   public:
    KERNEL() = default;
    void fpga(FPGA* p_fpga);
    void createKernel(const std::string& name);
    void* createBO(const int p_argIdx, const size_t p_bytes);
    void createBOfromHostPtr(const int p_argIdx, const size_t p_bytes, void* p_hostPtr); //p_hostPtr must be 4K aligned
    void sendBO(const int p_argIdx);
    void setMemArg(const int p_argIdx);
    void setMemArg(const int p_argIdx1, const int p_argIdx2);
    template <typename t_Type>
    void setScalarArg(const int p_argIdx, t_Type p_argVal);
    void run();
    void wait();
    void getBO(const int p_argIdx);
    void clearBOMap();

   protected:
    FPGA* m_fpga;
    xrt::kernel m_kernel;
    std::map<const int, xrt::bo> m_bos; // map arg index to bo
    xrt::run m_run;
};
}
}
#endif
