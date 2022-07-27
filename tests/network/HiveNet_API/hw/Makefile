#
# this file was created by a computer. trust it.
#


# ######################## Setting up Project Variables #################################
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/tests/*}')
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XFLIB_DIR = $(XF_PROJ_ROOT)

# ######################### Include environment variables in utils.mk ####################
include ../utils.mk

VPP_LINKER ?= ${XILINX_VITIS}/bin/v++
RM = rm -f
RMDIR = rm -rf
TARGET = hw

VITIS_PLATFORM = xilinx_u55c_gen3x16_xdma_3_202210_1
VITIS_PLATFORM_PATH = $(VITIS_PLATFORM)

VPP_OPTS = --target hw

#
# Hw kernel files
#

BINARY_CONTAINERS += binary_container_1.xclbin

BUILD_SUBDIRS += binary_container_1.build
BINARY_CONTAINER_1_OBJS += $(XFLIB_DIR)/network/roce_v2/hw/ip/HiveNet_kernel_0.xo
BINARY_CONTAINER_1_OBJS += $(XFLIB_DIR)/network/roce_v2/hw/ip/cmac_0.xo
BINARY_CONTAINER_1_OBJS += $(CUR_DIR)/src/generator_user_latency.xo

ALL_MESSAGE_FILES = $(subst .xclbin,.mdb,$(BINARY_CONTAINERS))

#
# primary build targets
#

.PHONY: all clean
all: $(BINARY_CONTAINERS)


clean:
	-$(RM) $(BINARY_CONTAINERS) $(ALL_MESSAGE_FILES) 
	-$(RM) *.xclbin.sh *.xclbin.info *.xclbin.link_summary*
	-$(RMDIR) $(BUILD_SUBDIRS)
	-$(RMDIR) .Xil

ifeq ($(MAKECMDGOALS),incremental)
.PHONY: binary_container_1.xclbin
.PHONY: ../kernels/src/HiveNet_kernel_0.xo
.PHONY: ../kernels/src/bandwidth_kernel.xo
.PHONY: ../kernels/src/cmac_0.xo
.PHONY: ../kernels/src/HiveNet_collector.xo
.PHONY: ../kernels/src/HiveNet_generator.xo
endif

.PHONY: incremental
incremental: all


nothing:

#
# binary container: binary_container_1.xclbin
#

binary_container_1.xclbin: $(BINARY_CONTAINER_1_OBJS) binary_container_1-link.cfg
	-@echo $(VPP_LINKER) $(VPP_OPTS) --link -g --config binary_container_1-link.cfg --config port_connection.txt -o"$@" $(BINARY_CONTAINER_1_OBJS) > binary_container_1.xclbin.sh
	$(VPP_LINKER) $(VPP_OPTS) --link -g --config binary_container_1-link.cfg --config port_connection.txt -o"$@" $(BINARY_CONTAINER_1_OBJS)

