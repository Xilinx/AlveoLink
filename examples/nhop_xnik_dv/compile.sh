#!/bin/bash
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

CUR_DIR=$(pwd)
graphanalyticsPath=$(awk 'sub(/graphanalytics.*/, "")' <<< "$CUR_DIR")graphanalytics
export AL_PATH=$graphanalyticsPath/../AlveoLink

echo "compile host"
make clean TARGET=hw HOST=$1
make host TARGET=hw HOST=$1
sleep 1
echo "done for $1"
