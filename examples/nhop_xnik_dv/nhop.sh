# Copyright 2019-2022 Xilinx, Inc.
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

#!/bin/bash
source /opt/xilinx/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

CUR_DIR=$(pwd)
export AL_PATH=$CUR_DIR/../../AlveoLink

echo "run nhop on host $1 device $2"
echo "run cmd: "
echo "./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/host.exe --xclbin nhop_dv.xclbin --devId $2 --hostId $3 --startId $4 --numIds 2 --numKernel $5 --filepath $6 --netfile ./netconfig.txt --hop $7  --batch $8"

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/$1/host.exe --xclbin nhop_dv.xclbin --devId $2 --hostId $3 --startId $4 --numIds 2 --numKernel $5 --filepath $6 --netfile ./netconfig.txt --hop $7  --batch $8
sleep 1
echo "done for host $1 device $2"
