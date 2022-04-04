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

#ifndef BASIC_EXCEPTION_HPP
#define BASIC_EXCEPTION_HPP

typedef enum {
    BASIC_STATUS_SUCCESS,            // success status
    BASIC_STATUS_FAILED_DEVICE_INIT, // unsupported behavior
    BASIC_STATUS_INVALID_VALUE       // unsupported behavior
} BASIC_Status_t;

class basicException : public std::exception {
   public:
    basicException(const std::string str, const BASIC_Status_t p_stat) : m_msg(str), m_status(p_stat) {}

    const char* what() const noexcept override { return m_msg.c_str(); }

    BASIC_Status_t getStatus() const { return m_status; }

   protected:
    std::string m_msg;
    BASIC_Status_t m_status;
};

class basicFailedDeviceInit : public basicException {
   public:
    basicFailedDeviceInit(std::string str)
        : basicException("INIT ERROR: " + str + "\n", BASIC_STATUS_FAILED_DEVICE_INIT) {}
};
class basicInvalidValue : public basicException {
   public:
    basicInvalidValue(std::string str) : basicException("ERROR: " + str + "\n", BASIC_STATUS_INVALID_VALUE) {}
};

#endif
