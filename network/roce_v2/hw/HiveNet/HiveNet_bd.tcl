set src_dir [lindex $argv 0]

create_project HiveNet_bd $src_dir/../HiveNet_bd -part xcu55c-fsvh2892-2L-e

set_property part xcu55c-fsvh2892-2L-e [current_project]
add_files -norecurse {$src_dir/HiveNet_bd/HiveNet_kernel.sv $src_dir/ip/bram_wrapper.v $src_dir/ip/axi_address_remap.v $src_dir/ip/ip_handler.v $src_dir/ip/axis_switch.v}
import_files -force -norecurse
set_property  ip_repo_paths  $src_dir/ip [current_project]
update_ip_catalog

set HBM_BANK [lindex $argv 2]
set HIVENET_ID [lindex $argv 1]
puts $HBM_BANK
source $src_dir/HiveNet_bd_create.tcl
set_property top HiveNet_kernel [current_fileset]
update_compile_order -fileset sources_1

source $src_dir/pack.tcl

