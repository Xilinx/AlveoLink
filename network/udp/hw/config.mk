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

NETLAYERDIR = $(XFLIB_DIR)/network/udp/hw/vnx/NetLayers
CMACDIR     = $(XFLIB_DIR)/network/udp/hw/vnx/Ethernet
XSA := $(PLATFORM_NAME)
TEMP_DIR := _x.$(XSA)

LIST_XO :=

NETLAYERHLS = 100G-fpga-network-stack-core

POSTSYSLINKTCL ?= $(shell readlink -f $(XFLIB_DIR)/network/udp/hw/vnx/Ethernet/post_sys_link.tcl)

LIST_XO += $(NETLAYERDIR)/$(TEMP_DIR)/networklayer.xo


# Include cmac kernel depending on the interface
ifeq (3,$(INTERFACE))
	LIST_XO += $(CMACDIR)/$(TEMP_DIR)/cmac_0.xo
	LIST_XO += $(CMACDIR)/$(TEMP_DIR)/cmac_1.xo
else
	LIST_XO += $(CMACDIR)/$(TEMP_DIR)/cmac_$(INTERFACE).xo
endif

# Linker parameters
# Linker userPostSysLinkTcl param
ifeq (u5,$(findstring u5, $(DEVICE)))
	HLS_IP_FOLDER  = $(shell readlink -f $(NETLAYERDIR)/$(NETLAYERHLS)/synthesis_results_HMB)
endif
ifeq (u200,$(findstring u200, $(DEVICE)))
	HLS_IP_FOLDER  = $(shell readlink -f $(NETLAYERDIR)/$(NETLAYERHLS)/synthesis_results_noHMB)
endif
ifeq (u250,$(findstring u250, $(DEVICE)))
	HLS_IP_FOLDER  = $(shell readlink -f $(NETLAYERDIR)/$(NETLAYERHLS)/synthesis_results_noHMB)
endif
ifeq (u280,$(findstring u280, $(DEVICE)))
	HLS_IP_FOLDER  = $(shell readlink -f $(NETLAYERDIR)/$(NETLAYERHLS)/synthesis_results_HMB)
endif

distclean:
	rm -rf *v++* *.log *.jou *.str
	rm -rf _x* *.tmp.cfg .Xil .ipcache/

distcleanall: distclean
	make -C $(NETLAYERDIR) distclean
	make -C $(CMACDIR) distclean

#Create configuration file for current design and settings
create-conf-file:
	cp ./conn_if0.cfg ./conn_if0.tmp.cfg
	echo "[advanced]" >> conn_if0.tmp.cfg
	echo "param=compiler.userPostSysLinkOverlayTcl=$(POSTSYSLINKTCL)" >> conn_if0.tmp.cfg 

# Building kernel
buildip:
	make -C $(NETLAYERDIR)/$(NETLAYERHLS) hbm
    
$(CMACDIR)/$(TEMP_DIR)/%.xo:
	make -C $(CMACDIR) all DEVICE=$(DEVICE) INTERFACE=$(INTERFACE)

$(NETLAYERDIR)/$(TEMP_DIR)/%.xo:
	make -C $(NETLAYERDIR) all DEVICE=$(DEVICE)

