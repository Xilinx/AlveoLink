#
# Copyright 2019-2020 Xilinx, Inc.
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
#


source settings.tcl
set PROJ "test.prj"
set SOLN "sol"

if {![info exists CLKP]} {
  set CLKP 3.3333
}

open_project -reset $PROJ
add_files "${XF_PROJ_ROOT}/tests/hls_modules/xnikRX_DV/uut_top.cpp" -cflags "-I ${XF_PROJ_ROOT}/kernel/hw/include -I ${XF_PROJ_ROOT}/adapter/dv/hw/include -g -O0 -std=c++11 -D HLS_SIM -D AL_mtuBytes=1472 -D AL_maxConnections=16 -D AL_netDataBits=512 -D AL_destBits=16"
add_files -tb "${XF_PROJ_ROOT}/tests/hls_modules/xnikRX_DV/test.cpp" -cflags "-I ${XF_PROJ_ROOT}/kernel/hw/include -I ${XF_PROJ_ROOT}/network/dv/sw/include -g -O0 -std=c++11 -D HLS_SIM -D AL_netDataBits=512 -D AL_maxConnections=16 -D AL_destBits=16 -D AL_numInfs=2"
set_top uut_top

open_solution -reset $SOLN

set_part $XPART
create_clock -period $CLKP

config_compile -pipeline_style frp
if {$CSIM == 1} {
  csim_design -argv "8 0 2 1"
}

if {$CSYNTH == 1} {
  csynth_design
}

if {$COSIM == 1} {
  cosim_design -trace_level all -argv "128 3 3 2"
  #cosim_design -argv "128 1 2 1"
}

if {$VIVADO_SYN == 1} {
  export_design -flow syn -rtl verilog
}

if {$VIVADO_IMPL == 1} {
  export_design -flow impl -rtl verilog
}

exit
