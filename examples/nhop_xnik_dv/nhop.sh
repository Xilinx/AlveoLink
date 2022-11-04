#!/bin/bash
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

CUR_DIR=$(pwd)
graphanalyticsPath=$(awk 'sub(/graphanalytics.*/, "")' <<< "$CUR_DIR")graphanalytics
export AL_PATH=$graphanalyticsPath/../AlveoLink

echo "run nhop on host $1 device $2"
echo "run cmd: "
echo "./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/nhop_dv.xclbin --devId $2 --hostId $3 --startId $4 --numIds 2 --numKernel $5 --filepath $6 --netfile ./netconfig.txt --hop $7  --batch $8"

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/nhop_dv.xclbin --devId $2 --hostId $3 --startId $4 --numIds 2 --numKernel $5 --filepath $6 --netfile ./netconfig.txt --hop $7  --batch $8
sleep 1
echo "done for host $1 device $2"
