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
#ifndef XNATIVEFPGA_EXCEPTION_HPP
#define XNATIVEFPGA_EXCEPTION_HPP

namespace AlveoLink {
namespace common {

typedef enum {
    XNATIVEFPGA_STATUS_SUCCESS, // success status
    XNATIVEFPGA_STATUS_INVALID_VALUE
} XNATIVEFPGA_Status_t;

class xNativeFPGAException : public std::exception {
   public:
    xNativeFPGAException(const std::string str, const XNATIVEFPGA_Status_t p_stat) : m_msg(str), m_status(p_stat) {}

    const char* what() const noexcept override { return m_msg.c_str(); }

    XNATIVEFPGA_Status_t getStatus() const { return m_status; }

   protected:
    std::string m_msg;
    XNATIVEFPGA_Status_t m_status;
};

class xNativeFPGAInvalidValue : public xNativeFPGAException {
   public:
    xNativeFPGAInvalidValue(std::string str)
        : xNativeFPGAException("INVALID VALUE ERROR: " + str + "\n", XNATIVEFPGA_STATUS_INVALID_VALUE) {}
};
}
}

#endif
