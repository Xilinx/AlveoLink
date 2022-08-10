create_project HiveNet_bd ./HiveNet_bd -part xcu55c-fsvh2892-2L-e

#set_property board_part xilinx.com:au55c:part0:1.0 [current_project]
set_property part xcu55c-fsvh2892-2L-e [current_project]
add_files -norecurse {./HiveNet_bd/HiveNet_kernel.sv ./ip/bram_wrapper.v ./ip/axi_address_remap.v ./ip/ip_handler.v ./ip/axis_switch.v}
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

