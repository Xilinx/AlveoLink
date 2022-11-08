#!/bin/bash
source /opt/xilinx/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

CUR_DIR=$(pwd)
export AL_PATH=$CUR_DIR/../../AlveoLink

echo "compile host"
make clean TARGET=hw HOST=$1
make host TARGET=hw HOST=$1
sleep 1
echo "done for $1"
