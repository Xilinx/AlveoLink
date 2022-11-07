#!/bin/bash
source /opt/xilinx/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

echo "run tm_managers on host $1 device $2 with idleCycles $3"
echo "run cmd: "
echo "./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/manager.exe traffic_managers.xclbin $2 $3 15" 

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/manager.exe ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/traffic_managers.xclbin $2 $3 15
sleep 1
echo "done for host $1 device $2"
