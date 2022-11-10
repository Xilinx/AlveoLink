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

if [ $1 == 'clean' ]
then
    cd HiveNet_bd
    rm -r */
    rm HiveNet_bd.xpr
    cd ..
    cd ip
    rm -r HiveNet
    cd ..
    rm *.log
    rm *.jou
    rm *.xml
    rm -r xgui
    rm -r .Xil
else
    vivado -mode batch -source HiveNet_bd.tcl -tclargs $1 $2
fi
