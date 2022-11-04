# Copyright 2019-2021 Xilinx, Inc.
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
# vitis makefile-generator v2.0.4

############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) ""
	$(ECHO) "  make host HOST_ARCH=<hw/hw_emu/sw_emu/>"
	$(ECHO) "      Command to build host application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
############################## Setting up Project Variables ##############################

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/L2/*}')
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XFLIB_DIR = $(XF_PROJ_ROOT)

# setting devault value
TARGET ?= sw_emu

BUILD_DIR := build_dir.$(TARGET).xilinx_u55c_gen3x16_xdma_2_202110_1

CXXFLAGS += -fmessage-length=0 -I$(CUR_DIR)/src/ -I$(XILINX_XRT)/include --sysroot=$(SYSROOT)  -I$(SYSROOT)/usr/include/xrt -I$(XILINX_HLS)/include -I$(XF_PROJ_ROOT)/ext -std=c++14 -Wall -Wno-unknown-pragmas -Wno-unused-label 
LDFLAGS += -pthread -L$(XILINX_XRT)/lib -lxrt_coreutil -L$(XILINX_HLS)/lnx64/tools/fpo_v7_0  -Wl,--as-needed

#Inclue Required Host Source Files
HOST_SRCS += $(XFLIB_DIR)/L2/nhop_xnik_dv/host/partition_main.cpp  
CXXFLAGS +=  -I $(XFLIB_DIR)/L2/nhop_xnik_dv/host -I $(AL_PATH)/common/sw/include
CXXFLAGS += -g -O0 
CXXFLAGS += -Wno-unused-variable -Wno-format -Wno-sign-compare

EXE_NAME := partition.exe
EXE_FILE := $(BUILD_DIR)/$(EXE_NAME)
EXE_FILE_DEPS := $(HOST_SRCS)

#HOST_ARGS := --graph $(CUR_DIR)/data/example-wt.txt --pair $(CUR_DIR)/data/data-pair.mtx --hop 2 --batch 1024
HOST_ARGS := --graph /proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT/cit-Patents-wt.mtx --pair data/cit-Patents-twoHopPair1000.mtx --hop 2 --limit 5 --numKernel 2 --batch 512
#HOST_ARGS := --graph /proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT/cit-Patents-wt.mtx --pair /proj/xsjhdstaff5/siyangw/nobkup/git_hub_test/pair_small/cit-Patents-twoHopPair.mtx --hop 2 --batch 512
#HOST_ARGS := --graph /proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT/cit-Patents-wt.mtx --pair data/cit-Patents-twoHopPair1000.mtx --hop 2 --limit 4 --numKernel 3 --batch 512
#HOST_ARGS := --offset $(CUR_DIR)/data/data-csr-offset.mtx --index $(CUR_DIR)/data/data-csr-indicesweights.mtx --pair $(CUR_DIR)/data/data-pair.mtx --hop 2 --batch 1024

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXE_FILE): $(EXE_FILE_DEPS)
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

run: $(EXE_FILE)
	$(EXE_FILE) $(HOST_ARGS)


############################## Setting Targets ##############################

.PHONY: host
host:   $(EXE_FILE)

############################## Cleaning Rules ##############################
clean:
	rm -rf ${EXE_FILE}
