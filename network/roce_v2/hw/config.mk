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

INTERFACE ?= 0
ROCEV2_DIR=$(AL_PATH)/network/roce_v2/

XSA := $(DEVICE)
TEMP_DIR := _x.$(XSA)

LIST_XO += $(ROCEV2_DIR)/hw/ip/HiveNet_kernel_0.xo
LIST_XO += $(ROCEV2_DIR)/hw/ip/cmac_0.xo

VPP_LDFLAGS += --config $(ROCEV2_DIR)/hw/conn_u55c_if0.cfg

POSTSYSLINKTCL ?= $(shell readlink -f $(ROCEV2_DIR)/hw/post_sys_link.tcl)


#Create configuration file for current design and settings

ifeq ($(INTERFACE), 0)
create-conf-file: $(CUR_DIR)/conn_u55c_if0.cfg
	cp $^ ./conn_u55c_if0.tmp.cfg
	echo "[advanced]" >> conn_u55c_if0.tmp.cfg
	echo "param=compiler.userPostSysLinkOverlayTcl=$(POSTSYSLINKTCL)" >> conn_u55c_if0.tmp.cfg 
else ifeq ($(INTERFACE), 2)
create-conf-file: $(CUR_DIR)/conn_u55c_if2.cfg
	cp $^ ./conn_u55c_if2.tmp.cfg
	echo "[advanced]" >> conn_u55c_if2.tmp.cfg
	echo "param=compiler.userPostSysLinkOverlayTcl=$(POSTSYSLINKTCL)" >> conn_u55c_if2.tmp.cfg 
endif
