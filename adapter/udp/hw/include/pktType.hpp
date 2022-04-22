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

#ifndef ALVEOLINK_ADAPTER_PKTTYPE_HPP
#define ALVEOLINK_ADAPTER_PKTTYPE_HPP

namespace AlveoLink {
namespace adapter {

typedef enum {
    DATA=0,
    ACK,  //pkt type used in network pkt
    SEND_ACK, //pkt type used in ackStr
    REC_ACK, //pkt type used in ackStr
    QUERY
} XNIK_PKT_TYPE;

}
}
#endif
