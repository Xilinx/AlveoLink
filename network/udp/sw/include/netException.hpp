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


#ifndef ALVEOLINK_NETWORK_EXCEPTION_HPP
#define ALVEOLINK_NETWORK_EXCEPTION_HPP

namespace AlveoLink {
namespace network_udp {

typedef enum {
    NET_STATUS_SUCCESS,        // success status
    NET_STATUS_FAILED_CU_INIT, // failed initializing CUs
    NET_STATUS_INVALID_IP,     // invalid IP address
    NET_STATUS_INVALID_VALUE   // invalid parameter value
} NET_Status_T;

class NetException : public std::exception {
   public:
    NetException(const std::string str, const NET_Status_T p_stat) : m_msg(str), m_status(p_stat) {}

    const char* what() const noexcept override { return m_msg.c_str(); }

    NET_Status_T getStatus() const { return m_status; }

   protected:
    std::string m_msg;
    NET_Status_T m_status;
};

class NetFailedCUInit : public NetException {
   public:
    NetFailedCUInit(std::string str) : NetException("CU INIT ERROR: " + str + "\n", NET_STATUS_FAILED_CU_INIT) {}
};

class NetInvalidValue : public NetException {
   public:
    NetInvalidValue(std::string str) : NetException("ERROR: " + str + "\n", NET_STATUS_INVALID_VALUE) {}
};

class NetInvalidIp : public NetException {
   public:
    NetInvalidIp(std::string str) : NetException("IP Address ERROR: " + str + "\n", NET_STATUS_INVALID_IP) {}
};
}
}
#endif
