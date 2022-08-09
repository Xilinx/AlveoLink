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
ROCEV2_DIR=$(AL_PATH)/network/roce_v2
XUPDIR=$(AL_PATH)/../xup_vitis_network_example
CMACDIR=$(XUPDIR)/Ethernet
NETLAYERDIR=$(AL_PATH)/network/roce_v2/hw/HiveNet

XSA := $(DEVICE)
TEMP_DIR := _x.$(XSA)

# Include cmac and HiveNet IPs depending on the interface
ifeq (3,$(INTERFACE))
	LIST_XO += $(CMACDIR)/$(TEMP_DIR)/cmac_0.xo
	LIST_XO += $(CMACDIR)/$(TEMP_DIR)/cmac_1.xo
	LIST_XO += $(NETLAYERDIR)/build/HiveNet_kerel_0.xo
	LIST_XO += $(NETLAYERDIR)/build/HiveNet_kerel_1.xo
else
	LIST_XO += $(CMACDIR)/$(TEMP_DIR)/cmac_$(INTERFACE).xo
	LIST_XO += $(NETLAYERDIR)/build/HiveNet_kerel_0.xo
endif

POSTSYSLINKTCL ?= $(shell readlink -f $(ROCEV2_DIR)/hw/post_sys_link.tcl)
#Create configuration file for current design and settings

ifeq ($(INTERFACE), 0)
create-conf-file: $(CUR_DIR)/conn_u55c_if0.cfg
	cp $^ ./conn_u55c_if0.tmp.cfg
	echo "[advanced]" >> conn_u55c_if0.tmp.cfg
	echo "param=compiler.userPostSysLinkOverlayTcl=$(POSTSYSLINKTCL)" >> conn_u55c_if0.tmp.cfg 
else ifeq ($(INTERFACE), 3)
create-conf-file: $(CUR_DIR)/conn_u55c_if2.cfg
	cp $^ ./conn_u55c_if2.tmp.cfg
	echo "[advanced]" >> conn_u55c_if2.tmp.cfg
	echo "param=compiler.userPostSysLinkOverlayTcl=$(POSTSYSLINKTCL)" >> conn_u55c_if2.tmp.cfg 
endif


# Building IPs 
buildcmac: $(XUPDIR)

$(CMACDIR)/$(TEMP_DIR)/%.xo:
	make -C $(CMACDIR) all DEVICE=$(DEVICE) INTERFACE=$(INTERFACE)

ifeq ($(INTERFACE), 0)
$(NETLAYERDIR)/build/%.xo:
	$(XILINX_VIVADO)/bin/vivado -mode batch -source HiveNet_bd.tcl -tclargs $(NETLAYERDIR)/src 0 0
else ifeq ($(INTERFACE), 3)
$(NETLAYERDIR)/build/HiveNet_kerel_0.xo:
	$(XILINX_VIVADO)/bin/vivado -mode batch -source HiveNet_bd.tcl -tclargs $(NETLAYERDIR)/src 0 0
$(NETLAYERDIR)/build/HiveNet_kerel_1.xo:
	$(XILINX_VIVADO)/bin/vivado -mode batch -source HiveNet_bd.tcl -tclargs $(NETLAYERDIR)/src 1 3
endif

$(XUPDIR):
	git clone https://github.com/Xilinx/xup_vitis_network_example.git $(XUPDIR)
	cd $(XUPDIR)
	git checkout @{c16cba6598099f87e203659d7f8d6a536d97ef86}
	cp $(ROCEV2_DIR)/hw/cmac/template.xml $(CMACDIR)
	cp $(ROCEV2_DIR)/hw/cmac/bd_cmac.tcl $(CMACDIR) 
