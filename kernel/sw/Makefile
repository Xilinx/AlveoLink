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
