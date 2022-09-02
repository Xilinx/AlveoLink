#!/bin/bash
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

echo "run tm_managers on host $1 device $2"
echo "run cmd: "
echo "./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/manager.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/traffic_managers.xclbin $2 65536 15" 

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/manager.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/traffic_managers.xclbin $2 65536 15
sleep 1
echo "done for host $1 device $2"
