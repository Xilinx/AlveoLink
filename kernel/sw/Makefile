BUILD_DIR := build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1
SERVER_SRC=src/manager.cpp
SERVER_TARGET=$(BUILD_DIR)/manager.exe

CFLAGS=-O3 -std=c++11 -D AL_netDataBits=512 -I ./include -I ../hw/include 
	   
CXX=g++

${SERVER_TARGET}: ${SERVER_SRC}
	mkdir -p $(BUILD_DIR)
	${CXX} $^ -o $@ ${CFLAGS} 	

run: ${SERVER_TARGET}

clean:
	rm -rf ${SERVER_TARGET}
