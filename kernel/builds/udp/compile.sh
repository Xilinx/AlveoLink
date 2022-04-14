#!/bin/bash

source /opt/xilinx/xrt/setup.sh

echo "compile network_config"
make clean TARGET=hw HOST=$1
make host TARGET=hw HOST=$1

sleep 1
echo "done for $1"
