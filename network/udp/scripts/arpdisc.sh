#!/bin/bash
source /opt/xilinx/xrt/setup.sh

echo "$2/network/udp/sw/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/arpDisc.exe $3 $4"

$2/network/udp/sw/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/arpDisc.exe $3 $4
sleep 1
echo "done for $1"
