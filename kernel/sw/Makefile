# Copyright 2019-2022 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

BUILD_DIR := build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1
SERVER_SRC=src/manager.cpp
SERVER_TARGET=$(BUILD_DIR)/manager.exe

CFLAGS=-g -O0 -std=c++11 -D AL_mtuBytes=1472 -D AL_maxConnections=16 -D AL_netDataBits=512 -D AL_userBits=1 -D AL_destBits=16 -I $(XILINX_HLS)/include -I ./include -I ../hw/include -I ../../adapter/udp/hw/include -pthread 
	   
CXX=g++

${SERVER_TARGET}: ${SERVER_SRC}
	mkdir -p $(BUILD_DIR)
	${CXX} $^ -o $@ ${CFLAGS} 	

server: ${SERVER_TARGET}

clean:
	rm -rf ${SERVER_TARGET}
