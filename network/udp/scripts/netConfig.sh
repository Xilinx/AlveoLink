#!/bin/bash
source /opt/xilinx/xrt/setup.sh

echo "$2/network/udp/sw/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/netConfig.exe $3 $4 $5 $6"

$2/network/udp/sw/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/netConfig.exe $3 $4 $5 $6
sleep 1
echo "done for $1"
