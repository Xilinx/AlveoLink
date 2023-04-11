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

create_project HiveNet_bd ./HiveNet_bd -part xcu55c-fsvh2892-2L-e

add_files -norecurse {./HiveNet_bd/HiveNet_kernel.sv ./ip/bram_wrapper.v ./ip/axi_address_remap.v ./ip/ip_handler.v ./ip/axis_switch.v ./ip/ecn_timer.v ./ip/stream_flag_reg.v ./ip/fifo_packet_drop.v ./ip/drop_tusrer.v}
import_files -force -norecurse
set_property  ip_repo_paths  ./ip [current_project]
update_ip_catalog

set HBM_BANK [lindex $argv 1]
set HIVENET_ID [lindex $argv 0]
puts $HBM_BANK
source ./HiveNet_bd_create.tcl
set_property top HiveNet_kernel [current_fileset]
update_compile_order -fileset sources_1
source ./pack.tcl

