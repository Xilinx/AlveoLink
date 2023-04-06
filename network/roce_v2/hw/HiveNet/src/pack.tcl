ipx::package_project -root_dir ./ -vendor grovf.com -library libhivenet -force

set_property name HiveNet_kernel_$HIVENET_ID [ipx::current_core]
set_property display_name HiveNet_kernel_${HIVENET_ID}_v1_0 [ipx::current_core]
set_property description HiveNet_kernel_${HIVENET_ID}_v1_0 [ipx::current_core]

ipx::infer_bus_interface ap_clk_320 xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface resetn_320 xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface ap_clk_400 xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface resetn_400 xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]

ipx::infer_bus_interface ap_clk xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]
ipx::infer_bus_interface ap_rst_n xilinx.com:signal:reset_rtl:1.0 [ipx::current_core]

ipx::infer_bus_interface ref_clock xilinx.com:signal:clock_rtl:1.0 [ipx::current_core]


ipx::associate_bus_interfaces -busif inputData -clock ap_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif outData -clock ap_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif rx -clock ap_clk_320 [ipx::current_core]
ipx::associate_bus_interfaces -busif tx -clock ap_clk_320 [ipx::current_core]
ipx::associate_bus_interfaces -busif S00_AXI_0 -clock ap_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif HBM_write -clock ap_clk_400 [ipx::current_core]
ipx::associate_bus_interfaces -busif HBM_read -clock ap_clk_400 [ipx::current_core]

set_property range 8192 [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]
set_property range 8192 [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]
set_property sdx_kernel true [ipx::current_core]
set_property sdx_kernel_type rtl [ipx::current_core]



#set mem_map    [::ipx::add_memory_map -quiet "s_axi_control" $core]
#set addr_block [::ipx::add_address_block -quiet "reg0" $mem_map]
set addr_block [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]

  set reg      [::ipx::add_register "localID" $addr_block]
  set_property description    "local ID"    $reg
  set_property address_offset 0x010 $reg
  set_property size           13    $reg

  set reg      [::ipx::add_register "timeout" $addr_block]
  set_property description    "Timeout"    $reg
  set_property address_offset 0x018 $reg
  set_property size           64    $reg

  set reg      [::ipx::add_register "ipSubnet" $addr_block]
  set_property description    "IP subnet"    $reg
  set_property address_offset 0x024 $reg
  set_property size           32    $reg  

  set reg      [::ipx::add_register "localMac" $addr_block]
  set_property description    "Local MAC address"    $reg
  set_property address_offset 0x02c $reg
  set_property size           48    $reg

  set reg      [::ipx::add_register "retanssmitions" $addr_block]
  set_property description    "Number of retranssmisions before error."    $reg
  set_property address_offset 0x038 $reg
  set_property size           16    $reg

  set reg      [::ipx::add_register "UDPport" $addr_block]
  set_property description    "src and dest udp ports"    $reg
  set_property address_offset 0x040 $reg
  set_property size           16    $reg

  set reg      [::ipx::add_register "arpTableReset_n" $addr_block]
  set_property description    "Active Low reset of the arp table"    $reg
  set_property address_offset 0x048 $reg
  set_property size           1     $reg

  set reg      [::ipx::add_register "readPSN" $addr_block]
  set_property description    "Read pointer of the fifo."    $reg
  set_property address_offset 0x050 $reg
  set_property size           24    $reg

  set reg      [::ipx::add_register "writePSN" $addr_block]
  set_property description    "Write pointer of the fifo."    $reg
  set_property address_offset 0x058 $reg
  set_property size           24    $reg

  set reg      [::ipx::add_register "buffSize" $addr_block]
  set_property description    "Internal buffer size."    $reg
  set_property address_offset 0x100 $reg
  set_property size           32    $reg

  set reg      [::ipx::add_register "ecnTimeout" $addr_block]
  set_property description    "Timeout for ECN in ns."    $reg
  set_property address_offset 0x180 $reg
  set_property size           32    $reg

  set reg      [::ipx::add_register "insert_vlan" $addr_block]
  set_property description    "VLAN ON/OFF."               $reg
  set_property address_offset 0x210 $reg
  set_property size           1     $reg  

  set reg      [::ipx::add_register "pfc_code" $addr_block]
  set_property description    "Priority of the packets."    $reg
  set_property address_offset 0x218 $reg
  set_property size           3     $reg  

  set reg      [::ipx::add_register "vlan_id" $addr_block]
  set_property description    "VLAN ID"               $reg
  set_property address_offset 0x220  $reg
  set_property size           12     $reg  

  ipx::add_register HBM_write [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]
  set_property address_offset 0x80 [ipx::get_registers HBM_write -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]
  set_property size 32 [ipx::get_registers HBM_write -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]
  ipx::add_register_parameter ASSOCIATED_BUSIF [ipx::get_registers HBM_write -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]
  set_property value HBM_write [ipx::get_register_parameters ASSOCIATED_BUSIF -of_objects [ipx::get_registers HBM_write -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]]

  ipx::add_register HBM_read [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]
  set_property address_offset 0x88 [ipx::get_registers HBM_read -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]
  set_property size 32 [ipx::get_registers HBM_read -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]
  ipx::add_register_parameter ASSOCIATED_BUSIF [ipx::get_registers HBM_read -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]
  set_property value HBM_read [ipx::get_register_parameters ASSOCIATED_BUSIF -of_objects [ipx::get_registers HBM_read -of_objects [ipx::get_address_blocks reg0 -of_objects [ipx::get_memory_maps S00_AXI_0 -of_objects [ipx::current_core]]]]]





ipx::add_bus_parameter NUM_WRITE_OUTSTANDING [ipx::get_bus_interfaces HBM_write -of_objects [ipx::current_core]]
set_property value 256 [ipx::get_bus_parameters NUM_WRITE_OUTSTANDING -of_objects [ipx::get_bus_interfaces HBM_write -of_objects [ipx::current_core]]]

set_property vitis_drc {ctrl_protocol ap_ctrl_hs} [ipx::current_core]
set_property vitis_drc {ctrl_protocol user_managed} [ipx::current_core]

set_property core_revision 3 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity -kernel [ipx::current_core]
ipx::save_core [ipx::current_core]
set_property ipi_drc {ignore_freq_hz true} [ipx::current_core]


package_xo  -force -xo_path ../build/HiveNet_kernel_$HIVENET_ID.xo \
 -kernel_name HiveNet_kernel_$HIVENET_ID \
 -ip_directory . \
 -ctrl_protocol user_managed