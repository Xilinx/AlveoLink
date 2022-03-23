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

#include "common/xNativeFPGA.hpp"
#include "common/xNativeFPGAExpection.hpp"

namespace AlveoLink {
namespace common {

void FPGA::setId(const int p_id) {
    m_id = p_id;
    m_device = xrt::device(p_id);
}
void FPGA::load_xclbin(const std::string& xclbin_fnm) {
    m_uuid = m_device.load_xclbin(xclbin_fnm);
}
const xrt::device& FPGA::getDevice() const {
    return m_device;
}
const xrt::uuid& FPGA::getUUID() const {
    return m_uuid;
}

void IP::fpga(FPGA* p_fpga) {
    m_fpga = p_fpga;
}
void IP::getIP(const std::string& p_ipName) {
    m_ip = xrt::ip(m_fpga->getDevice(), m_fpga->getUUID(), p_ipName);
}
uint32_t IP::readReg(const size_t p_regOffset) const {
    return (m_ip.read_register(p_regOffset));
}
void IP::writeReg(const size_t p_regOffset, const uint32_t p_regVal) {
    m_ip.write_register(p_regOffset, p_regVal);
}

void KERNEL::fpga(FPGA* p_fpga) {
    m_fpga = p_fpga;
}

void KERNEL::createKernel(const std::string& name) {
    m_kernel = xrt::kernel(m_fpga->getDevice(), m_fpga->getUUID().get(), name);
    m_run = xrt::run(m_kernel);
}

void* KERNEL::createBO(const int p_argIdx, const size_t p_bytes) {
    xrt::bo l_bo = xrt::bo(m_fpga->getDevice(), p_bytes, m_kernel.group_id(p_argIdx));
    m_bos.insert({p_argIdx, l_bo});
    void* l_mem = l_bo.map<void*>();
    return l_mem;
}

//p_hostPtr must be 4K aligned
void KERNEL::createBOfromHostPtr(const int p_argIdx, const size_t p_bytes, void* p_hostPtr) {
    xrt::bo l_bo = xrt::bo(m_fpga->getDevice(), p_hostPtr, p_bytes, m_kernel.group_id(p_argIdx));
    m_bos.insert({p_argIdx, l_bo});
}

void KERNEL::sendBO(const int p_argIdx) {
    xrt::bo l_bo = m_bos.find(p_argIdx)->second;
    l_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
}

void KERNEL::setMemArg(const int p_argIdx) {
    if (m_bos.find(p_argIdx) != m_bos.end()) {
        xrt::bo l_bo = m_bos.find(p_argIdx)->second;
        m_run.set_arg(p_argIdx, l_bo);
    } else {
        throw AlveoLink::common::xNativeFPGAInvalidValue("could not find the BO");
    }
}

void KERNEL::setMemArg(const int p_argIdx1, const int p_argIdx2) {
    if (m_bos.find(p_argIdx1) != m_bos.end()) {
        xrt::bo l_bo = m_bos.find(p_argIdx1)->second;
        m_run.set_arg(p_argIdx1, l_bo);
        m_run.set_arg(p_argIdx2, l_bo);
    } else {
        throw AlveoLink::common::xNativeFPGAInvalidValue("could not find the BO");
    }
}

template <typename t_Type>
void KERNEL::setScalarArg(const int p_argIdx, t_Type p_argVal) {
    m_run.set_arg(p_argIdx, p_argVal);
}

void KERNEL::run() {
    m_run.start();
}

void KERNEL::getBO(const int p_argIdx) {
    auto state = m_run.wait();
    //std::cout << "state " << state << std::endl; // ERT_CMD_STATE_COMPLETED = 4
    if (m_bos.find(p_argIdx) != m_bos.end()) {
        xrt::bo l_bo = m_bos.find(p_argIdx)->second;
        l_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    } else {
        throw AlveoLink::common::xNativeFPGAInvalidValue("could not find the BO");
    }
}

void KERNEL::wait() {
    auto state = m_run.wait();
    //std::cout << "state " << state << std::endl;
}

void KERNEL::clearBOMap() {
    m_bos.clear();
}

template void KERNEL::setScalarArg<unsigned int>(const int, unsigned int);
}
}
