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

#ifndef GRAPHSYNC_MANAGER_HPP
#define GRAPHSYNC_MANAGER_HPP

#include <chrono>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "graphPktDefs.hpp"

#define PORT 62781

namespace AlveoLink {
namespace kernel {

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePointType;

typedef enum {
    idle = 0,
    active,
    query,
    termination
} MANAGER_STATE;

template <unsigned int t_NetDataBytes>
class Manager {
    public:
        Manager() {m_state = MANAGER_STATE::idle;}
        Manager(std::string& p_ipFileName) {
            m_state = MANAGER_STATE::idle;
            init(p_ipFileName);
        }
        void init(std::string& p_ipFileName) {
            m_numDevs = 0;
            std::ifstream l_ipFile(p_ipFileName);
            if (l_ipFile.is_open()) {
                std::string l_ipStr;
                while (getline(l_ipFile, l_ipStr)) {
                    m_ipTable.push_back(l_ipStr);
                    m_numDevs++;
                }
                l_ipFile.close();
            }
            m_numDevs--;

            if ((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket creation failed");
            }
            memset(&(m_servaddr), 0, sizeof(m_servaddr));
            m_servaddr.sin_family = AF_INET; // IPv4
            m_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            m_servaddr.sin_port = htons(PORT);

            // Bind the socket with the server address
            if (bind(m_sockfd, (const struct sockaddr*)&m_servaddr, sizeof(m_servaddr)) < 0) {
                perror("bind failed");
            }
            
        }

        unsigned int getPktType(const std::vector<uint8_t>& p_pkt) {
            return (p_pkt[2] >> 4);
        }
        uint16_t getSrcId(const std::vector<uint8_t>& p_pkt) {
            return((p_pkt[4]<<8) + p_pkt[3]);
        }
        bool hasRecAllDevIds(const std::vector<uint16_t>& p_devIds){
            bool l_res = true;
            if (p_devIds.size() != m_numDevs) {
                l_res = false;
            }
            return l_res;
            unsigned int l_sumExp, l_sumOut;
            l_sumExp = 0;
            l_sumOut = 0;
            for (unsigned i=0; i<m_numDevs; ++i) {
                l_sumExp += i;
                l_sumOut += p_devIds[i];
            } 
            return (l_sumExp == l_sumOut);
        }
        bool hasRecAllPkts(const PKT_TYPE p_pktType) {
            std::vector<struct sockaddr_in> l_cliaddrs(m_numDevs);
            int l_len = sizeof(struct sockaddr_in);
            bool l_success = true;
            int l_numPkts = 0;
            std::vector<uint16_t> l_srcIds;
            while (l_numPkts < m_numDevs) {
                std::vector<uint8_t> l_recPkt(t_NetDataBytes);
                memset(&(l_cliaddrs[l_numPkts]), 0, l_len);
                int l_recBytes = recvfrom(m_sockfd, l_recPkt.data(), t_NetDataBytes, MSG_WAITALL, (struct sockaddr*)&(l_cliaddrs[l_numPkts]), (socklen_t*)&l_len);
                if (l_recBytes != t_NetDataBytes) {
                    l_success = false;
                }
                unsigned int l_pktType = getPktType(l_recPkt);
                if (l_pktType != p_pktType) {
                    perror("received wrong packet type.");
                    l_success = false;
                }
                uint16_t l_srcId = getSrcId(l_recPkt);
                l_srcIds.push_back(l_srcId);
                l_numPkts++;
            }
            if (l_success) {
                l_success = hasRecAllDevIds(l_srcIds);
            }
            return l_success;
        }
        void sendAllPkts(const PKT_TYPE p_pktType) {
            std::vector<struct sockaddr_in> l_cliaddrs(m_numDevs);
            int l_len = sizeof(struct sockaddr_in);
            for (unsigned int i=0; i<m_numDevs; ++i) {
                memset(&(l_cliaddrs[i]), 0, l_len);
                l_cliaddrs[i].sin_family = AF_INET;
                l_cliaddrs[i].sin_addr.s_addr = inet_addr(m_ipTable[i].c_str());
                l_cliaddrs[i].sin_port = htons(PORT);

                std::vector<uint8_t> l_sendPkt(t_NetDataBytes);
                l_sendPkt[0] = i;
                l_sendPkt[1] = 0;
                l_sendPkt[2] = p_pktType;
                l_sendPkt[2] = l_sendPkt[2] << 4;
                sendto(m_sockfd, l_sendPkt.data(), t_NetDataBytes, 0, (const struct sockaddr*)&(l_cliaddrs[i]), l_len);
            }
        }
        bool hasRecAllDonePkts(bool& p_allIdle) {
            std::vector<struct sockaddr_in> l_cliaddrs(m_numDevs);
            int l_len = sizeof(struct sockaddr_in);
            bool l_success = true;
            int l_numPkts = 0;
            std::vector<uint16_t> l_srcIds;
            p_allIdle = false;
            while (l_numPkts < m_numDevs) {
                std::vector<uint8_t> l_recPkt(t_NetDataBytes);
                memset(&(l_cliaddrs[l_numPkts]), 0, l_len);
                int l_recBytes = recvfrom(m_sockfd, l_recPkt.data(), t_NetDataBytes, MSG_WAITALL, (struct sockaddr*)&(l_cliaddrs[l_numPkts]), (socklen_t*)&l_len);
                if (l_recBytes != t_NetDataBytes) {
                    l_success = false;
                }
                unsigned int l_pktType = getPktType(l_recPkt);
                if ((l_pktType != PKT_TYPE::done) && (l_pktType != PKT_TYPE::idle_after_done)) {
                    perror("received wrong packet type.");
                    l_success = false;
                }
                if (l_pktType != PKT_TYPE::idle_after_done) {
                    p_allIdle = false;
                }
                uint16_t l_srcId = getSrcId(l_recPkt);
                l_srcIds.push_back(l_srcId);
                l_numPkts++;
            }
            if (l_success) {
                l_success = hasRecAllDevIds(l_srcIds);
            }
            if (l_success) {
                p_allIdle = true;
            }
            return l_success;
    }
        double process(const unsigned int p_flushCounter, const unsigned int p_microseconds) {
            unsigned int l_flushCounter = p_flushCounter;
            bool l_exit = false;
            int l_len = sizeof(struct sockaddr_in);
            
            TimePointType l_start;
            bool l_allIdle = true;
            while (!l_exit) {
                switch (m_state) {
                    case MANAGER_STATE::idle:
                        if (hasRecAllPkts(PKT_TYPE::start)) {
                            sendAllPkts(PKT_TYPE::start);
                            l_start = std::chrono::high_resolution_clock::now();
                            m_state = MANAGER_STATE::active;
                        }
                    break;
                    case MANAGER_STATE::active:
                        if (hasRecAllDonePkts(l_allIdle)) {
                            sendAllPkts(PKT_TYPE::query_status);
                            m_state = MANAGER_STATE::query;
                        }
                    break;
                    case MANAGER_STATE::query:
                        if (hasRecAllDonePkts(l_allIdle)) {
                            sendAllPkts(PKT_TYPE::query_status);
                            if (l_allIdle) {
                                m_state = MANAGER_STATE::termination;
                            }
                        }
                    break;
                    case MANAGER_STATE::termination:
                        if (hasRecAllDonePkts(l_allIdle)) {
                            if (l_allIdle) {
                                std::cout << "INFO: received all idle_after_done packts. flushCounter = " << l_flushCounter << std::endl;
                                if (l_flushCounter == 0) {
                                    sendAllPkts(PKT_TYPE::terminate);
                                    l_exit = true;
                                    m_state = MANAGER_STATE::idle; 
                                }
                                else {
                                    usleep(p_microseconds);
                                    sendAllPkts(PKT_TYPE::query_status);
                                    l_flushCounter--;
                                }
                            }
                            else {
                                sendAllPkts(PKT_TYPE::query_status);
                                l_flushCounter = p_flushCounter;
                                m_state = MANAGER_STATE::query;
                            }
                        }
                        
                    break;
                }
            }
            TimePointType l_end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> l_durationSec = l_end - l_start;
            double l_timeSec = l_durationSec.count();
            return l_timeSec;
        }
    private:
        MANAGER_STATE m_state;
        unsigned int m_numDevs; 
        int m_sockfd;
        struct sockaddr_in m_servaddr;
        std::vector<std::string> m_ipTable;
};

}
}

#endif
