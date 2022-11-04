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

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <array>
#include <map>
#include <vector>
#include <bitset>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "pktType.hpp"
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

template <unsigned int t_MaxConnections, unsigned int t_MTUBytes, unsigned int t_NetDataBytes>
class Manager {
    public:
        static const unsigned int t_MaxStrobes = t_MTUBytes / t_NetDataBytes;
        static const unsigned int t_BufSize = t_MaxStrobes * 2;
        static const unsigned int t_MaxSeqNo = t_MaxStrobes - 1;
    public:
        Manager() {m_state = MANAGER_STATE::idle;}
        Manager(std::string& p_ipFileName, const unsigned int p_microSec4retran) {
            m_state = MANAGER_STATE::idle;
            init(p_ipFileName, p_microSec4retran);
        }
        void init(std::string& p_ipFileName, const unsigned int p_microSec4retran) {
            m_microSec4retran = p_microSec4retran;
            m_allDone.set();
            m_allIdle.set();
            m_allStart.set();
            m_wait.reset();
            m_numDevs = 0;
            std::ifstream l_ipFile(p_ipFileName);
            if (l_ipFile.is_open()) {
                std::string l_ipStr;
                while (getline(l_ipFile, l_ipStr)) {
                    m_ipTable.push_back(l_ipStr);
                    m_ipMap[l_ipStr] = m_numDevs;
                    m_numDevs++;
                }
                l_ipFile.close();
            }
            m_numDevs--;

            m_sentSeqNo.resize(m_numDevs);
            m_recSeqNo.resize(m_numDevs);
    
            for (auto i=0; i<m_numDevs; ++i) {
                 m_sentSeqNo[i] = 0;
                 m_recSeqNo[i] = 0;
                 m_sendBuf[i].resize(t_MaxStrobes*2);
            }

            if ((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket creation failed");
            }
            memset(&(m_servaddr), 0, sizeof(m_servaddr));
            m_servaddr.sin_family = AF_INET; // IPv4
            m_servaddr.sin_addr.s_addr = inet_addr(m_ipTable[m_numDevs].c_str());
            m_servaddr.sin_port = htons(PORT);

            // Bind the socket with the server address
            if (bind(m_sockfd, (const struct sockaddr*)&m_servaddr, sizeof(m_servaddr)) < 0) {
                perror("bind failed");
            }

            //set timeout for recvfrom
            struct timeval l_readTimeout;
            l_readTimeout.tv_sec = 0;
            l_readTimeout.tv_usec = p_microSec4retran;
            setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, &l_readTimeout, sizeof(l_readTimeout));
        }

        unsigned int getPktClass(const std::vector<uint8_t>& p_pkt) {
            return p_pkt[0];
        }
        unsigned int getPktSeqNo(const std::vector<uint8_t>& p_pkt) {
            return p_pkt[1];
        }

        unsigned int getPktType(const std::vector<uint8_t>& p_pkt) {
            return (p_pkt[2] >> 4);
        }
        uint16_t getSrcId(const std::vector<uint8_t>& p_pkt) {
            return((p_pkt[4]<<8) + p_pkt[3]);
        }
        bool hasReceived(struct sockaddr_in& p_cliaddr, std::vector<uint8_t>& p_pktBuf, unsigned int& p_id) {
            unsigned int l_id = 0;
            int l_len = sizeof(struct sockaddr_in);
            int l_recBytes = recvfrom(m_sockfd, p_pktBuf.data(), t_NetDataBytes, MSG_WAITALL, (struct sockaddr*)&p_cliaddr, (socklen_t*)&l_len);
            if (l_recBytes != t_NetDataBytes) {//timeout not received
                return false;
            }
            else {
                char l_ipCstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(p_cliaddr.sin_addr), l_ipCstr, INET_ADDRSTRLEN);
                std::string l_ipStr(l_ipCstr);
                l_id = m_ipMap[l_ipStr];
                unsigned int l_pktClass = getPktClass(p_pktBuf);
                unsigned int l_pktNo = getPktSeqNo(p_pktBuf);
                unsigned int l_expRecNo = m_recSeqNo[l_id];
                unsigned int l_sendSeqNo = m_sentSeqNo[l_id];
                bool l_missingPkts = (l_pktNo > l_expRecNo);

                std::vector<uint8_t> l_ackPkt(t_NetDataBytes);
                if (l_pktClass == AlveoLink::adapter::XNIK_PKT_TYPE::DATA) {
                    if (l_pktNo == l_expRecNo) {
                        m_recSeqNo[l_id] = (l_expRecNo >= t_MaxSeqNo)? 0: l_expRecNo+1;
                    }
                    else if (l_pktNo < l_expRecNo) { //rubbish data drop it
                        return false;
                    }
                    l_ackPkt[0] = AlveoLink::adapter::XNIK_PKT_TYPE::ACK;
                    l_ackPkt[1] = m_recSeqNo[l_id];
                    sendto(m_sockfd, l_ackPkt.data(), t_NetDataBytes, 0, (const struct sockaddr*)&p_cliaddr, l_len);
                    //std::cout << "INFO: received DATA pkt." << std::endl;
                }
                else if (l_pktClass == AlveoLink::adapter::XNIK_PKT_TYPE::ACK) {
                    if (l_pktNo == l_sendSeqNo) { // received
                        m_wait[l_id] = false;
                    }
                    else if (l_pktNo < l_sendSeqNo) { //re-tran
                        for (auto i=l_pktNo; i<l_sendSeqNo; ++i) {
                            sendto(m_sockfd, m_sendBuf[l_id][i].data(), t_NetDataBytes, 0, (const struct sockaddr*)&p_cliaddr, l_len);
                        }
                    }
                    else if (l_sendSeqNo == 0) {
                        for (unsigned int i=l_pktNo; i<t_MaxStrobes; ++i) {
                            sendto(m_sockfd, m_sendBuf[l_id][i].data(), t_NetDataBytes, 0, (const struct sockaddr*)&p_cliaddr, l_len);
                        }
                    }
                    //std::cout << "INFO: received ACK pkt." << std::endl;
                }
                else if (l_pktClass == AlveoLink::adapter::XNIK_PKT_TYPE::QUERY) {
                    std::vector<uint8_t> l_ackPkt(t_NetDataBytes);
                    l_ackPkt[0] = AlveoLink::adapter::XNIK_PKT_TYPE::ACK;
                    l_ackPkt[1] = m_recSeqNo[l_id];
                    sendto(m_sockfd, l_ackPkt.data(), t_NetDataBytes, 0, (const struct sockaddr*)&p_cliaddr, l_len);
                    //std::cout << "INFO: received QUERY pkt." << std::endl;
                } 
            }
            p_id = l_id;
            return true;
        }
        void recAllPkts() {
            std::vector<struct sockaddr_in> l_cliaddrs(m_numDevs);
            int l_len = sizeof(struct sockaddr_in);
            for (auto i=0; i < m_numDevs; ++i) {
                std::vector<uint8_t> l_recPkt(t_NetDataBytes);
                memset(&(l_cliaddrs[i]), 0, l_len);
                l_cliaddrs[i].sin_family = AF_INET;
                l_cliaddrs[i].sin_port = htons(PORT);
                unsigned int l_recId = 0;
                bool l_hasReceived = hasReceived(l_cliaddrs[i], l_recPkt, l_recId);
                unsigned int l_pktClass = getPktClass(l_recPkt);
                unsigned int l_pktType = getPktType(l_recPkt);
                if (l_hasReceived && (l_pktClass == AlveoLink::adapter::XNIK_PKT_TYPE::DATA)) {
                    m_allStart[l_recId] = (l_pktType == PKT_TYPE::start);
                    m_allDone[l_recId] = ((l_pktType == PKT_TYPE::done) || (l_pktType == PKT_TYPE::idle_after_done));
                    m_allIdle[l_recId] = (l_pktType == PKT_TYPE::idle_after_done);
                }
            }
        }
        void sendQueryAckPkts() {
            std::vector<struct sockaddr_in> l_cliaddrs(m_numDevs);
            int l_len = sizeof(struct sockaddr_in);
            for (unsigned int i=0; i<m_numDevs; ++i) {
                memset(&(l_cliaddrs[i]), 0, l_len);
                l_cliaddrs[i].sin_family = AF_INET;
                l_cliaddrs[i].sin_addr.s_addr = inet_addr(m_ipTable[i].c_str());
                l_cliaddrs[i].sin_port = htons(PORT);
                if (m_wait[i]) {
                    std::vector<uint8_t> l_sendPkt(t_NetDataBytes);
                    l_sendPkt[0] = AlveoLink::adapter::XNIK_PKT_TYPE::QUERY;
                    l_sendPkt[1] = m_sentSeqNo[i];
                    sendto(m_sockfd, l_sendPkt.data(), t_NetDataBytes, 0, (const struct sockaddr*)&(l_cliaddrs[i]), l_len);
                }
            }
        }
        void sendAllPkts(const PKT_TYPE p_pktType) {
            std::vector<struct sockaddr_in> l_cliaddrs(m_numDevs);
            int l_len = sizeof(struct sockaddr_in);
            for (unsigned int i=0; i<m_numDevs; ++i) {
                memset(&(l_cliaddrs[i]), 0, l_len);
                l_cliaddrs[i].sin_family = AF_INET;
                l_cliaddrs[i].sin_addr.s_addr = inet_addr(m_ipTable[i].c_str());
                l_cliaddrs[i].sin_port = htons(PORT);

                uint8_t l_sendSeqNo = m_sentSeqNo[i];
                std::vector<uint8_t> l_sendPkt(t_NetDataBytes);
                l_sendPkt[0] = 0;
                l_sendPkt[1] = l_sendSeqNo;
                l_sendPkt[2] = p_pktType;
                l_sendPkt[2] = l_sendPkt[2] << 4;
                sendto(m_sockfd, l_sendPkt.data(), t_NetDataBytes, 0, (const struct sockaddr*)&(l_cliaddrs[i]), l_len);
                memcpy(m_sendBuf[i][l_sendSeqNo].data(), l_sendPkt.data(), t_NetDataBytes);
                m_sentSeqNo[i] = (m_sentSeqNo[i] == t_MaxSeqNo)?0: m_sentSeqNo[i]+1;
                m_wait[i] = true;
                m_allDone[i] = false;
                m_allIdle[i] = false;
            }
            TimePointType l_startWait = std::chrono::high_resolution_clock::now();
            while (m_wait.any()) {
                recAllPkts();
                TimePointType l_endWait = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> l_waitSec = l_endWait - l_startWait; 
                if (l_waitSec.count() * 1e6 > m_microSec4retran) {
                    sendQueryAckPkts(); 
                }
            }
        }
        double process(const unsigned int p_flushCounter, const unsigned int p_microSec4query) {
            unsigned int l_flushCounter = p_flushCounter;
            bool m_exit = false;
            int l_len = sizeof(struct sockaddr_in);
            
            TimePointType l_start;
            for (auto i=0; i<m_numDevs; ++i) {
                m_allStart[i] = false;
            }
            bool l_exit = false;
            while (!l_exit) {
                switch (m_state) {
                    case MANAGER_STATE::idle:
                        recAllPkts();
                        if (m_allStart.all()) {
                            sendAllPkts(PKT_TYPE::start);
                            l_start = std::chrono::high_resolution_clock::now();
                            std::cout << "INFO: go active ..." << std::endl; 
                            m_state = MANAGER_STATE::active;
                        }
                        
                    break;
                    case MANAGER_STATE::active:
                        recAllPkts();
                        if (m_allDone.all()) {
                            sendAllPkts(PKT_TYPE::query_status);
                            std::cout << "INFO: go query ..." << std::endl; 
                            m_state = MANAGER_STATE::query;
                        }
                    break;
                    case MANAGER_STATE::query:
                        recAllPkts();
                        if (m_allDone.all()) {
                            sendAllPkts(PKT_TYPE::query_status);
                            if (m_allIdle.all()) {
                                std::cout << "INFO: go termination ..." << std::endl; 
                                m_state = MANAGER_STATE::termination;
                            }
                        }
                    break;
                    case MANAGER_STATE::termination:
                        recAllPkts();
                        if (m_allDone.all()) {
                            if (m_allIdle.all()) {
                                std::cout << "INFO: received all idle_after_done packts. flushCounter = " << l_flushCounter << std::endl;
                                if (l_flushCounter == 0) {
                                    sendAllPkts(PKT_TYPE::terminate);
                                    l_exit = true;
                                    std::cout << "INFO: go idle ..." << std::endl; 
                                    m_state = MANAGER_STATE::idle; 
                                }
                                else {
                                    usleep(p_microSec4query);
                                    sendAllPkts(PKT_TYPE::query_status);
                                    l_flushCounter--;
                                }
                            }
                            else {
                                sendAllPkts(PKT_TYPE::query_status);
                                l_flushCounter = p_flushCounter;
                                std::cout << "INFO: go query ..." << std::endl; 
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
        unsigned int m_microSec4retran;
        unsigned int m_numDevs; 
        int m_sockfd;
        struct sockaddr_in m_servaddr;
        std::vector<std::string> m_ipTable;
        std::map<std::string, int> m_ipMap;
        std::vector<uint8_t> m_sentSeqNo;
        std::vector<uint8_t> m_recSeqNo;
        std::vector<std::array<uint8_t, t_NetDataBytes> > m_sendBuf[t_MaxConnections];
        std::bitset<t_MaxConnections> m_allStart;
        std::bitset<t_MaxConnections> m_wait;
        std::bitset<t_MaxConnections> m_allDone;
        std::bitset<t_MaxConnections> m_allIdle;
};

}
}

#endif
