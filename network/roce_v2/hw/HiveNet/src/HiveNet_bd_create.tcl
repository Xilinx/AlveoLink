
################################################################
# This is a generated script based on design: HiveNet
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2022.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source HiveNet_script.tcl


# The design that will be created by this Tcl script contains the following 
# module references:
# axi_address_remap, ip_handler, axis_switch, bram_wrapper

# Please add the sources of those modules before sourcing this Tcl script.

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcu55c-fsvh2892-2L-e
   set_property BOARD_PART xilinx.com:au55c:part0:1.0 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name HiveNet

# This script was generated for a remote BD. To create a non-remote design,
# change the variable <run_remote_bd_flow> to <0>.

set run_remote_bd_flow 1
if { $run_remote_bd_flow == 1 } {
  # Set the reference directory for source file relative paths (by default 
  # the value is script directory path)
  set origin_dir ./ip

  # Use origin directory path location variable, if specified in the tcl shell
  if { [info exists ::origin_dir_loc] } {
     set origin_dir $::origin_dir_loc
  }

  set str_bd_folder [file normalize ${origin_dir}]
  set str_bd_filepath ${str_bd_folder}/${design_name}/${design_name}.bd

  # Check if remote design exists on disk
  if { [file exists $str_bd_filepath ] == 1 } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2030 -severity "ERROR" "The remote BD file path <$str_bd_filepath> already exists!"}
     common::send_gid_msg -ssname BD::TCL -id 2031 -severity "INFO" "To create a non-remote BD, change the variable <run_remote_bd_flow> to <0>."
     common::send_gid_msg -ssname BD::TCL -id 2032 -severity "INFO" "Also make sure there is no design <$design_name> existing in your current project."

     return 1
  }

  # Check if design exists in memory
  set list_existing_designs [get_bd_designs -quiet $design_name]
  if { $list_existing_designs ne "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2033 -severity "ERROR" "The design <$design_name> already exists in this project! Will not create the remote BD <$design_name> at the folder <$str_bd_folder>."}

     common::send_gid_msg -ssname BD::TCL -id 2034 -severity "INFO" "To create a non-remote BD, change the variable <run_remote_bd_flow> to <0> or please set a different value to variable <design_name>."

     return 1
  }

  # Check if design exists on disk within project
  set list_existing_designs [get_files -quiet */${design_name}.bd]
  if { $list_existing_designs ne "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2035 -severity "ERROR" "The design <$design_name> already exists in this project at location:
    $list_existing_designs"}
     catch {common::send_gid_msg -ssname BD::TCL -id 2036 -severity "ERROR" "Will not create the remote BD <$design_name> at the folder <$str_bd_folder>."}

     common::send_gid_msg -ssname BD::TCL -id 2037 -severity "INFO" "To create a non-remote BD, change the variable <run_remote_bd_flow> to <0> or please set a different value to variable <design_name>."

     return 1
  }

  # Now can create the remote BD
  # NOTE - usage of <-dir> will create <$str_bd_folder/$design_name/$design_name.bd>
  create_bd_design -dir $str_bd_folder $design_name
} else {

  # Create regular design
  if { [catch {create_bd_design $design_name} errmsg] } {
     common::send_gid_msg -ssname BD::TCL -id 2038 -severity "INFO" "Please set a different value to variable <design_name>."

     return 1
  }
}

current_bd_design $design_name

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:axi_register_slice:2.1\
xilinx.com:ip:axis_clock_converter:1.1\
xilinx.com:ip:axis_register_slice:1.1\
xilinx.com:ip:clk_wiz:6.0\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:xlconstant:1.1\
xilinx.com:ip:xpm_cdc_gen:1.0\
xilinx.com:ip:axi_gpio:2.0\
ethz.systems.fpga:hls:arp_server_subnet:1.1\
xilinx.com:ip:axis_data_fifo:2.0\
xilinx.com:ip:axis_dwidth_converter:1.1\
xilinx.com:hls:cmac_tkeep_fix:1.0\
xilinx.com:hls:cut_packet:1.0\
xilinx.com:hls:eth_stream_switch_dev:1.0\
xilinx.labs:hls:icmp_server:1.67\
xilinx.com:hls:insert_ethernet_header:1.0\
xilinx.com:hls:register_map_full:1.0\
xilinx.com:hls:asynchronousWriteToHBM:1.0\
xilinx.com:hls:axi_broadcast:1.0\
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:c_counter_binary:12.0\
xilinx.com:ip:c_shift_ram:12.0\
xilinx.com:hls:inputSizeInBytes:1.0\
xilinx.com:hls:rx_qp:1.0\
xilinx.com:hls:tx_checkCurrentPacket_dev:1.0\
xilinx.com:hls:tx_checkResponce_dev:1.0\
xilinx.com:hls:tx_checkTimeout_dev:1.0\
xilinx.com:hls:tx_checkcurrent_data:1.0\
xilinx.com:hls:tx_dev_mem_management:1.0\
xilinx.com:hls:tx_syncroniceWritesAndInterconnect_dev:1.0\
xilinx.com:hls:tx_write_dev:1.0\
xilinx.com:ip:axi_clock_converter:2.1\
xilinx.com:ip:axi_crossbar:2.1\
xilinx.com:ip:xlconcat:2.1\
xilinx.com:ip:xlslice:1.0\
xilinx.com:ip:c_addsub:12.0\
xilinx.com:ip:util_vector_logic:2.0\
xilinx.com:hls:rx_rdma_hdr:1.0\
xilinx.com:hls:tx_rdma_hdr:1.0\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

##################################################################
# CHECK Modules
##################################################################
set bCheckModules 1
if { $bCheckModules == 1 } {
   set list_check_mods "\ 
axi_address_remap\
ip_handler\
axis_switch\
bram_wrapper\
"

   set list_mods_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2020 -severity "INFO" "Checking if the following modules exist in the project's sources: $list_check_mods ."

   foreach mod_vlnv $list_check_mods {
      if { [can_resolve_reference $mod_vlnv] == 0 } {
         lappend list_mods_missing $mod_vlnv
      }
   }

   if { $list_mods_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2021 -severity "ERROR" "The following module(s) are not found in the project: $list_mods_missing" }
      common::send_gid_msg -ssname BD::TCL -id 2022 -severity "INFO" "Please add source files for the missing module(s) above."
      set bCheckIPsPassed 0
   }
}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################


# Hierarchical cell: tx_rdma_hdr
proc create_hier_cell_tx_rdma_hdr { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_tx_rdma_hdr() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 axis_frame_ctx_rx_V

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 axis_frame_ctx_tx_V

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 tx_in

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 tx_out


  # Create pins
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -type rst ap_rst_n
  create_bd_pin -dir I -type clk m_axis_aclk
  create_bd_pin -dir I -type clk m_axis_aclk1
  create_bd_pin -dir I -type rst m_axis_aresetn
  create_bd_pin -dir I -from 31 -to 0 -type data my_ip
  create_bd_pin -dir I -type rst peripheral_aresetn
  create_bd_pin -dir I -from 15 -to 0 -type data udp_port

  # Create instance: axis_clock_converter_0, and set properties
  set axis_clock_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_0 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {4} \
   CONFIG.TDATA_NUM_BYTES {4} \
 ] $axis_clock_converter_0

  # Create instance: axis_clock_converter_1, and set properties
  set axis_clock_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_1 ]

  # Create instance: axis_clock_converter_2, and set properties
  set axis_clock_converter_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_2 ]

  # Create instance: axis_clock_converter_3, and set properties
  set axis_clock_converter_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_3 ]

  # Create instance: axis_clock_converter_4, and set properties
  set axis_clock_converter_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_4 ]

  # Create instance: axis_clock_converter_5, and set properties
  set axis_clock_converter_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_5 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {4} \
   CONFIG.TDATA_NUM_BYTES {2} \
 ] $axis_clock_converter_5

  # Create instance: tx_rdma_hdr_0, and set properties
  set tx_rdma_hdr_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_rdma_hdr:1.0 tx_rdma_hdr_0 ]

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create interface connections
  connect_bd_intf_net -intf_net axis_clock_converter_1_M_AXIS [get_bd_intf_pins axis_clock_converter_1/M_AXIS] [get_bd_intf_pins tx_rdma_hdr_0/tx_in]
  connect_bd_intf_net -intf_net axis_clock_converter_2_M_AXIS [get_bd_intf_pins axis_clock_converter_2/M_AXIS] [get_bd_intf_pins tx_rdma_hdr_0/axis_frame_ctx_tx_V]
  connect_bd_intf_net -intf_net axis_clock_converter_3_M_AXIS [get_bd_intf_pins axis_clock_converter_3/M_AXIS] [get_bd_intf_pins tx_rdma_hdr_0/axis_frame_ctx_rx_V]
  connect_bd_intf_net -intf_net axis_clock_converter_4_M_AXIS [get_bd_intf_pins tx_out] [get_bd_intf_pins axis_clock_converter_4/M_AXIS]
  connect_bd_intf_net -intf_net axis_frame_ctx_rx_V_1 [get_bd_intf_pins axis_frame_ctx_rx_V] [get_bd_intf_pins axis_clock_converter_3/S_AXIS]
  connect_bd_intf_net -intf_net axis_frame_ctx_tx_V_1 [get_bd_intf_pins axis_frame_ctx_tx_V] [get_bd_intf_pins axis_clock_converter_2/S_AXIS]
  connect_bd_intf_net -intf_net tx_in_1 [get_bd_intf_pins tx_in] [get_bd_intf_pins axis_clock_converter_1/S_AXIS]
  connect_bd_intf_net -intf_net tx_rdma_hdr_0_tx_out [get_bd_intf_pins axis_clock_converter_4/S_AXIS] [get_bd_intf_pins tx_rdma_hdr_0/tx_out]

  # Create port connections
  connect_bd_net -net ap_clk_1 [get_bd_pins m_axis_aclk] [get_bd_pins axis_clock_converter_0/m_axis_aclk] [get_bd_pins axis_clock_converter_1/m_axis_aclk] [get_bd_pins axis_clock_converter_2/m_axis_aclk] [get_bd_pins axis_clock_converter_3/m_axis_aclk] [get_bd_pins axis_clock_converter_4/s_axis_aclk] [get_bd_pins axis_clock_converter_5/m_axis_aclk] [get_bd_pins tx_rdma_hdr_0/ap_clk]
  connect_bd_net -net ap_clk_2 [get_bd_pins ap_clk] [get_bd_pins axis_clock_converter_0/s_axis_aclk] [get_bd_pins axis_clock_converter_1/s_axis_aclk] [get_bd_pins axis_clock_converter_2/s_axis_aclk] [get_bd_pins axis_clock_converter_3/s_axis_aclk] [get_bd_pins axis_clock_converter_5/s_axis_aclk]
  connect_bd_net -net ap_rst_n_1 [get_bd_pins ap_rst_n] [get_bd_pins axis_clock_converter_0/s_axis_aresetn] [get_bd_pins axis_clock_converter_1/s_axis_aresetn] [get_bd_pins axis_clock_converter_2/s_axis_aresetn] [get_bd_pins axis_clock_converter_3/s_axis_aresetn] [get_bd_pins axis_clock_converter_5/s_axis_aresetn]
  connect_bd_net -net axis_clock_converter_0_m_axis_tdata [get_bd_pins axis_clock_converter_0/m_axis_tdata] [get_bd_pins tx_rdma_hdr_0/my_ip]
  connect_bd_net -net axis_clock_converter_5_m_axis_tdata [get_bd_pins axis_clock_converter_5/m_axis_tdata] [get_bd_pins tx_rdma_hdr_0/rdma_udp_port_dst]
  connect_bd_net -net m_axis_aclk1_1 [get_bd_pins m_axis_aclk1] [get_bd_pins axis_clock_converter_4/m_axis_aclk]
  connect_bd_net -net my_ip_1 [get_bd_pins my_ip] [get_bd_pins axis_clock_converter_0/s_axis_tdata]
  connect_bd_net -net peripheral_aresetn_1 [get_bd_pins peripheral_aresetn] [get_bd_pins axis_clock_converter_4/m_axis_aresetn]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins m_axis_aresetn] [get_bd_pins axis_clock_converter_0/m_axis_aresetn] [get_bd_pins axis_clock_converter_1/m_axis_aresetn] [get_bd_pins axis_clock_converter_2/m_axis_aresetn] [get_bd_pins axis_clock_converter_3/m_axis_aresetn] [get_bd_pins axis_clock_converter_4/s_axis_aresetn] [get_bd_pins axis_clock_converter_5/m_axis_aresetn] [get_bd_pins tx_rdma_hdr_0/ap_rst_n]
  connect_bd_net -net udp_port_1 [get_bd_pins udp_port] [get_bd_pins axis_clock_converter_5/s_axis_tdata]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins axis_clock_converter_0/m_axis_tready] [get_bd_pins axis_clock_converter_0/s_axis_tvalid] [get_bd_pins axis_clock_converter_5/m_axis_tready] [get_bd_pins axis_clock_converter_5/s_axis_tvalid] [get_bd_pins xlconstant_0/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: rx_rdma_hdr_hier
proc create_hier_cell_rx_rdma_hdr_hier { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_rx_rdma_hdr_hier() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 rx_i

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 rx_req_o_V

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 rx_req_payload_o

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 tx_resp_o_V


  # Create pins
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -type rst ap_rst_n
  create_bd_pin -dir I -type rst ap_rst_n_0
  create_bd_pin -dir I -type clk clk_out3
  create_bd_pin -dir I -type clk gt_txusrclk2
  create_bd_pin -dir I -type rst peripheral_aresetn
  create_bd_pin -dir I tx_resp_payload_o_TREADY

  # Create instance: axis_clock_converter_0, and set properties
  set axis_clock_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_0 ]

  # Create instance: axis_clock_converter_1, and set properties
  set axis_clock_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_1 ]

  # Create instance: axis_clock_converter_2, and set properties
  set axis_clock_converter_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_2 ]

  # Create instance: axis_clock_converter_3, and set properties
  set axis_clock_converter_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_3 ]

  # Create instance: rx_rdma_hdr_0, and set properties
  set rx_rdma_hdr_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:rx_rdma_hdr:1.0 rx_rdma_hdr_0 ]

  # Create interface connections
  connect_bd_intf_net -intf_net axis_clock_converter_0_M_AXIS [get_bd_intf_pins axis_clock_converter_0/M_AXIS] [get_bd_intf_pins rx_rdma_hdr_0/rx_i]
  connect_bd_intf_net -intf_net axis_clock_converter_1_M_AXIS [get_bd_intf_pins rx_req_o_V] [get_bd_intf_pins axis_clock_converter_1/M_AXIS]
  connect_bd_intf_net -intf_net axis_clock_converter_2_M_AXIS [get_bd_intf_pins rx_req_payload_o] [get_bd_intf_pins axis_clock_converter_2/M_AXIS]
  connect_bd_intf_net -intf_net axis_clock_converter_3_M_AXIS [get_bd_intf_pins tx_resp_o_V] [get_bd_intf_pins axis_clock_converter_3/M_AXIS]
  connect_bd_intf_net -intf_net rx_i_1 [get_bd_intf_pins rx_i] [get_bd_intf_pins axis_clock_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net rx_rdma_hdr_0_rx_req_o_V [get_bd_intf_pins axis_clock_converter_1/S_AXIS] [get_bd_intf_pins rx_rdma_hdr_0/rx_req_o_V]
  connect_bd_intf_net -intf_net rx_rdma_hdr_0_rx_req_payload_o [get_bd_intf_pins axis_clock_converter_2/S_AXIS] [get_bd_intf_pins rx_rdma_hdr_0/rx_req_payload_o]
  connect_bd_intf_net -intf_net rx_rdma_hdr_0_tx_resp_o_V [get_bd_intf_pins axis_clock_converter_3/S_AXIS] [get_bd_intf_pins rx_rdma_hdr_0/tx_resp_o_V]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins clk_out3] [get_bd_pins axis_clock_converter_0/m_axis_aclk] [get_bd_pins axis_clock_converter_1/s_axis_aclk] [get_bd_pins axis_clock_converter_2/s_axis_aclk] [get_bd_pins axis_clock_converter_3/s_axis_aclk] [get_bd_pins rx_rdma_hdr_0/ap_clk]
  connect_bd_net -net Net2 [get_bd_pins ap_rst_n] [get_bd_pins axis_clock_converter_1/m_axis_aresetn] [get_bd_pins axis_clock_converter_2/m_axis_aresetn] [get_bd_pins axis_clock_converter_3/m_axis_aresetn]
  connect_bd_net -net Net3 [get_bd_pins ap_clk] [get_bd_pins axis_clock_converter_1/m_axis_aclk] [get_bd_pins axis_clock_converter_2/m_axis_aclk] [get_bd_pins axis_clock_converter_3/m_axis_aclk]
  connect_bd_net -net ap_rst_n_0_1 [get_bd_pins ap_rst_n_0] [get_bd_pins axis_clock_converter_0/m_axis_aresetn] [get_bd_pins axis_clock_converter_1/s_axis_aresetn] [get_bd_pins axis_clock_converter_2/s_axis_aresetn] [get_bd_pins axis_clock_converter_3/s_axis_aresetn] [get_bd_pins rx_rdma_hdr_0/ap_rst_n]
  connect_bd_net -net gt_txusrclk2_1 [get_bd_pins gt_txusrclk2] [get_bd_pins axis_clock_converter_0/s_axis_aclk]
  connect_bd_net -net peripheral_aresetn_1 [get_bd_pins peripheral_aresetn] [get_bd_pins axis_clock_converter_0/s_axis_aresetn]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins tx_resp_payload_o_TREADY] [get_bd_pins rx_rdma_hdr_0/tx_resp_payload_o_TREADY]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: register_writes
proc create_hier_cell_register_writes { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_register_writes() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir I -from 0 -to 0 -type data D
  create_bd_pin -dir I -from 7 -to 0 -type data D1
  create_bd_pin -dir I -from 0 -to 0 -type data D2
  create_bd_pin -dir I -from 19 -to 0 -type data D3
  create_bd_pin -dir O -from 0 -to 0 -type data Q
  create_bd_pin -dir O -from 1 -to 0 -type data Q1
  create_bd_pin -dir O -from 0 -to 0 -type data Q2
  create_bd_pin -dir O -from 19 -to 0 -type data Q3
  create_bd_pin -dir I -type clk ap_clk

  # Create instance: c_shift_ram_1, and set properties
  set c_shift_ram_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_1 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {0} \
   CONFIG.DefaultData {0} \
   CONFIG.Depth {1} \
   CONFIG.SyncInitVal {0} \
   CONFIG.Width {1} \
 ] $c_shift_ram_1

  # Create instance: c_shift_ram_2, and set properties
  set c_shift_ram_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_2 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {0} \
   CONFIG.DefaultData {0} \
   CONFIG.Depth {1} \
   CONFIG.SyncInitVal {0} \
   CONFIG.Width {1} \
 ] $c_shift_ram_2

  # Create instance: c_shift_ram_3, and set properties
  set c_shift_ram_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_3 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {00} \
   CONFIG.DefaultData {00} \
   CONFIG.Depth {1} \
   CONFIG.SyncInitVal {00} \
   CONFIG.Width {2} \
 ] $c_shift_ram_3

  # Create instance: c_shift_ram_4, and set properties
  set c_shift_ram_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_4 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {00000000000000000000} \
   CONFIG.DefaultData {00000000000000000000} \
   CONFIG.Depth {1} \
   CONFIG.SyncInitVal {00000000000000000000} \
   CONFIG.Width {20} \
 ] $c_shift_ram_4

  # Create port connections
  connect_bd_net -net ap_clk_0_1 [get_bd_pins ap_clk] [get_bd_pins c_shift_ram_1/CLK] [get_bd_pins c_shift_ram_2/CLK] [get_bd_pins c_shift_ram_3/CLK] [get_bd_pins c_shift_ram_4/CLK]
  connect_bd_net -net c_shift_ram_1_Q [get_bd_pins Q2] [get_bd_pins c_shift_ram_1/Q]
  connect_bd_net -net c_shift_ram_2_Q [get_bd_pins Q] [get_bd_pins c_shift_ram_2/Q]
  connect_bd_net -net c_shift_ram_3_Q [get_bd_pins Q1] [get_bd_pins c_shift_ram_3/Q]
  connect_bd_net -net c_shift_ram_4_Q [get_bd_pins Q3] [get_bd_pins c_shift_ram_4/Q]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_address1 [get_bd_pins D3] [get_bd_pins c_shift_ram_4/D]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_ce1 [get_bd_pins D] [get_bd_pins c_shift_ram_2/D]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_d1 [get_bd_pins D1] [get_bd_pins c_shift_ram_3/D]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_we1 [get_bd_pins D2] [get_bd_pins c_shift_ram_1/D]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: bram_wrapper
proc create_hier_cell_bram_wrapper { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_bram_wrapper() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir I -from 19 -to 0 -type data D
  create_bd_pin -dir I -from 19 -to 0 -type data D1
  create_bd_pin -dir I -from 0 -to 0 Op1
  create_bd_pin -dir I -from 0 -to 0 Op2
  create_bd_pin -dir O -from 0 -to 0 Res
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir O -from 1 -to 0 outToRead
  create_bd_pin -dir I -from 1 -to 0 read_data
  create_bd_pin -dir I -from 1 -to 0 write_data

  # Create instance: bram_wrapper_0, and set properties
  set block_name bram_wrapper
  set block_cell_name bram_wrapper_0
  if { [catch {set bram_wrapper_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $bram_wrapper_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: c_shift_ram_1, and set properties
  set c_shift_ram_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_1 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {00000000000000000000} \
   CONFIG.DefaultData {00000000000000000000} \
   CONFIG.Depth {2} \
   CONFIG.SyncInitVal {00000000000000000000} \
   CONFIG.Width {20} \
 ] $c_shift_ram_1

  # Create instance: c_shift_ram_2, and set properties
  set c_shift_ram_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_2 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {00000000000000000000} \
   CONFIG.DefaultData {00000000000000000000} \
   CONFIG.Depth {2} \
   CONFIG.SyncInitVal {00000000000000000000} \
   CONFIG.Width {20} \
 ] $c_shift_ram_2

  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0 ]
  set_property -dict [ list \
   CONFIG.C_OPERATION {or} \
   CONFIG.C_SIZE {1} \
   CONFIG.LOGO_FILE {data/sym_orgate.png} \
 ] $util_vector_logic_0

  # Create port connections
  connect_bd_net -net D1_1 [get_bd_pins D1] [get_bd_pins c_shift_ram_1/D]
  connect_bd_net -net ap_clk_0_1 [get_bd_pins ap_clk] [get_bd_pins c_shift_ram_1/CLK] [get_bd_pins c_shift_ram_2/CLK]
  connect_bd_net -net blk_mem_gen_0_douta [get_bd_pins write_data] [get_bd_pins bram_wrapper_0/write_data]
  connect_bd_net -net blk_mem_gen_0_doutb [get_bd_pins outToRead] [get_bd_pins bram_wrapper_0/outToRead]
  connect_bd_net -net blk_mem_gen_0_doutb1 [get_bd_pins read_data] [get_bd_pins bram_wrapper_0/read_data]
  connect_bd_net -net c_shift_ram_1_Q [get_bd_pins bram_wrapper_0/write_address] [get_bd_pins c_shift_ram_1/Q]
  connect_bd_net -net c_shift_ram_2_Q [get_bd_pins Op1] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net c_shift_ram_2_Q1 [get_bd_pins bram_wrapper_0/read_address] [get_bd_pins c_shift_ram_2/Q]
  connect_bd_net -net tx_checkCurrentPacke_0_bitMap_address0 [get_bd_pins D] [get_bd_pins c_shift_ram_2/D]
  connect_bd_net -net tx_checkCurrentPacke_0_bitMap_ce0 [get_bd_pins Op2] [get_bd_pins util_vector_logic_0/Op2]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins Res] [get_bd_pins util_vector_logic_0/Res]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: Endianness
proc create_hier_cell_Endianness { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_Endianness() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir O -from 31 -to 0 -type data S
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -from 31 -to 0 -type data ip_subnet
  create_bd_pin -dir I -from 12 -to 0 -type data local_ID

  # Create instance: c_addsub_0, and set properties
  set c_addsub_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_addsub:12.0 c_addsub_0 ]
  set_property -dict [ list \
   CONFIG.A_Width {32} \
   CONFIG.B_Value {0000000000000} \
   CONFIG.B_Width {13} \
   CONFIG.CE {false} \
   CONFIG.Latency {4} \
   CONFIG.Out_Width {32} \
 ] $c_addsub_0

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]
  set_property -dict [ list \
   CONFIG.IN0_WIDTH {8} \
   CONFIG.IN1_WIDTH {8} \
   CONFIG.IN2_WIDTH {8} \
   CONFIG.IN3_WIDTH {8} \
   CONFIG.NUM_PORTS {4} \
 ] $xlconcat_0

  # Create instance: xlconcat_1, and set properties
  set xlconcat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_1 ]
  set_property -dict [ list \
   CONFIG.IN0_WIDTH {8} \
   CONFIG.IN1_WIDTH {8} \
   CONFIG.IN2_WIDTH {8} \
   CONFIG.IN3_WIDTH {8} \
   CONFIG.NUM_PORTS {4} \
 ] $xlconcat_1

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_0

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {15} \
   CONFIG.DIN_TO {8} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_1

  # Create instance: xlslice_2, and set properties
  set xlslice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_2 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {23} \
   CONFIG.DIN_TO {16} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_2

  # Create instance: xlslice_3, and set properties
  set xlslice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_3 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {31} \
   CONFIG.DIN_TO {24} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_3

  # Create instance: xlslice_4, and set properties
  set xlslice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_4 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_4

  # Create instance: xlslice_5, and set properties
  set xlslice_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_5 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {15} \
   CONFIG.DIN_TO {8} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_5

  # Create instance: xlslice_6, and set properties
  set xlslice_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_6 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {23} \
   CONFIG.DIN_TO {16} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_6

  # Create instance: xlslice_7, and set properties
  set xlslice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_7 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {31} \
   CONFIG.DIN_TO {24} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_7

  # Create port connections
  connect_bd_net -net ap_clk_0_1 [get_bd_pins ap_clk] [get_bd_pins c_addsub_0/CLK]
  connect_bd_net -net c_addsub_0_S [get_bd_pins c_addsub_0/S] [get_bd_pins xlslice_4/Din] [get_bd_pins xlslice_5/Din] [get_bd_pins xlslice_6/Din] [get_bd_pins xlslice_7/Din]
  connect_bd_net -net ip_subnet_0_1 [get_bd_pins ip_subnet] [get_bd_pins xlslice_0/Din] [get_bd_pins xlslice_1/Din] [get_bd_pins xlslice_2/Din] [get_bd_pins xlslice_3/Din]
  connect_bd_net -net local_qp_0_1 [get_bd_pins local_ID] [get_bd_pins c_addsub_0/B]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins c_addsub_0/A] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net xlconcat_1_dout [get_bd_pins S] [get_bd_pins xlconcat_1/dout]
  connect_bd_net -net xlslice_0_Dout [get_bd_pins xlconcat_0/In3] [get_bd_pins xlslice_0/Dout]
  connect_bd_net -net xlslice_1_Dout [get_bd_pins xlconcat_0/In2] [get_bd_pins xlslice_1/Dout]
  connect_bd_net -net xlslice_2_Dout [get_bd_pins xlconcat_0/In1] [get_bd_pins xlslice_2/Dout]
  connect_bd_net -net xlslice_3_Dout [get_bd_pins xlconcat_0/In0] [get_bd_pins xlslice_3/Dout]
  connect_bd_net -net xlslice_4_Dout [get_bd_pins xlconcat_1/In3] [get_bd_pins xlslice_4/Dout]
  connect_bd_net -net xlslice_5_Dout [get_bd_pins xlconcat_1/In2] [get_bd_pins xlslice_5/Dout]
  connect_bd_net -net xlslice_6_Dout [get_bd_pins xlconcat_1/In1] [get_bd_pins xlslice_6/Dout]
  connect_bd_net -net xlslice_7_Dout [get_bd_pins xlconcat_1/In0] [get_bd_pins xlslice_7/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: Mac
proc create_hier_cell_Mac { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_Mac() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins

  # Create pins
  create_bd_pin -dir O -from 47 -to 0 dout
  create_bd_pin -dir I -from 12 -to 0 -type data local_ID
  create_bd_pin -dir I -from 47 -to 0 myMacAddress_V_0

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]
  set_property -dict [ list \
   CONFIG.IN0_WIDTH {8} \
   CONFIG.IN1_WIDTH {8} \
   CONFIG.IN2_WIDTH {8} \
   CONFIG.IN3_WIDTH {8} \
   CONFIG.IN4_WIDTH {8} \
   CONFIG.IN5_WIDTH {8} \
   CONFIG.NUM_PORTS {6} \
 ] $xlconcat_0

  # Create instance: xlconcat_2, and set properties
  set xlconcat_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_2 ]
  set_property -dict [ list \
   CONFIG.IN0_WIDTH {13} \
   CONFIG.IN1_WIDTH {35} \
   CONFIG.IN2_WIDTH {8} \
   CONFIG.IN3_WIDTH {8} \
   CONFIG.IN4_WIDTH {8} \
   CONFIG.IN5_WIDTH {8} \
   CONFIG.NUM_PORTS {2} \
 ] $xlconcat_2

  # Create instance: xlslice_10, and set properties
  set xlslice_10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_10 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {47} \
   CONFIG.DIN_TO {13} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {35} \
 ] $xlslice_10

  # Create instance: xlslice_11, and set properties
  set xlslice_11 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_11 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_11

  # Create instance: xlslice_12, and set properties
  set xlslice_12 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_12 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {15} \
   CONFIG.DIN_TO {8} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_12

  # Create instance: xlslice_13, and set properties
  set xlslice_13 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_13 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {23} \
   CONFIG.DIN_TO {16} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_13

  # Create instance: xlslice_14, and set properties
  set xlslice_14 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_14 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {31} \
   CONFIG.DIN_TO {24} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_14

  # Create instance: xlslice_15, and set properties
  set xlslice_15 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_15 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {39} \
   CONFIG.DIN_TO {32} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_15

  # Create instance: xlslice_16, and set properties
  set xlslice_16 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_16 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {47} \
   CONFIG.DIN_TO {40} \
   CONFIG.DIN_WIDTH {48} \
   CONFIG.DOUT_WIDTH {8} \
 ] $xlslice_16

  # Create port connections
  connect_bd_net -net local_ID_1 [get_bd_pins local_ID] [get_bd_pins xlconcat_2/In0]
  connect_bd_net -net myMacAddress_V_0_1 [get_bd_pins myMacAddress_V_0] [get_bd_pins xlslice_10/Din]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins dout] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net xlconcat_2_dout [get_bd_pins xlconcat_2/dout] [get_bd_pins xlslice_11/Din] [get_bd_pins xlslice_12/Din] [get_bd_pins xlslice_13/Din] [get_bd_pins xlslice_14/Din] [get_bd_pins xlslice_15/Din] [get_bd_pins xlslice_16/Din]
  connect_bd_net -net xlslice_10_Dout [get_bd_pins xlconcat_2/In1] [get_bd_pins xlslice_10/Dout]
  connect_bd_net -net xlslice_11_Dout [get_bd_pins xlconcat_0/In5] [get_bd_pins xlslice_11/Dout]
  connect_bd_net -net xlslice_12_Dout [get_bd_pins xlconcat_0/In4] [get_bd_pins xlslice_12/Dout]
  connect_bd_net -net xlslice_13_Dout [get_bd_pins xlconcat_0/In3] [get_bd_pins xlslice_13/Dout]
  connect_bd_net -net xlslice_14_Dout [get_bd_pins xlconcat_0/In2] [get_bd_pins xlslice_14/Dout]
  connect_bd_net -net xlslice_15_Dout [get_bd_pins xlconcat_0/In1] [get_bd_pins xlslice_15/Dout]
  connect_bd_net -net xlslice_16_Dout [get_bd_pins xlconcat_0/In0] [get_bd_pins xlslice_16/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: inter
proc create_hier_cell_inter { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_inter() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI2


  # Create pins
  create_bd_pin -dir I -type rst aresetn
  create_bd_pin -dir I -type clk m_axis_aclk1
  create_bd_pin -dir I -type clk s_axis_aclk_0
  create_bd_pin -dir I -type rst s_axis_aresetn_0

  # Create instance: axi_clock_converter_2, and set properties
  set axi_clock_converter_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_clock_converter:2.1 axi_clock_converter_2 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.READ_WRITE_MODE {READ_ONLY} \
   CONFIG.SYNCHRONIZATION_STAGES {5} \
 ] $axi_clock_converter_2

  # Create instance: axi_clock_converter_4, and set properties
  set axi_clock_converter_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_clock_converter:2.1 axi_clock_converter_4 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.READ_WRITE_MODE {READ_ONLY} \
   CONFIG.SYNCHRONIZATION_STAGES {5} \
 ] $axi_clock_converter_4

  # Create instance: axi_crossbar_0, and set properties
  set axi_crossbar_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_crossbar:2.1 axi_crossbar_0 ]
  set_property -dict [ list \
   CONFIG.M00_S00_WRITE_CONNECTIVITY {0} \
   CONFIG.M00_S01_WRITE_CONNECTIVITY {0} \
   CONFIG.NUM_MI {1} \
   CONFIG.NUM_SI {2} \
   CONFIG.S01_BASE_ID {0x00000001} \
   CONFIG.S02_BASE_ID {0x00000002} \
   CONFIG.S03_BASE_ID {0x00000003} \
   CONFIG.S04_BASE_ID {0x00000004} \
   CONFIG.S05_BASE_ID {0x00000005} \
   CONFIG.S06_BASE_ID {0x00000006} \
   CONFIG.S07_BASE_ID {0x00000007} \
   CONFIG.S08_BASE_ID {0x00000008} \
   CONFIG.S09_BASE_ID {0x00000009} \
   CONFIG.S10_BASE_ID {0x0000000a} \
   CONFIG.S11_BASE_ID {0x0000000b} \
   CONFIG.S12_BASE_ID {0x0000000c} \
   CONFIG.S13_BASE_ID {0x0000000d} \
   CONFIG.S14_BASE_ID {0x0000000e} \
   CONFIG.S15_BASE_ID {0x0000000f} \
   CONFIG.STRATEGY {2} \
 ] $axi_crossbar_0

  # Create instance: axi_register_slice_0, and set properties
  set axi_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_0 ]
  set_property -dict [ list \
   CONFIG.ID_WIDTH {2} \
   CONFIG.PROTOCOL {AXI4} \
   CONFIG.READ_WRITE_MODE {READ_ONLY} \
   CONFIG.REG_AR {7} \
   CONFIG.REG_AW {7} \
   CONFIG.REG_B {7} \
 ] $axi_register_slice_0

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins M_AXI_0] [get_bd_intf_pins axi_register_slice_0/M_AXI]
  connect_bd_intf_net -intf_net S_AXI1_1 [get_bd_intf_pins S_AXI1] [get_bd_intf_pins axi_clock_converter_4/S_AXI]
  connect_bd_intf_net -intf_net S_AXI2_1 [get_bd_intf_pins S_AXI2] [get_bd_intf_pins axi_clock_converter_2/S_AXI]
  connect_bd_intf_net -intf_net axi_clock_converter_2_M_AXI [get_bd_intf_pins axi_clock_converter_2/M_AXI] [get_bd_intf_pins axi_crossbar_0/S00_AXI]
  connect_bd_intf_net -intf_net axi_clock_converter_4_M_AXI [get_bd_intf_pins axi_clock_converter_4/M_AXI] [get_bd_intf_pins axi_crossbar_0/S01_AXI]
  connect_bd_intf_net -intf_net axi_crossbar_0_M00_AXI [get_bd_intf_pins axi_crossbar_0/M00_AXI] [get_bd_intf_pins axi_register_slice_0/S_AXI]

  # Create port connections
  connect_bd_net -net aresetn_1 [get_bd_pins aresetn] [get_bd_pins axi_clock_converter_2/m_axi_aresetn] [get_bd_pins axi_clock_converter_4/m_axi_aresetn] [get_bd_pins axi_crossbar_0/aresetn] [get_bd_pins axi_register_slice_0/aresetn]
  connect_bd_net -net m_axis_aclk1_1 [get_bd_pins m_axis_aclk1] [get_bd_pins axi_clock_converter_2/m_axi_aclk] [get_bd_pins axi_clock_converter_4/m_axi_aclk] [get_bd_pins axi_crossbar_0/aclk] [get_bd_pins axi_register_slice_0/aclk]
  connect_bd_net -net s_axis_aclk_0_1 [get_bd_pins s_axis_aclk_0] [get_bd_pins axi_clock_converter_2/s_axi_aclk] [get_bd_pins axi_clock_converter_4/s_axi_aclk]
  connect_bd_net -net s_axis_aresetn_0_1 [get_bd_pins s_axis_aresetn_0] [get_bd_pins axi_clock_converter_2/s_axi_aresetn] [get_bd_pins axi_clock_converter_4/s_axi_aresetn]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: hier_0
proc create_hier_cell_hier_0 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_hier_0() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M_AXIS_0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 inputData

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_HBM

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_HBM_a_0

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_header

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 rx_i

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 tx_out


  # Create pins
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -type rst ap_rst
  create_bd_pin -dir I -type rst ap_rst_n_0
  create_bd_pin -dir I -from 31 -to 0 -type data ip_subnet
  create_bd_pin -dir I -from 12 -to 0 -type data local_ID
  create_bd_pin -dir I -type clk m_axis_aclk
  create_bd_pin -dir I -type clk m_axis_aclk1
  create_bd_pin -dir O -from 23 -to 0 m_axis_tdata
  create_bd_pin -dir O -from 31 -to 0 -type data my_ip
  create_bd_pin -dir I -from 15 -to 0 -type data nRetransmissions
  create_bd_pin -dir O -from 0 -to 0 -type rst peripheral_aresetn
  create_bd_pin -dir O -from 23 -to 0 readPSN_l_V_TDATA
  create_bd_pin -dir I -type rst resetn_300
  create_bd_pin -dir O -from 31 -to 0 size_o_V_TDATA_0
  create_bd_pin -dir I -from 63 -to 0 -type data timeoutInNs
  create_bd_pin -dir I -from 15 -to 0 -type data udp_port

  # Create instance: Endianness
  create_hier_cell_Endianness $hier_obj Endianness

  # Create instance: asynchronousWriteToH_0, and set properties
  set asynchronousWriteToH_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:asynchronousWriteToHBM:1.0 asynchronousWriteToH_0 ]
  set_property -dict [ list \
   CONFIG.C_M_AXI_HBM_ENABLE_ID_PORTS {false} \
   CONFIG.C_M_AXI_HBM_ID_WIDTH {1} \
 ] $asynchronousWriteToH_0

  # Create instance: axi_broadcast_readPSN, and set properties
  set axi_broadcast_readPSN [ create_bd_cell -type ip -vlnv xilinx.com:hls:axi_broadcast:1.0 axi_broadcast_readPSN ]

  # Create instance: axi_broadcast_writePSN, and set properties
  set axi_broadcast_writePSN [ create_bd_cell -type ip -vlnv xilinx.com:hls:axi_broadcast:1.0 axi_broadcast_writePSN ]

  # Create instance: axis_clock_converter_0, and set properties
  set axis_clock_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_0 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {5} \
 ] $axis_clock_converter_0

  # Create instance: axis_clock_converter_1, and set properties
  set axis_clock_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_1 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {5} \
 ] $axis_clock_converter_1

  # Create instance: axis_clock_converter_2, and set properties
  set axis_clock_converter_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_2 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {5} \
 ] $axis_clock_converter_2

  # Create instance: axis_data_fifo_0, and set properties
  set axis_data_fifo_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo:2.0 axis_data_fifo_0 ]
  set_property -dict [ list \
   CONFIG.FIFO_DEPTH {16} \
 ] $axis_data_fifo_0

  # Create instance: axis_register_slice_3, and set properties
  set axis_register_slice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_3 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_3

  # Create instance: axis_register_slice_4, and set properties
  set axis_register_slice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_4 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_4

  # Create instance: axis_register_slice_5, and set properties
  set axis_register_slice_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_5 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_5

  # Create instance: axis_register_slice_6, and set properties
  set axis_register_slice_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_6 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_6

  # Create instance: axis_register_slice_7, and set properties
  set axis_register_slice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_7 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_7

  # Create instance: axis_register_slice_8, and set properties
  set axis_register_slice_8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_8 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_8

  # Create instance: axis_register_slice_9, and set properties
  set axis_register_slice_9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_9 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_9

  # Create instance: axis_register_slice_10, and set properties
  set axis_register_slice_10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_10 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_10

  # Create instance: axis_register_slice_11, and set properties
  set axis_register_slice_11 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_11 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_11

  # Create instance: axis_register_slice_12, and set properties
  set axis_register_slice_12 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_12 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_12

  # Create instance: axis_register_slice_13, and set properties
  set axis_register_slice_13 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_13 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_13

  # Create instance: axis_switch_0, and set properties
  set block_name axis_switch
  set block_cell_name axis_switch_0
  if { [catch {set axis_switch_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $axis_switch_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: blk_mem_gen_0, and set properties
  set blk_mem_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_0 ]
  set_property -dict [ list \
   CONFIG.Assume_Synchronous_Clk {true} \
   CONFIG.Byte_Size {9} \
   CONFIG.EN_SAFETY_CKT {false} \
   CONFIG.Enable_32bit_Address {false} \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Fill_Remaining_Memory_Locations {false} \
   CONFIG.Load_Init_File {false} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Operating_Mode_A {WRITE_FIRST} \
   CONFIG.Pipeline_Stages {0} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Read_Width_A {2} \
   CONFIG.Read_Width_B {2} \
   CONFIG.Register_PortA_Output_of_Memory_Primitives {true} \
   CONFIG.Register_PortB_Output_of_Memory_Core {false} \
   CONFIG.Register_PortB_Output_of_Memory_Primitives {true} \
   CONFIG.Remaining_Memory_Locations {0} \
   CONFIG.Use_Byte_Write_Enable {false} \
   CONFIG.Use_RSTA_Pin {false} \
   CONFIG.Use_RSTB_Pin {false} \
   CONFIG.Write_Depth_A {1048576} \
   CONFIG.Write_Width_A {2} \
   CONFIG.Write_Width_B {2} \
   CONFIG.use_bram_block {Stand_Alone} \
 ] $blk_mem_gen_0

  # Create instance: bram_wrapper
  create_hier_cell_bram_wrapper $hier_obj bram_wrapper

  # Create instance: c_counter_binary_0, and set properties
  set c_counter_binary_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_counter_binary:12.0 c_counter_binary_0 ]
  set_property -dict [ list \
   CONFIG.Increment_Value {5} \
   CONFIG.Output_Width {64} \
 ] $c_counter_binary_0

  # Create instance: c_shift_ram_0, and set properties
  set c_shift_ram_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 c_shift_ram_0 ]
  set_property -dict [ list \
   CONFIG.AsyncInitVal {0} \
   CONFIG.DefaultData {0} \
   CONFIG.Depth {2} \
   CONFIG.Width {1} \
 ] $c_shift_ram_0

  # Create instance: inputSizeInBytes_0, and set properties
  set inputSizeInBytes_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:inputSizeInBytes:1.0 inputSizeInBytes_0 ]

  # Create instance: register_writes
  create_hier_cell_register_writes $hier_obj register_writes

  # Create instance: rx_qp_0, and set properties
  set rx_qp_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:rx_qp:1.0 rx_qp_0 ]

  # Create instance: rx_rdma_hdr_hier
  create_hier_cell_rx_rdma_hdr_hier $hier_obj rx_rdma_hdr_hier

  # Create instance: tx_checkCurrentPacke_0, and set properties
  set tx_checkCurrentPacke_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_checkCurrentPacket_dev:1.0 tx_checkCurrentPacke_0 ]

  # Create instance: tx_checkResponce_dev_0, and set properties
  set tx_checkResponce_dev_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_checkResponce_dev:1.0 tx_checkResponce_dev_0 ]

  # Create instance: tx_checkTimeout_dev_0, and set properties
  set tx_checkTimeout_dev_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_checkTimeout_dev:1.0 tx_checkTimeout_dev_0 ]

  # Create instance: tx_checkcurrent_data_0, and set properties
  set tx_checkcurrent_data_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_checkcurrent_data:1.0 tx_checkcurrent_data_0 ]

  # Create instance: tx_dev_mem_management_0, and set properties
  set tx_dev_mem_management_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_dev_mem_management:1.0 tx_dev_mem_management_0 ]

  # Create instance: tx_rdma_hdr
  create_hier_cell_tx_rdma_hdr $hier_obj tx_rdma_hdr

  # Create instance: tx_syncroniceWritesA_0, and set properties
  set tx_syncroniceWritesA_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_syncroniceWritesAndInterconnect_dev:1.0 tx_syncroniceWritesA_0 ]

  # Create instance: tx_write_dev_0, and set properties
  set tx_write_dev_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:tx_write_dev:1.0 tx_write_dev_0 ]

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_1 ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins m_axi_HBM_a_0] [get_bd_intf_pins tx_checkcurrent_data_0/m_axi_HBM_a]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins M_AXIS_0] [get_bd_intf_pins axis_register_slice_7/M_AXIS]
  connect_bd_intf_net -intf_net Conn3 [get_bd_intf_pins m_axi_HBM] [get_bd_intf_pins asynchronousWriteToH_0/m_axi_HBM]
  connect_bd_intf_net -intf_net axi_broadcast_readPSN_out1_V [get_bd_intf_pins axi_broadcast_readPSN/out1_V] [get_bd_intf_pins tx_write_dev_0/readPSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_readPSN_out2_V [get_bd_intf_pins axi_broadcast_readPSN/out2_V] [get_bd_intf_pins tx_syncroniceWritesA_0/readPSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_readPSN_out3_V [get_bd_intf_pins axi_broadcast_readPSN/out3_V] [get_bd_intf_pins tx_checkResponce_dev_0/readPSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_readPSN_out4_V [get_bd_intf_pins axi_broadcast_readPSN/out4_V] [get_bd_intf_pins tx_checkTimeout_dev_0/readPSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_writePSN_out1_V [get_bd_intf_pins axi_broadcast_writePSN/out1_V] [get_bd_intf_pins tx_checkCurrentPacke_0/writePSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_writePSN_out2_V [get_bd_intf_pins axi_broadcast_writePSN/out2_V] [get_bd_intf_pins tx_syncroniceWritesA_0/writePSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_writePSN_out3_V [get_bd_intf_pins axi_broadcast_writePSN/out3_V] [get_bd_intf_pins tx_checkResponce_dev_0/writePSN_l_V]
  connect_bd_intf_net -intf_net axi_broadcast_writePSN_out4_V [get_bd_intf_pins axi_broadcast_writePSN/out4_V] [get_bd_intf_pins tx_checkTimeout_dev_0/writePSN_l_V]
  connect_bd_intf_net -intf_net axis_clock_converter_0_M_AXIS [get_bd_intf_pins axis_clock_converter_0/M_AXIS] [get_bd_intf_pins axis_switch_0/s_axis_header]
  connect_bd_intf_net -intf_net axis_clock_converter_1_M_AXIS [get_bd_intf_pins axis_clock_converter_1/M_AXIS] [get_bd_intf_pins axis_switch_0/s_axis_data_0]
  connect_bd_intf_net -intf_net axis_clock_converter_2_M_AXIS [get_bd_intf_pins axis_clock_converter_2/M_AXIS] [get_bd_intf_pins axis_switch_0/s_axis_data_1]
  connect_bd_intf_net -intf_net axis_data_fifo_0_M_AXIS [get_bd_intf_pins axis_data_fifo_0/M_AXIS] [get_bd_intf_pins tx_syncroniceWritesA_0/userData_s]
  connect_bd_intf_net -intf_net axis_register_slice_10_M_AXIS [get_bd_intf_pins axi_broadcast_writePSN/in_V] [get_bd_intf_pins axis_register_slice_10/M_AXIS]
  connect_bd_intf_net -intf_net axis_register_slice_11_M_AXIS [get_bd_intf_pins axis_register_slice_10/S_AXIS] [get_bd_intf_pins axis_register_slice_11/M_AXIS]
  connect_bd_intf_net -intf_net axis_register_slice_12_M_AXIS [get_bd_intf_pins axis_register_slice_12/M_AXIS] [get_bd_intf_pins tx_rdma_hdr/tx_in]
  connect_bd_intf_net -intf_net axis_register_slice_13_M_AXIS [get_bd_intf_pins axis_register_slice_11/S_AXIS] [get_bd_intf_pins axis_register_slice_13/M_AXIS]
  connect_bd_intf_net -intf_net axis_register_slice_3_M_AXIS [get_bd_intf_pins axis_register_slice_3/M_AXIS] [get_bd_intf_pins tx_rdma_hdr/axis_frame_ctx_rx_V]
  connect_bd_intf_net -intf_net axis_register_slice_4_M_AXIS [get_bd_intf_pins axis_register_slice_4/M_AXIS] [get_bd_intf_pins rx_qp_0/rx_req_payload_i]
  connect_bd_intf_net -intf_net axis_register_slice_5_M_AXIS [get_bd_intf_pins axis_register_slice_5/M_AXIS] [get_bd_intf_pins tx_checkResponce_dev_0/header_s_V]
  connect_bd_intf_net -intf_net axis_register_slice_6_M_AXIS [get_bd_intf_pins axis_register_slice_6/M_AXIS] [get_bd_intf_pins rx_qp_0/rx_req_i_V]
  connect_bd_intf_net -intf_net axis_register_slice_9_M_AXIS [get_bd_intf_pins axis_register_slice_13/S_AXIS] [get_bd_intf_pins axis_register_slice_9/M_AXIS]
  connect_bd_intf_net -intf_net axis_switch_0_m_axis_out [get_bd_intf_pins asynchronousWriteToH_0/streamFromWrtie_i] [get_bd_intf_pins axis_switch_0/m_axis_out]
  connect_bd_intf_net -intf_net inputPacket_0_1 [get_bd_intf_pins inputData] [get_bd_intf_pins inputSizeInBytes_0/inputPacket]
  connect_bd_intf_net -intf_net inputSizeInBytes_0_outputPacket [get_bd_intf_pins axis_data_fifo_0/S_AXIS] [get_bd_intf_pins inputSizeInBytes_0/outputPacket]
  connect_bd_intf_net -intf_net inputSizeInBytes_0_packetSize_V [get_bd_intf_pins inputSizeInBytes_0/packetSize_V] [get_bd_intf_pins tx_syncroniceWritesA_0/userDataSize_s_V]
  connect_bd_intf_net -intf_net rx_i_0_1 [get_bd_intf_pins rx_i] [get_bd_intf_pins rx_rdma_hdr_hier/rx_i]
  connect_bd_intf_net -intf_net rx_qp_0_axis_tx_resp_ctx_o_V1 [get_bd_intf_pins axis_register_slice_3/S_AXIS] [get_bd_intf_pins rx_qp_0/axis_tx_resp_ctx_o_V]
  connect_bd_intf_net -intf_net rx_qp_0_outData_o1 [get_bd_intf_pins axis_register_slice_7/S_AXIS] [get_bd_intf_pins rx_qp_0/outData_o]
  connect_bd_intf_net -intf_net rx_rdma_hdr_0_rx_req_o_V [get_bd_intf_pins axis_register_slice_6/S_AXIS] [get_bd_intf_pins rx_rdma_hdr_hier/rx_req_o_V]
  connect_bd_intf_net -intf_net rx_rdma_hdr_0_rx_req_payload_o [get_bd_intf_pins axis_register_slice_4/S_AXIS] [get_bd_intf_pins rx_rdma_hdr_hier/rx_req_payload_o]
  connect_bd_intf_net -intf_net rx_rdma_hdr_0_tx_resp_o_V [get_bd_intf_pins axis_register_slice_5/S_AXIS] [get_bd_intf_pins rx_rdma_hdr_hier/tx_resp_o_V]
  connect_bd_intf_net -intf_net tx_checkCurrentPacke_0_dataToWrite_s [get_bd_intf_pins tx_checkcurrent_data_0/dataToWrite_s] [get_bd_intf_pins tx_syncroniceWritesA_0/nacANDretranssmitData_s]
  connect_bd_intf_net -intf_net tx_checkCurrentPacke_0_headerToWrite_s_V [get_bd_intf_pins tx_checkcurrent_data_0/headerToWrite_s_V] [get_bd_intf_pins tx_syncroniceWritesA_0/nacANDretranssmitHeader_s_V]
  connect_bd_intf_net -intf_net tx_checkCurrentPacke_0_readPSN_l_V [get_bd_intf_pins axi_broadcast_readPSN/in_V] [get_bd_intf_pins tx_checkCurrentPacke_0/readPSN_l_V]
  connect_bd_intf_net -intf_net tx_checkCurrentPacke_0_statBitMap_address_o_V [get_bd_intf_pins tx_checkCurrentPacke_0/statBitMap_address_o_V] [get_bd_intf_pins tx_dev_mem_management_0/checkCurrent_address_w_s_V]
  connect_bd_intf_net -intf_net tx_checkCurrentPacke_0_statBitMap_data_o_V [get_bd_intf_pins tx_checkCurrentPacke_0/statBitMap_data_o_V] [get_bd_intf_pins tx_dev_mem_management_0/checkCurrent_data_w_s_V]
  connect_bd_intf_net -intf_net tx_checkResponce_dev_0_statBitMap_address_i_V [get_bd_intf_pins tx_checkResponce_dev_0/statBitMap_address_i_V] [get_bd_intf_pins tx_dev_mem_management_0/checkResponce_address_w_s_V]
  connect_bd_intf_net -intf_net tx_checkResponce_dev_0_statBitMap_data_i_V [get_bd_intf_pins tx_checkResponce_dev_0/statBitMap_data_i_V] [get_bd_intf_pins tx_dev_mem_management_0/checkResponce_data_w_s_V]
  connect_bd_intf_net -intf_net tx_checkTimeout_dev_0_address_i_V [get_bd_intf_pins tx_checkTimeout_dev_0/address_i_V] [get_bd_intf_pins tx_dev_mem_management_0/checkTimeout_address_w_s_V]
  connect_bd_intf_net -intf_net tx_checkTimeout_dev_0_data_i_V [get_bd_intf_pins tx_checkTimeout_dev_0/data_i_V] [get_bd_intf_pins tx_dev_mem_management_0/checkTimeout_data_w_s_V]
  connect_bd_intf_net -intf_net tx_checkTimeout_dev_0_m_axi_header [get_bd_intf_pins m_axi_header] [get_bd_intf_pins tx_checkTimeout_dev_0/m_axi_header]
  connect_bd_intf_net -intf_net tx_rdma_hdr_0_tx_out [get_bd_intf_pins tx_out] [get_bd_intf_pins axis_register_slice_8/M_AXIS]
  connect_bd_intf_net -intf_net tx_rdma_hdr_0_tx_out1 [get_bd_intf_pins axis_register_slice_8/S_AXIS] [get_bd_intf_pins tx_rdma_hdr/tx_out]
  connect_bd_intf_net -intf_net tx_syncroniceWritesA_0_outToWRITEData_s [get_bd_intf_pins tx_syncroniceWritesA_0/outToWRITEData_s] [get_bd_intf_pins tx_write_dev_0/data_s]
  connect_bd_intf_net -intf_net tx_syncroniceWritesA_0_outToWRITEHeader_s_V [get_bd_intf_pins tx_syncroniceWritesA_0/outToWRITEHeader_s_V] [get_bd_intf_pins tx_write_dev_0/header_V]
  connect_bd_intf_net -intf_net tx_write_dev_0_dataHBM_0 [get_bd_intf_pins axis_clock_converter_1/S_AXIS] [get_bd_intf_pins tx_write_dev_0/dataHBM_0]
  connect_bd_intf_net -intf_net tx_write_dev_0_dataHBM_1 [get_bd_intf_pins axis_clock_converter_2/S_AXIS] [get_bd_intf_pins tx_write_dev_0/dataHBM_1]
  connect_bd_intf_net -intf_net tx_write_dev_0_headerHBM [get_bd_intf_pins axis_clock_converter_0/S_AXIS] [get_bd_intf_pins tx_write_dev_0/headerHBM]
  connect_bd_intf_net -intf_net tx_write_dev_0_payloadOut_s [get_bd_intf_pins axis_register_slice_12/S_AXIS] [get_bd_intf_pins tx_write_dev_0/payloadOut_s]
  connect_bd_intf_net -intf_net tx_write_dev_0_sq_frame_context_s_V [get_bd_intf_pins tx_rdma_hdr/axis_frame_ctx_tx_V] [get_bd_intf_pins tx_write_dev_0/sq_frame_context_s_V]
  connect_bd_intf_net -intf_net tx_write_dev_0_statBitMap_address_i_V [get_bd_intf_pins tx_dev_mem_management_0/wrtie_address_w_s_V] [get_bd_intf_pins tx_write_dev_0/statBitMap_address_i_V]
  connect_bd_intf_net -intf_net tx_write_dev_0_statBitMap_data_i_V [get_bd_intf_pins tx_dev_mem_management_0/write_data_w_s_V] [get_bd_intf_pins tx_write_dev_0/statBitMap_data_i_V]
  connect_bd_intf_net -intf_net tx_write_dev_0_writePSN_l_V [get_bd_intf_pins axis_register_slice_9/S_AXIS] [get_bd_intf_pins tx_write_dev_0/writePSN_l_V]

  # Create port connections
  connect_bd_net -net Endianness_S [get_bd_pins my_ip] [get_bd_pins Endianness/S] [get_bd_pins tx_rdma_hdr/my_ip]
  connect_bd_net -net Op1_1 [get_bd_pins bram_wrapper/Op1] [get_bd_pins register_writes/Q]
  connect_bd_net -net S02_ARESETN_1 [get_bd_pins ap_rst] [get_bd_pins peripheral_aresetn] [get_bd_pins axi_broadcast_readPSN/ap_rst_n] [get_bd_pins axi_broadcast_writePSN/ap_rst_n] [get_bd_pins axis_clock_converter_0/s_axis_aresetn] [get_bd_pins axis_clock_converter_1/s_axis_aresetn] [get_bd_pins axis_clock_converter_2/s_axis_aresetn] [get_bd_pins axis_data_fifo_0/s_axis_aresetn] [get_bd_pins axis_register_slice_10/aresetn] [get_bd_pins axis_register_slice_11/aresetn] [get_bd_pins axis_register_slice_12/aresetn] [get_bd_pins axis_register_slice_13/aresetn] [get_bd_pins axis_register_slice_3/aresetn] [get_bd_pins axis_register_slice_4/aresetn] [get_bd_pins axis_register_slice_5/aresetn] [get_bd_pins axis_register_slice_6/aresetn] [get_bd_pins axis_register_slice_7/aresetn] [get_bd_pins axis_register_slice_9/aresetn] [get_bd_pins inputSizeInBytes_0/ap_rst_n] [get_bd_pins rx_qp_0/ap_rst_n] [get_bd_pins rx_rdma_hdr_hier/ap_rst_n] [get_bd_pins tx_checkCurrentPacke_0/ap_rst_n] [get_bd_pins tx_checkResponce_dev_0/ap_rst_n] [get_bd_pins tx_checkTimeout_dev_0/ap_rst_n] [get_bd_pins tx_checkcurrent_data_0/ap_rst_n] [get_bd_pins tx_dev_mem_management_0/ap_rst_n] [get_bd_pins tx_rdma_hdr/ap_rst_n] [get_bd_pins tx_syncroniceWritesA_0/ap_rst_n] [get_bd_pins tx_write_dev_0/ap_rst_n]
  connect_bd_net -net ap_clk_0_1 [get_bd_pins ap_clk] [get_bd_pins Endianness/ap_clk] [get_bd_pins axi_broadcast_readPSN/ap_clk] [get_bd_pins axi_broadcast_writePSN/ap_clk] [get_bd_pins axis_clock_converter_0/s_axis_aclk] [get_bd_pins axis_clock_converter_1/s_axis_aclk] [get_bd_pins axis_clock_converter_2/s_axis_aclk] [get_bd_pins axis_data_fifo_0/s_axis_aclk] [get_bd_pins axis_register_slice_10/aclk] [get_bd_pins axis_register_slice_11/aclk] [get_bd_pins axis_register_slice_12/aclk] [get_bd_pins axis_register_slice_13/aclk] [get_bd_pins axis_register_slice_3/aclk] [get_bd_pins axis_register_slice_4/aclk] [get_bd_pins axis_register_slice_5/aclk] [get_bd_pins axis_register_slice_6/aclk] [get_bd_pins axis_register_slice_7/aclk] [get_bd_pins axis_register_slice_9/aclk] [get_bd_pins blk_mem_gen_0/clka] [get_bd_pins blk_mem_gen_0/clkb] [get_bd_pins bram_wrapper/ap_clk] [get_bd_pins c_counter_binary_0/CLK] [get_bd_pins c_shift_ram_0/CLK] [get_bd_pins inputSizeInBytes_0/ap_clk] [get_bd_pins register_writes/ap_clk] [get_bd_pins rx_qp_0/ap_clk] [get_bd_pins rx_rdma_hdr_hier/ap_clk] [get_bd_pins tx_checkCurrentPacke_0/ap_clk] [get_bd_pins tx_checkResponce_dev_0/ap_clk] [get_bd_pins tx_checkTimeout_dev_0/ap_clk] [get_bd_pins tx_checkcurrent_data_0/ap_clk] [get_bd_pins tx_dev_mem_management_0/ap_clk] [get_bd_pins tx_rdma_hdr/ap_clk] [get_bd_pins tx_syncroniceWritesA_0/ap_clk] [get_bd_pins tx_write_dev_0/ap_clk]
  connect_bd_net -net ap_rst_n_0_1 [get_bd_pins ap_rst_n_0] [get_bd_pins rx_rdma_hdr_hier/ap_rst_n_0] [get_bd_pins tx_rdma_hdr/m_axis_aresetn]
  connect_bd_net -net axi_broadcast_readPSN_in_V_TREADY [get_bd_pins axi_broadcast_readPSN/in_V_TREADY] [get_bd_pins tx_checkCurrentPacke_0/readPSN_l_V_TREADY]
  connect_bd_net -net axi_broadcast_writePSN_in_V_TREADY [get_bd_pins axi_broadcast_writePSN/in_V_TREADY] [get_bd_pins axis_register_slice_10/m_axis_tready]
  connect_bd_net -net axis_register_slice_10_m_axis_tdata [get_bd_pins m_axis_tdata] [get_bd_pins axi_broadcast_writePSN/in_V_TDATA] [get_bd_pins axis_register_slice_10/m_axis_tdata]
  connect_bd_net -net axis_register_slice_10_m_axis_tvalid [get_bd_pins axi_broadcast_writePSN/in_V_TVALID] [get_bd_pins axis_register_slice_10/m_axis_tvalid]
  connect_bd_net -net blk_mem_gen_0_doutb [get_bd_pins bram_wrapper/outToRead] [get_bd_pins tx_checkCurrentPacke_0/bitMap_q0] [get_bd_pins tx_checkTimeout_dev_0/currentPacketStatus]
  connect_bd_net -net blk_mem_gen_0_doutb1 [get_bd_pins blk_mem_gen_0/doutb] [get_bd_pins bram_wrapper/read_data]
  connect_bd_net -net bram_wrapper_Res [get_bd_pins blk_mem_gen_0/ena] [get_bd_pins bram_wrapper/Res]
  connect_bd_net -net c_counter_binary_0_Q [get_bd_pins c_counter_binary_0/Q] [get_bd_pins tx_checkTimeout_dev_0/timer] [get_bd_pins tx_write_dev_0/currTime]
  connect_bd_net -net c_shift_ram_0_Q [get_bd_pins c_shift_ram_0/Q] [get_bd_pins tx_checkTimeout_dev_0/currentPacketStatus_ap_vld]
  connect_bd_net -net c_shift_ram_1_Q [get_bd_pins blk_mem_gen_0/wea] [get_bd_pins register_writes/Q2]
  connect_bd_net -net c_shift_ram_3_Q [get_bd_pins blk_mem_gen_0/dina] [get_bd_pins register_writes/Q1]
  connect_bd_net -net c_shift_ram_4_Q [get_bd_pins blk_mem_gen_0/addra] [get_bd_pins bram_wrapper/D1] [get_bd_pins register_writes/Q3]
  connect_bd_net -net ip_subnet_0_1 [get_bd_pins ip_subnet] [get_bd_pins Endianness/ip_subnet] [get_bd_pins rx_qp_0/ip_subnet] [get_bd_pins tx_write_dev_0/ip_subnet]
  connect_bd_net -net local_qp_0_1 [get_bd_pins local_ID] [get_bd_pins Endianness/local_ID] [get_bd_pins rx_qp_0/qpn_local_i] [get_bd_pins tx_write_dev_0/local_qp]
  connect_bd_net -net m_axis_aclk1_1 [get_bd_pins m_axis_aclk1] [get_bd_pins asynchronousWriteToH_0/ap_clk] [get_bd_pins axis_clock_converter_0/m_axis_aclk] [get_bd_pins axis_clock_converter_1/m_axis_aclk] [get_bd_pins axis_clock_converter_2/m_axis_aclk] [get_bd_pins axis_register_slice_8/aclk] [get_bd_pins axis_switch_0/aclk] [get_bd_pins rx_rdma_hdr_hier/gt_txusrclk2] [get_bd_pins tx_rdma_hdr/m_axis_aclk1]
  connect_bd_net -net m_axis_aclk_1 [get_bd_pins m_axis_aclk] [get_bd_pins rx_rdma_hdr_hier/clk_out3] [get_bd_pins tx_rdma_hdr/m_axis_aclk]
  connect_bd_net -net numberOfretranssmisionsForressetingMSN_0_1 [get_bd_pins nRetransmissions] [get_bd_pins tx_write_dev_0/numberOfretranssmisionsForressetingMSN]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins resetn_300] [get_bd_pins asynchronousWriteToH_0/ap_rst_n] [get_bd_pins axis_clock_converter_0/m_axis_aresetn] [get_bd_pins axis_clock_converter_1/m_axis_aresetn] [get_bd_pins axis_clock_converter_2/m_axis_aresetn] [get_bd_pins axis_register_slice_8/aresetn] [get_bd_pins axis_switch_0/aresetn] [get_bd_pins rx_rdma_hdr_hier/peripheral_aresetn] [get_bd_pins tx_rdma_hdr/peripheral_aresetn]
  connect_bd_net -net timeoutInNs_0_1 [get_bd_pins timeoutInNs] [get_bd_pins tx_checkTimeout_dev_0/timeoutInNs]
  connect_bd_net -net tx_checkCurrentPacke_0_bitMap_address0 [get_bd_pins blk_mem_gen_0/addrb] [get_bd_pins bram_wrapper/D] [get_bd_pins tx_checkCurrentPacke_0/bitMap_address0]
  connect_bd_net -net tx_checkCurrentPacke_0_bitMap_ce0 [get_bd_pins blk_mem_gen_0/enb] [get_bd_pins bram_wrapper/Op2] [get_bd_pins c_shift_ram_0/D] [get_bd_pins tx_checkCurrentPacke_0/bitMap_ce0]
  connect_bd_net -net tx_checkCurrentPacke_0_readPSN_l_V_TDATA [get_bd_pins readPSN_l_V_TDATA] [get_bd_pins axi_broadcast_readPSN/in_V_TDATA] [get_bd_pins tx_checkCurrentPacke_0/readPSN_l_V_TDATA]
  connect_bd_net -net tx_checkCurrentPacke_0_readPSN_l_V_TVALID [get_bd_pins axi_broadcast_readPSN/in_V_TVALID] [get_bd_pins tx_checkCurrentPacke_0/readPSN_l_V_TVALID]
  connect_bd_net -net tx_checkCurrentPacke_0_toRetransmit_V_TDATA [get_bd_pins tx_checkCurrentPacke_0/toRetransmit_V_TDATA] [get_bd_pins tx_checkcurrent_data_0/retranssmit]
  connect_bd_net -net tx_checkCurrentPacke_0_toRetransmit_V_TVALID [get_bd_pins tx_checkCurrentPacke_0/toRetransmit_V_TVALID] [get_bd_pins tx_checkcurrent_data_0/ap_start]
  connect_bd_net -net tx_checkResponce_dev_0_size_o_V_TDATA [get_bd_pins size_o_V_TDATA_0] [get_bd_pins tx_checkResponce_dev_0/size_o_V_TDATA]
  connect_bd_net -net tx_checkTimeout_dev_0_sleep_o [get_bd_pins tx_checkTimeout_dev_0/sleep_o]
  connect_bd_net -net tx_checkcurrent_data_0_ap_done [get_bd_pins tx_checkCurrentPacke_0/toRetransmit_V_TREADY] [get_bd_pins tx_checkcurrent_data_0/ap_done]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_address1 [get_bd_pins register_writes/D3] [get_bd_pins tx_dev_mem_management_0/bitMap_address1]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_ce1 [get_bd_pins register_writes/D] [get_bd_pins tx_dev_mem_management_0/bitMap_ce1]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_d1 [get_bd_pins register_writes/D1] [get_bd_pins tx_dev_mem_management_0/bitMap_d1]
  connect_bd_net -net tx_dev_mem_management_0_bitMap_we1 [get_bd_pins register_writes/D2] [get_bd_pins tx_dev_mem_management_0/bitMap_we1]
  connect_bd_net -net udp_port [get_bd_pins udp_port] [get_bd_pins tx_rdma_hdr/udp_port] [get_bd_pins tx_write_dev_0/req_rdma_udp_port_i]
  connect_bd_net -net write_data_1 [get_bd_pins blk_mem_gen_0/douta] [get_bd_pins bram_wrapper/write_data]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins axi_broadcast_readPSN/out1_V_TREADY] [get_bd_pins axi_broadcast_readPSN/out2_V_TREADY] [get_bd_pins axi_broadcast_readPSN/out3_V_TREADY] [get_bd_pins axi_broadcast_readPSN/out4_V_TREADY] [get_bd_pins axi_broadcast_writePSN/out1_V_TREADY] [get_bd_pins axi_broadcast_writePSN/out2_V_TREADY] [get_bd_pins axi_broadcast_writePSN/out3_V_TREADY] [get_bd_pins axi_broadcast_writePSN/out4_V_TREADY] [get_bd_pins tx_checkResponce_dev_0/resp_msn_V_TREADY] [get_bd_pins tx_checkResponce_dev_0/resp_psn_o_V_TREADY] [get_bd_pins tx_checkResponce_dev_0/resp_qp_o_V_TREADY] [get_bd_pins tx_checkResponce_dev_0/resp_syndrome_o_V_TREADY] [get_bd_pins tx_checkResponce_dev_0/size_o_V_TREADY] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins rx_rdma_hdr_hier/tx_resp_payload_o_TREADY] [get_bd_pins tx_write_dev_0/errorDataOut_s_TREADY] [get_bd_pins tx_write_dev_0/statOut_s_V_TREADY] [get_bd_pins xlconstant_1/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: registers_0
proc create_hier_cell_registers_0 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_registers_0() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_AXILiteS_0


  # Create pins
  create_bd_pin -dir O -from 15 -to 0 Ntranssmisions
  create_bd_pin -dir I -type rst ap_rst_n
  create_bd_pin -dir O -from 0 -to 0 arp_clean
  create_bd_pin -dir I -type clk clk
  create_bd_pin -dir I -type clk clk_in_300
  create_bd_pin -dir O -from 12 -to 0 local_ID
  create_bd_pin -dir O -from 12 -to 0 -type data local_id_300
  create_bd_pin -dir O -from 47 -to 0 my_mac
  create_bd_pin -dir O -from 15 -to 0 packet_port
  create_bd_pin -dir O -from 15 -to 0 packet_port_300
  create_bd_pin -dir I -from 23 -to 0 -type data readPSN_in_V
  create_bd_pin -dir I -from 31 -to 0 size
  create_bd_pin -dir O -from 31 -to 0 subnet
  create_bd_pin -dir O -from 63 -to 0 timeout
  create_bd_pin -dir I -from 23 -to 0 -type data writePSN_in_V

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_ALL_INPUTS {1} \
 ] $axi_gpio_0

  # Create instance: axi_register_slice_0, and set properties
  set axi_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_0 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {4} \
   CONFIG.PROTOCOL {AXI4LITE} \
   CONFIG.REG_R {7} \
   CONFIG.REG_W {7} \
 ] $axi_register_slice_0

  # Create instance: cdc_read_psn, and set properties
  set cdc_read_psn [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 cdc_read_psn ]
  set_property -dict [ list \
   CONFIG.WIDTH {24} \
 ] $cdc_read_psn

  # Create instance: cdc_write_psn, and set properties
  set cdc_write_psn [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 cdc_write_psn ]
  set_property -dict [ list \
   CONFIG.WIDTH {24} \
 ] $cdc_write_psn

  # Create instance: cdc_write_psn1, and set properties
  set cdc_write_psn1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 cdc_write_psn1 ]
  set_property -dict [ list \
   CONFIG.DEST_SYNC_FF {6} \
   CONFIG.WIDTH {32} \
 ] $cdc_write_psn1

  # Create instance: register_map_full_0, and set properties
  set register_map_full_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:register_map_full:1.0 register_map_full_0 ]

  # Create instance: xpm_cdc_gen_1, and set properties
  set xpm_cdc_gen_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_1 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {32} \
 ] $xpm_cdc_gen_1

  # Create instance: xpm_cdc_gen_2, and set properties
  set xpm_cdc_gen_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_2 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {64} \
 ] $xpm_cdc_gen_2

  # Create instance: xpm_cdc_gen_3, and set properties
  set xpm_cdc_gen_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_3 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {13} \
 ] $xpm_cdc_gen_3

  # Create instance: xpm_cdc_gen_4, and set properties
  set xpm_cdc_gen_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_4 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {13} \
 ] $xpm_cdc_gen_4

  # Create instance: xpm_cdc_gen_5, and set properties
  set xpm_cdc_gen_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_5 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {48} \
 ] $xpm_cdc_gen_5

  # Create instance: xpm_cdc_gen_6, and set properties
  set xpm_cdc_gen_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_6 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {16} \
 ] $xpm_cdc_gen_6

  # Create instance: xpm_cdc_gen_7, and set properties
  set xpm_cdc_gen_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_7 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {16} \
 ] $xpm_cdc_gen_7

  # Create instance: xpm_cdc_gen_8, and set properties
  set xpm_cdc_gen_8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_8 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {1} \
 ] $xpm_cdc_gen_8

  # Create instance: xpm_cdc_gen_9, and set properties
  set xpm_cdc_gen_9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_9 ]
  set_property -dict [ list \
   CONFIG.CDC_TYPE {xpm_cdc_array_single} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.SRC_INPUT_REG {true} \
   CONFIG.WIDTH {16} \
 ] $xpm_cdc_gen_9

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axi_AXILiteS_0] [get_bd_intf_pins register_map_full_0/s_axi_AXILiteS]
  connect_bd_intf_net -intf_net S_AXI_1 [get_bd_intf_pins S_AXI] [get_bd_intf_pins axi_register_slice_0/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_0_M_AXI [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins axi_register_slice_0/M_AXI]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins clk] [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins axi_register_slice_0/aclk] [get_bd_pins cdc_read_psn/dest_clk] [get_bd_pins cdc_read_psn/src_clk] [get_bd_pins cdc_write_psn/dest_clk] [get_bd_pins cdc_write_psn/src_clk] [get_bd_pins cdc_write_psn1/dest_clk] [get_bd_pins cdc_write_psn1/src_clk] [get_bd_pins register_map_full_0/ap_clk] [get_bd_pins xpm_cdc_gen_1/dest_clk] [get_bd_pins xpm_cdc_gen_1/src_clk] [get_bd_pins xpm_cdc_gen_2/dest_clk] [get_bd_pins xpm_cdc_gen_2/src_clk] [get_bd_pins xpm_cdc_gen_3/dest_clk] [get_bd_pins xpm_cdc_gen_3/src_clk] [get_bd_pins xpm_cdc_gen_4/src_clk] [get_bd_pins xpm_cdc_gen_5/src_clk] [get_bd_pins xpm_cdc_gen_6/dest_clk] [get_bd_pins xpm_cdc_gen_6/src_clk] [get_bd_pins xpm_cdc_gen_7/src_clk] [get_bd_pins xpm_cdc_gen_8/src_clk] [get_bd_pins xpm_cdc_gen_9/dest_clk] [get_bd_pins xpm_cdc_gen_9/src_clk]
  connect_bd_net -net ap_rst_n_1 [get_bd_pins ap_rst_n] [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_register_slice_0/aresetn] [get_bd_pins register_map_full_0/ap_rst_n]
  connect_bd_net -net cdc_write_psn1_dest_out [get_bd_pins axi_gpio_0/gpio_io_i] [get_bd_pins cdc_write_psn1/dest_out]
  connect_bd_net -net clk_in_300_1 [get_bd_pins clk_in_300] [get_bd_pins xpm_cdc_gen_4/dest_clk] [get_bd_pins xpm_cdc_gen_5/dest_clk] [get_bd_pins xpm_cdc_gen_7/dest_clk] [get_bd_pins xpm_cdc_gen_8/dest_clk]
  connect_bd_net -net readPSN_in_V_1 [get_bd_pins readPSN_in_V] [get_bd_pins cdc_read_psn/src_in]
  connect_bd_net -net register_map_full_0_arp_clean_n_out_V [get_bd_pins register_map_full_0/arp_clean_n_out_V] [get_bd_pins xpm_cdc_gen_8/src_in]
  connect_bd_net -net register_map_full_0_ip_subnet_out_V [get_bd_pins register_map_full_0/ip_subnet_out_V] [get_bd_pins xpm_cdc_gen_1/src_in]
  connect_bd_net -net register_map_full_0_local_ID_out_V [get_bd_pins register_map_full_0/local_ID_out_V] [get_bd_pins xpm_cdc_gen_3/src_in] [get_bd_pins xpm_cdc_gen_4/src_in]
  connect_bd_net -net register_map_full_0_myMac_out_V [get_bd_pins register_map_full_0/myMac_out_V] [get_bd_pins xpm_cdc_gen_5/src_in]
  connect_bd_net -net register_map_full_0_nRetransmissions_out_V [get_bd_pins register_map_full_0/nRetransmissions_out_V] [get_bd_pins xpm_cdc_gen_6/src_in]
  connect_bd_net -net register_map_full_0_timeoutInNs_out_V [get_bd_pins register_map_full_0/timeoutInNs_out_V] [get_bd_pins xpm_cdc_gen_2/src_in]
  connect_bd_net -net register_map_full_0_udp_port_out_V [get_bd_pins register_map_full_0/udp_port_out_V] [get_bd_pins xpm_cdc_gen_7/src_in] [get_bd_pins xpm_cdc_gen_9/src_in]
  connect_bd_net -net src_in_1 [get_bd_pins size] [get_bd_pins cdc_write_psn1/src_in]
  connect_bd_net -net writePSN_in_V_1 [get_bd_pins writePSN_in_V] [get_bd_pins cdc_write_psn/src_in]
  connect_bd_net -net xpm_cdc_gen_10_dest_out [get_bd_pins cdc_read_psn/dest_out] [get_bd_pins register_map_full_0/readPSN_in_V]
  connect_bd_net -net xpm_cdc_gen_11_dest_out [get_bd_pins cdc_write_psn/dest_out] [get_bd_pins register_map_full_0/writePSN_in_V]
  connect_bd_net -net xpm_cdc_gen_1_dest_out [get_bd_pins subnet] [get_bd_pins xpm_cdc_gen_1/dest_out]
  connect_bd_net -net xpm_cdc_gen_2_dest_out [get_bd_pins timeout] [get_bd_pins xpm_cdc_gen_2/dest_out]
  connect_bd_net -net xpm_cdc_gen_3_dest_out [get_bd_pins local_ID] [get_bd_pins xpm_cdc_gen_3/dest_out]
  connect_bd_net -net xpm_cdc_gen_4_dest_out [get_bd_pins local_id_300] [get_bd_pins xpm_cdc_gen_4/dest_out]
  connect_bd_net -net xpm_cdc_gen_5_dest_out [get_bd_pins my_mac] [get_bd_pins xpm_cdc_gen_5/dest_out]
  connect_bd_net -net xpm_cdc_gen_6_dest_out [get_bd_pins Ntranssmisions] [get_bd_pins xpm_cdc_gen_6/dest_out]
  connect_bd_net -net xpm_cdc_gen_7_dest_out [get_bd_pins packet_port_300] [get_bd_pins xpm_cdc_gen_7/dest_out]
  connect_bd_net -net xpm_cdc_gen_8_dest_out [get_bd_pins arp_clean] [get_bd_pins xpm_cdc_gen_8/dest_out]
  connect_bd_net -net xpm_cdc_gen_9_dest_out [get_bd_pins packet_port] [get_bd_pins xpm_cdc_gen_9/dest_out]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: ethernet_stack_0
proc create_hier_cell_ethernet_stack_0 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_ethernet_stack_0() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 ethernet_rx_0

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 rdma_rx

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 tx_ethernet_0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 tx_rdma_0


  # Create pins
  create_bd_pin -dir I -type clk ap_clk_0
  create_bd_pin -dir I -type rst ap_rst_n_0
  create_bd_pin -dir I -from 0 -to 0 -type data arp_clear_n_V_0
  create_bd_pin -dir I -from 12 -to 0 -type data id_src_V
  create_bd_pin -dir I -from 31 -to 0 -type data myIpAddress_V_0
  create_bd_pin -dir I -from 47 -to 0 -type data myMacAddress_V_0
  create_bd_pin -dir I -from 15 -to 0 -type data roce_port_V_0

  # Create instance: Mac
  create_hier_cell_Mac $hier_obj Mac

  # Create instance: arp_server_subnet_0, and set properties
  set arp_server_subnet_0 [ create_bd_cell -type ip -vlnv ethz.systems.fpga:hls:arp_server_subnet:1.1 arp_server_subnet_0 ]

  # Create instance: axis_data_fifo_0, and set properties
  set axis_data_fifo_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo:2.0 axis_data_fifo_0 ]

  # Create instance: axis_dwidth_converter_0, and set properties
  set axis_dwidth_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_dwidth_converter:1.1 axis_dwidth_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {8} \
 ] $axis_dwidth_converter_0

  # Create instance: axis_dwidth_converter_1, and set properties
  set axis_dwidth_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_dwidth_converter:1.1 axis_dwidth_converter_1 ]
  set_property -dict [ list \
   CONFIG.HAS_MI_TKEEP {1} \
   CONFIG.M_TDATA_NUM_BYTES {64} \
 ] $axis_dwidth_converter_1

  # Create instance: axis_register_slice_0, and set properties
  set axis_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_0 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_0

  # Create instance: axis_register_slice_1, and set properties
  set axis_register_slice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_1 ]

  # Create instance: axis_register_slice_2, and set properties
  set axis_register_slice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_2 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_2

  # Create instance: axis_register_slice_3, and set properties
  set axis_register_slice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_3 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_3

  # Create instance: cmac_tkeep_fix_0, and set properties
  set cmac_tkeep_fix_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:cmac_tkeep_fix:1.0 cmac_tkeep_fix_0 ]

  # Create instance: cut_packet_0, and set properties
  set cut_packet_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:cut_packet:1.0 cut_packet_0 ]

  # Create instance: cut_packet_1, and set properties
  set cut_packet_1 [ create_bd_cell -type ip -vlnv xilinx.com:hls:cut_packet:1.0 cut_packet_1 ]

  # Create instance: eth_stream_switch_dev_0, and set properties
  set eth_stream_switch_dev_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:eth_stream_switch_dev:1.0 eth_stream_switch_dev_0 ]

  # Create instance: eth_stream_switch_dev_1, and set properties
  set eth_stream_switch_dev_1 [ create_bd_cell -type ip -vlnv xilinx.com:hls:eth_stream_switch_dev:1.0 eth_stream_switch_dev_1 ]

  # Create instance: icmp_server_0, and set properties
  set icmp_server_0 [ create_bd_cell -type ip -vlnv xilinx.labs:hls:icmp_server:1.67 icmp_server_0 ]

  # Create instance: insert_ethernet_head_0, and set properties
  set insert_ethernet_head_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:insert_ethernet_header:1.0 insert_ethernet_head_0 ]

  # Create instance: ip_handler_1, and set properties
  set block_name ip_handler
  set block_cell_name ip_handler_1
  if { [catch {set ip_handler_1 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $ip_handler_1 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
 ] [get_bd_intf_pins /ethernet_stack_0/ip_handler_1/m_axis_arp]

  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
 ] [get_bd_intf_pins /ethernet_stack_0/ip_handler_1/m_axis_hivenet]

  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
 ] [get_bd_intf_pins /ethernet_stack_0/ip_handler_1/m_axis_icmp]

  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
 ] [get_bd_intf_pins /ethernet_stack_0/ip_handler_1/m_axis_other]

  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
 ] [get_bd_pins /ethernet_stack_0/ip_handler_1/aclk]

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_1 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $xlconstant_1

  # Create instance: xlconstant_3, and set properties
  set xlconstant_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_3 ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins tx_rdma_0] [get_bd_intf_pins eth_stream_switch_dev_0/tx_rdma_i]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins tx_ethernet_0] [get_bd_intf_pins cmac_tkeep_fix_0/out_r]
  connect_bd_intf_net -intf_net arp_server_subnet_0_m_axis [get_bd_intf_pins arp_server_subnet_0/m_axis] [get_bd_intf_pins eth_stream_switch_dev_1/tx_eth_i]
  connect_bd_intf_net -intf_net axis_data_fifo_0_M_AXIS [get_bd_intf_pins axis_data_fifo_0/M_AXIS] [get_bd_intf_pins axis_dwidth_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_dwidth_converter_0_M_AXIS [get_bd_intf_pins axis_dwidth_converter_0/M_AXIS] [get_bd_intf_pins icmp_server_0/s_axis]
  connect_bd_intf_net -intf_net axis_dwidth_converter_1_M_AXIS [get_bd_intf_pins axis_dwidth_converter_1/M_AXIS] [get_bd_intf_pins eth_stream_switch_dev_0/tx_eth_i]
  connect_bd_intf_net -intf_net axis_register_slice_0_M_AXIS [get_bd_intf_pins axis_register_slice_0/M_AXIS] [get_bd_intf_pins cmac_tkeep_fix_0/in_r]
  connect_bd_intf_net -intf_net axis_register_slice_1_M_AXIS [get_bd_intf_pins axis_register_slice_1/M_AXIS] [get_bd_intf_pins ip_handler_1/s_axis_raw]
  connect_bd_intf_net -intf_net axis_register_slice_2_M_AXIS [get_bd_intf_pins axis_register_slice_2/M_AXIS] [get_bd_intf_pins eth_stream_switch_dev_1/tx_rdma_i]
  connect_bd_intf_net -intf_net axis_register_slice_3_M_AXIS [get_bd_intf_pins axis_register_slice_3/M_AXIS] [get_bd_intf_pins insert_ethernet_head_0/input_r]
  connect_bd_intf_net -intf_net cut_packet_0_output_r [get_bd_intf_pins rdma_rx] [get_bd_intf_pins cut_packet_0/output_r]
  connect_bd_intf_net -intf_net cut_packet_1_output_r [get_bd_intf_pins axis_data_fifo_0/S_AXIS] [get_bd_intf_pins cut_packet_1/output_r]
  connect_bd_intf_net -intf_net eth_stream_switch_dev_0_tx_o [get_bd_intf_pins axis_register_slice_3/S_AXIS] [get_bd_intf_pins eth_stream_switch_dev_0/tx_o]
  connect_bd_intf_net -intf_net eth_stream_switch_dev_1_tx_o [get_bd_intf_pins axis_register_slice_0/S_AXIS] [get_bd_intf_pins eth_stream_switch_dev_1/tx_o]
  connect_bd_intf_net -intf_net ethernet_rx_0_1 [get_bd_intf_pins ethernet_rx_0] [get_bd_intf_pins axis_register_slice_1/S_AXIS]
  connect_bd_intf_net -intf_net icmp_server_0_m_axis [get_bd_intf_pins axis_dwidth_converter_1/S_AXIS] [get_bd_intf_pins icmp_server_0/m_axis]
  connect_bd_intf_net -intf_net insert_ethernet_head_0_output_r [get_bd_intf_pins axis_register_slice_2/S_AXIS] [get_bd_intf_pins insert_ethernet_head_0/output_r]
  connect_bd_intf_net -intf_net ip_handler_1_m_axis_arp [get_bd_intf_pins arp_server_subnet_0/s_axis] [get_bd_intf_pins ip_handler_1/m_axis_arp]
  connect_bd_intf_net -intf_net ip_handler_1_m_axis_hivenet [get_bd_intf_pins cut_packet_0/input_r] [get_bd_intf_pins ip_handler_1/m_axis_hivenet]
  connect_bd_intf_net -intf_net ip_handler_1_m_axis_icmp [get_bd_intf_pins cut_packet_1/input_r] [get_bd_intf_pins ip_handler_1/m_axis_icmp]

  # Create port connections
  connect_bd_net -net Mac_dout [get_bd_pins Mac/dout] [get_bd_pins arp_server_subnet_0/myMacAddress_i_V]
  connect_bd_net -net ap_clk_0_1 [get_bd_pins ap_clk_0] [get_bd_pins arp_server_subnet_0/ap_clk] [get_bd_pins axis_data_fifo_0/s_axis_aclk] [get_bd_pins axis_dwidth_converter_0/aclk] [get_bd_pins axis_dwidth_converter_1/aclk] [get_bd_pins axis_register_slice_0/aclk] [get_bd_pins axis_register_slice_1/aclk] [get_bd_pins axis_register_slice_2/aclk] [get_bd_pins axis_register_slice_3/aclk] [get_bd_pins cmac_tkeep_fix_0/ap_clk] [get_bd_pins cut_packet_0/ap_clk] [get_bd_pins cut_packet_1/ap_clk] [get_bd_pins eth_stream_switch_dev_0/ap_clk] [get_bd_pins eth_stream_switch_dev_1/ap_clk] [get_bd_pins icmp_server_0/ap_clk] [get_bd_pins insert_ethernet_head_0/ap_clk] [get_bd_pins ip_handler_1/aclk]
  connect_bd_net -net ap_rst_n_0_1 [get_bd_pins ap_rst_n_0] [get_bd_pins arp_server_subnet_0/ap_rst_n] [get_bd_pins axis_data_fifo_0/s_axis_aresetn] [get_bd_pins axis_dwidth_converter_0/aresetn] [get_bd_pins axis_dwidth_converter_1/aresetn] [get_bd_pins axis_register_slice_0/aresetn] [get_bd_pins axis_register_slice_1/aresetn] [get_bd_pins axis_register_slice_2/aresetn] [get_bd_pins axis_register_slice_3/aresetn] [get_bd_pins cmac_tkeep_fix_0/ap_rst_n] [get_bd_pins cut_packet_0/ap_rst_n] [get_bd_pins cut_packet_1/ap_rst_n] [get_bd_pins eth_stream_switch_dev_0/ap_rst_n] [get_bd_pins eth_stream_switch_dev_1/ap_rst_n] [get_bd_pins icmp_server_0/ap_rst_n] [get_bd_pins insert_ethernet_head_0/ap_rst_n] [get_bd_pins ip_handler_1/aresetn]
  connect_bd_net -net arp_clear_n_V_0_1 [get_bd_pins arp_clear_n_V_0] [get_bd_pins arp_server_subnet_0/arp_clear_n_V]
  connect_bd_net -net id_src_V_1 [get_bd_pins id_src_V] [get_bd_pins Mac/local_ID] [get_bd_pins insert_ethernet_head_0/id_src_V]
  connect_bd_net -net insert_ethernet_head_0_ip_output_V_ip_V_TDATA [get_bd_pins arp_server_subnet_0/s_axis_arp_lookup_request_V_V_TDATA] [get_bd_pins insert_ethernet_head_0/ip_output_V_ip_V_TDATA]
  connect_bd_net -net insert_ethernet_head_0_ip_output_V_ip_V_TVALID [get_bd_pins arp_server_subnet_0/s_axis_arp_lookup_request_V_V_TVALID] [get_bd_pins insert_ethernet_head_0/ip_output_V_ip_V_TVALID]
  connect_bd_net -net myIpAddress_V_0_1 [get_bd_pins myIpAddress_V_0] [get_bd_pins arp_server_subnet_0/myIpAddress_i_V] [get_bd_pins ip_handler_1/my_ip_address]
  connect_bd_net -net myMacAddress_V_0_1 [get_bd_pins myMacAddress_V_0] [get_bd_pins Mac/myMacAddress_V_0] [get_bd_pins insert_ethernet_head_0/mac_subnet_V]
  connect_bd_net -net roce_port_V_0_1 [get_bd_pins roce_port_V_0] [get_bd_pins ip_handler_1/roce_port]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins ip_handler_1/m_axis_other_tready] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins icmp_server_0/ttlIn_TVALID] [get_bd_pins icmp_server_0/udpIn_TVALID] [get_bd_pins xlconstant_1/dout]
  connect_bd_net -net xlconstant_3_dout [get_bd_pins arp_server_subnet_0/m_axis_arp_lookup_reply_V_TREADY] [get_bd_pins arp_server_subnet_0/m_axis_host_arp_lookup_reply_V_TREADY] [get_bd_pins insert_ethernet_head_0/ip_output_V_ip_V_TREADY] [get_bd_pins xlconstant_3/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: HiveNet_0h
proc create_hier_cell_HiveNet_0h { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_HiveNet_0h() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M_AXIS_0

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 inputData

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_HBM

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 rx_i

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 tx_out


  # Create pins
  create_bd_pin -dir I -type rst ap_rst_n_0
  create_bd_pin -dir I -type rst aresetn
  create_bd_pin -dir I -from 31 -to 0 -type data ip_subnet
  create_bd_pin -dir I -from 12 -to 0 local_ID
  create_bd_pin -dir I -type clk m_axis_aclk
  create_bd_pin -dir I -type clk m_axis_aclk1
  create_bd_pin -dir O -from 23 -to 0 -type data m_axis_tdata
  create_bd_pin -dir O -from 31 -to 0 -type data my_ip
  create_bd_pin -dir I -from 15 -to 0 -type data nRetransmissions
  create_bd_pin -dir O -from 23 -to 0 -type data readPSN_l_V_TDATA
  create_bd_pin -dir I -type clk s_axis_aclk_0
  create_bd_pin -dir I -type rst s_axis_aresetn_0
  create_bd_pin -dir O -from 31 -to 0 size_o_V_TDATA_0
  create_bd_pin -dir I -from 63 -to 0 -type data timeoutInNs
  create_bd_pin -dir I -from 15 -to 0 -type data udp_port

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_ALL_OUTPUTS {1} \
 ] $axi_gpio_0

  # Create instance: hier_0
  create_hier_cell_hier_0 $hier_obj hier_0

  # Create instance: inter
  create_hier_cell_inter $hier_obj inter

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins S_AXI_0] [get_bd_intf_pins axi_gpio_0/S_AXI]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins M_AXIS_0] [get_bd_intf_pins hier_0/M_AXIS_0]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins m_axi_HBM] [get_bd_intf_pins hier_0/m_axi_HBM]
  connect_bd_intf_net -intf_net Conn5 [get_bd_intf_pins M_AXI_1] [get_bd_intf_pins inter/M_AXI_0]
  connect_bd_intf_net -intf_net HiveNet_0_tx_out [get_bd_intf_pins tx_out] [get_bd_intf_pins hier_0/tx_out]
  connect_bd_intf_net -intf_net hier_0_m_axi_HBM_a_0 [get_bd_intf_pins hier_0/m_axi_HBM_a_0] [get_bd_intf_pins inter/S_AXI2]
  connect_bd_intf_net -intf_net hier_0_m_axi_header [get_bd_intf_pins hier_0/m_axi_header] [get_bd_intf_pins inter/S_AXI1]
  connect_bd_intf_net -intf_net inputData_1 [get_bd_intf_pins inputData] [get_bd_intf_pins hier_0/inputData]
  connect_bd_intf_net -intf_net rx_i_1 [get_bd_intf_pins rx_i] [get_bd_intf_pins hier_0/rx_i]

  # Create port connections
  connect_bd_net -net ap_rst_n_0_1 [get_bd_pins ap_rst_n_0] [get_bd_pins hier_0/ap_rst_n_0]
  connect_bd_net -net aresetn_1 [get_bd_pins aresetn] [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins hier_0/resetn_300] [get_bd_pins inter/aresetn]
  connect_bd_net -net hier_0_m_axis_tdata [get_bd_pins m_axis_tdata] [get_bd_pins hier_0/m_axis_tdata]
  connect_bd_net -net hier_0_my_ip [get_bd_pins my_ip] [get_bd_pins hier_0/my_ip]
  connect_bd_net -net hier_0_readPSN_l_V_TDATA [get_bd_pins readPSN_l_V_TDATA] [get_bd_pins hier_0/readPSN_l_V_TDATA]
  connect_bd_net -net hier_0_size_o_V_TDATA_0 [get_bd_pins size_o_V_TDATA_0] [get_bd_pins hier_0/size_o_V_TDATA_0]
  connect_bd_net -net ip_subnet_1 [get_bd_pins ip_subnet] [get_bd_pins hier_0/ip_subnet]
  connect_bd_net -net local_ID_1 [get_bd_pins local_ID] [get_bd_pins hier_0/local_ID]
  connect_bd_net -net m_axis_aclk1_1 [get_bd_pins m_axis_aclk1] [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins hier_0/m_axis_aclk1] [get_bd_pins inter/m_axis_aclk1]
  connect_bd_net -net m_axis_aclk_1 [get_bd_pins m_axis_aclk] [get_bd_pins hier_0/m_axis_aclk]
  connect_bd_net -net nRetransmissions_1 [get_bd_pins nRetransmissions] [get_bd_pins hier_0/nRetransmissions]
  connect_bd_net -net s_axis_aclk_0_1 [get_bd_pins s_axis_aclk_0] [get_bd_pins hier_0/ap_clk] [get_bd_pins inter/s_axis_aclk_0]
  connect_bd_net -net s_axis_aresetn_0_1 [get_bd_pins s_axis_aresetn_0] [get_bd_pins hier_0/ap_rst] [get_bd_pins inter/s_axis_aresetn_0]
  connect_bd_net -net timeoutInNs_1 [get_bd_pins timeoutInNs] [get_bd_pins hier_0/timeoutInNs]
  connect_bd_net -net udp_port_1 [get_bd_pins udp_port] [get_bd_pins hier_0/udp_port]

  # Restore current instance
  current_bd_instance $oldCurInst
}


# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell HBM_BANK } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set HBM_read [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 HBM_read ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.DATA_WIDTH {256} \
   CONFIG.FREQ_HZ {300000000} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_REGION {0} \
   CONFIG.NUM_READ_OUTSTANDING {16} \
   CONFIG.NUM_WRITE_OUTSTANDING {32} \
   CONFIG.PROTOCOL {AXI4} \
   CONFIG.READ_WRITE_MODE {READ_ONLY} \
   ] $HBM_read

  set HBM_write [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 HBM_write ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.DATA_WIDTH {256} \
   CONFIG.FREQ_HZ {300000000} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_RRESP {0} \
   CONFIG.NUM_READ_OUTSTANDING {16} \
   CONFIG.NUM_WRITE_OUTSTANDING {256} \
   CONFIG.PROTOCOL {AXI4} \
   CONFIG.READ_WRITE_MODE {WRITE_ONLY} \
   ] $HBM_write

  set S00_AXI_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S00_AXI_0 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {16} \
   CONFIG.ARUSER_WIDTH {0} \
   CONFIG.AWUSER_WIDTH {0} \
   CONFIG.BUSER_WIDTH {0} \
   CONFIG.DATA_WIDTH {32} \
   CONFIG.FREQ_HZ {300000000} \
   CONFIG.HAS_BRESP {1} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_CACHE {0} \
   CONFIG.HAS_LOCK {0} \
   CONFIG.HAS_PROT {1} \
   CONFIG.HAS_QOS {0} \
   CONFIG.HAS_REGION {0} \
   CONFIG.HAS_RRESP {1} \
   CONFIG.HAS_WSTRB {1} \
   CONFIG.ID_WIDTH {0} \
   CONFIG.MAX_BURST_LENGTH {1} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_READ_THREADS {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_THREADS {1} \
   CONFIG.PROTOCOL {AXI4LITE} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   CONFIG.RUSER_BITS_PER_BYTE {0} \
   CONFIG.RUSER_WIDTH {0} \
   CONFIG.SUPPORTS_NARROW_BURST {0} \
   CONFIG.WUSER_BITS_PER_BYTE {0} \
   CONFIG.WUSER_WIDTH {0} \
   ] $S00_AXI_0

  set inputData [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 inputData ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {13} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $inputData

  set outData [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 outData ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
   ] $outData

  set rx [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 rx ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {1} \
   ] $rx

  set tx [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 tx ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {300000000} \
   ] $tx


  # Create ports
  set ap_clk [ create_bd_port -dir I -type clk -freq_hz 300000000 ap_clk ]
  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {outData:inputData:S00_AXI_0} \
   CONFIG.ASSOCIATED_RESET {ap_rst_n} \
 ] $ap_clk
  set ap_clk_300 [ create_bd_port -dir I -type clk -freq_hz 300000000 ap_clk_300 ]
  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {rx:tx:HBM_write:HBM_read} \
   CONFIG.ASSOCIATED_RESET {resetn_300} \
 ] $ap_clk_300
  set ap_rst_n [ create_bd_port -dir I -type rst ap_rst_n ]
  set ref_clock [ create_bd_port -dir I -type clk ref_clock ]
  set resetn_300 [ create_bd_port -dir I -type rst resetn_300 ]

  # Create instance: HiveNet_0h
  create_hier_cell_HiveNet_0h [current_bd_instance .] HiveNet_0h

  # Create instance: axi_address_remap_0, and set properties
  set block_name axi_address_remap
  set block_cell_name axi_address_remap_0
  if { [catch {set axi_address_remap_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $axi_address_remap_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: axi_interconnect_0, and set properties
  set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {3} \
 ] $axi_interconnect_0

  # Create instance: axi_register_slice_0, and set properties
  set axi_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_0 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {9} \
   CONFIG.PROTOCOL {AXI4LITE} \
   CONFIG.REG_AR {7} \
   CONFIG.REG_AW {7} \
   CONFIG.REG_B {7} \
   CONFIG.REG_R {7} \
   CONFIG.REG_W {7} \
 ] $axi_register_slice_0

  # Create instance: axi_register_slice_1, and set properties
  set axi_register_slice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_1 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.DATA_WIDTH {256} \
   CONFIG.NUM_WRITE_OUTSTANDING {256} \
   CONFIG.READ_WRITE_MODE {WRITE_ONLY} \
   CONFIG.REG_AW {1} \
   CONFIG.REG_B {1} \
 ] $axi_register_slice_1

  # Create instance: axi_register_slice_2, and set properties
  set axi_register_slice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_2 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.DATA_WIDTH {256} \
   CONFIG.NUM_WRITE_OUTSTANDING {256} \
   CONFIG.READ_WRITE_MODE {WRITE_ONLY} \
   CONFIG.REG_AW {1} \
   CONFIG.REG_B {1} \
 ] $axi_register_slice_2

  # Create instance: axi_register_slice_3, and set properties
  set axi_register_slice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_3 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.DATA_WIDTH {256} \
   CONFIG.NUM_WRITE_OUTSTANDING {0} \
   CONFIG.READ_WRITE_MODE {READ_ONLY} \
   CONFIG.REG_AR {1} \
   CONFIG.REG_AW {7} \
   CONFIG.REG_B {7} \
 ] $axi_register_slice_3

  # Create instance: axi_register_slice_4, and set properties
  set axi_register_slice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_4 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {34} \
   CONFIG.DATA_WIDTH {256} \
   CONFIG.NUM_WRITE_OUTSTANDING {0} \
   CONFIG.READ_WRITE_MODE {READ_ONLY} \
   CONFIG.REG_AR {1} \
   CONFIG.REG_AW {7} \
   CONFIG.REG_B {7} \
 ] $axi_register_slice_4

  # Create instance: axis_clock_converter_0, and set properties
  set axis_clock_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_0 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {3} \
 ] $axis_clock_converter_0

  # Create instance: axis_clock_converter_1, and set properties
  set axis_clock_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_1 ]
  set_property -dict [ list \
   CONFIG.SYNCHRONIZATION_STAGES {3} \
 ] $axis_clock_converter_1

  # Create instance: axis_register_slice_0, and set properties
  set axis_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_0 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_0

  # Create instance: axis_register_slice_1, and set properties
  set axis_register_slice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_1 ]
  set_property -dict [ list \
   CONFIG.NUM_SLR_CROSSINGS {0} \
   CONFIG.REG_CONFIG {12} \
 ] $axis_register_slice_1

  # Create instance: axis_register_slice_2, and set properties
  set axis_register_slice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_2 ]
  set_property -dict [ list \
   CONFIG.REG_CONFIG {8} \
 ] $axis_register_slice_2

  # Create instance: axis_register_slice_3, and set properties
  set axis_register_slice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_register_slice_3 ]
  set_property -dict [ list \
   CONFIG.NUM_SLR_CROSSINGS {0} \
   CONFIG.REG_CONFIG {12} \
 ] $axis_register_slice_3

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.CLKOUT1_JITTER {112.733} \
   CONFIG.CLKOUT1_PHASE_ERROR {91.100} \
   CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {150} \
   CONFIG.CLKOUT2_JITTER {104.670} \
   CONFIG.CLKOUT2_PHASE_ERROR {91.100} \
   CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {225} \
   CONFIG.CLKOUT2_USED {true} \
   CONFIG.MMCM_CLKFBOUT_MULT_F {11.250} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {7.500} \
   CONFIG.MMCM_CLKOUT1_DIVIDE {5} \
   CONFIG.NUM_OUT_CLKS {2} \
   CONFIG.PRIM_IN_FREQ {100.000} \
   CONFIG.USE_LOCKED {false} \
   CONFIG.USE_RESET {false} \
 ] $clk_wiz_0

  # Create instance: ethernet_stack_0
  create_hier_cell_ethernet_stack_0 [current_bd_instance .] ethernet_stack_0

  # Create instance: proc_sys_reset_150, and set properties
  set proc_sys_reset_150 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_150 ]

  # Create instance: proc_sys_reset_225, and set properties
  set proc_sys_reset_225 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_225 ]

  # Create instance: registers_0
  create_hier_cell_registers_0 [current_bd_instance .] registers_0

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  puts "#################################################"
  puts $HBM_BANK
  puts "#################################################"

  if { $HBM_BANK == 0 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x0} CONFIG.CONST_WIDTH {34} ] $xlconstant_0
} elseif { $HBM_BANK == 1 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x20000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0
} elseif { $HBM_BANK == 2 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x40000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0
} elseif { $HBM_BANK == 3 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x60000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0
} elseif { $HBM_BANK == 4 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x80000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0
} elseif { $HBM_BANK == 5 } {
	set_property -dict [ list CONFIG.CONST_VAL {0xa0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0
} elseif { $HBM_BANK == 6 } {
	set_property -dict [ list CONFIG.CONST_VAL {0xc0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 7 } {
	set_property -dict [ list CONFIG.CONST_VAL {0xe0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 8 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x100000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 9 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x120000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 10 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x140000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 11 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x160000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 12 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x180000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 13 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x1a0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 14 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x1c0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 15 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x1e0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 16 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x200000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 17 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x220000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 18 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x240000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 19 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x260000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 20 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x280000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 21 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x2a0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 22 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x2c0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 23 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x2e0000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 24 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x300000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 25 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x320000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 26 } {
	set_property -dict [ list CONFIG.CONST_VAL {0x340000000} CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 27 } {
	set_property -dict [ list    CONFIG.CONST_VAL {0x360000000}    CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 28 } {
	set_property -dict [ list    CONFIG.CONST_VAL {0x380000000}    CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} elseif { $HBM_BANK == 29 } {
	set_property -dict [ list  CONFIG.CONST_VAL {0x3a0000000}  CONFIG.CONST_WIDTH {34} ] $xlconstant_0

} else {
  set_property -dict [ list CONFIG.CONST_VAL {0x0} CONFIG.CONST_WIDTH {34}  ] $xlconstant_0
}

  # Create instance: xpm_cdc_gen_0, and set properties
  set xpm_cdc_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_0 ]
  set_property -dict [ list \
   CONFIG.WIDTH {32} \
 ] $xpm_cdc_gen_0

  # Create interface connections
  connect_bd_intf_net -intf_net HiveNet_0_tx_out [get_bd_intf_pins HiveNet_0h/tx_out] [get_bd_intf_pins axis_register_slice_3/S_AXIS]
  connect_bd_intf_net -intf_net HiveNet_0h_M_AXIS_0 [get_bd_intf_pins HiveNet_0h/M_AXIS_0] [get_bd_intf_pins axis_clock_converter_1/S_AXIS]
  connect_bd_intf_net -intf_net HiveNet_0h_M_AXI_1 [get_bd_intf_pins HiveNet_0h/M_AXI_1] [get_bd_intf_pins axi_register_slice_3/S_AXI]
  connect_bd_intf_net -intf_net HiveNet_0h_m_axi_HBM [get_bd_intf_pins HiveNet_0h/m_axi_HBM] [get_bd_intf_pins axi_register_slice_2/S_AXI]
  connect_bd_intf_net -intf_net S_AXIS_0_1 [get_bd_intf_ports rx] [get_bd_intf_pins axis_register_slice_0/S_AXIS]
  connect_bd_intf_net -intf_net S_AXI_0_1 [get_bd_intf_ports S00_AXI_0] [get_bd_intf_pins axi_register_slice_0/S_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_interconnect_0/M00_AXI] [get_bd_intf_pins registers_0/s_axi_AXILiteS_0]
  connect_bd_intf_net -intf_net axi_interconnect_0_M01_AXI [get_bd_intf_pins HiveNet_0h/S_AXI_0] [get_bd_intf_pins axi_interconnect_0/M01_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M02_AXI [get_bd_intf_pins axi_interconnect_0/M02_AXI] [get_bd_intf_pins registers_0/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_0_M_AXI [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins axi_register_slice_0/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_1_M_AXI [get_bd_intf_ports HBM_write] [get_bd_intf_pins axi_register_slice_1/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_2_M_AXI [get_bd_intf_pins axi_register_slice_1/S_AXI] [get_bd_intf_pins axi_register_slice_2/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_3_M_AXI [get_bd_intf_pins axi_register_slice_3/M_AXI] [get_bd_intf_pins axi_register_slice_4/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_4_M_AXI [get_bd_intf_ports HBM_read] [get_bd_intf_pins axi_register_slice_4/M_AXI]
  connect_bd_intf_net -intf_net axis_clock_converter_0_M_AXIS [get_bd_intf_pins HiveNet_0h/inputData] [get_bd_intf_pins axis_clock_converter_0/M_AXIS]
  connect_bd_intf_net -intf_net axis_clock_converter_1_M_AXIS [get_bd_intf_ports outData] [get_bd_intf_pins axis_clock_converter_1/M_AXIS]
  connect_bd_intf_net -intf_net axis_register_slice_0_M_AXIS [get_bd_intf_pins axis_register_slice_0/M_AXIS] [get_bd_intf_pins ethernet_stack_0/ethernet_rx_0]
  connect_bd_intf_net -intf_net axis_register_slice_2_M_AXIS [get_bd_intf_ports tx] [get_bd_intf_pins axis_register_slice_2/M_AXIS]
  connect_bd_intf_net -intf_net ethernet_stack_0_rdma_rx [get_bd_intf_pins axis_register_slice_1/S_AXIS] [get_bd_intf_pins ethernet_stack_0/rdma_rx]
  connect_bd_intf_net -intf_net ethernet_stack_0_tx_ethernet_0 [get_bd_intf_pins axis_register_slice_2/S_AXIS] [get_bd_intf_pins ethernet_stack_0/tx_ethernet_0]
  connect_bd_intf_net -intf_net inputData_0_1 [get_bd_intf_ports inputData] [get_bd_intf_pins axis_clock_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net rx_i_1 [get_bd_intf_pins HiveNet_0h/rx_i] [get_bd_intf_pins axis_register_slice_1/M_AXIS]
  connect_bd_intf_net -intf_net tx_rdma_0_1 [get_bd_intf_pins axis_register_slice_3/M_AXIS] [get_bd_intf_pins ethernet_stack_0/tx_rdma_0]

  # Create port connections
  connect_bd_net -net HiveNet_0h_m_axis_tdata [get_bd_pins HiveNet_0h/m_axis_tdata] [get_bd_pins registers_0/writePSN_in_V]
  connect_bd_net -net HiveNet_0h_my_ip [get_bd_pins HiveNet_0h/my_ip] [get_bd_pins xpm_cdc_gen_0/src_in]
  connect_bd_net -net HiveNet_0h_readPSN_l_V_TDATA [get_bd_pins HiveNet_0h/readPSN_l_V_TDATA] [get_bd_pins registers_0/readPSN_in_V]
  connect_bd_net -net HiveNet_0h_size_o_V_TDATA_0 [get_bd_pins HiveNet_0h/size_o_V_TDATA_0] [get_bd_pins registers_0/size]
  connect_bd_net -net M00_ARESETN_0_1 [get_bd_pins HiveNet_0h/s_axis_aresetn_0] [get_bd_pins axi_interconnect_0/M00_ARESETN] [get_bd_pins axi_interconnect_0/M02_ARESETN] [get_bd_pins axis_clock_converter_0/m_axis_aresetn] [get_bd_pins axis_clock_converter_1/s_axis_aresetn] [get_bd_pins proc_sys_reset_150/peripheral_aresetn] [get_bd_pins registers_0/ap_rst_n]
  connect_bd_net -net ap_rst_n_0_1 [get_bd_pins HiveNet_0h/ap_rst_n_0] [get_bd_pins proc_sys_reset_225/peripheral_aresetn]
  connect_bd_net -net axi_address_remap_0_m_hbm_araddr [get_bd_pins axi_address_remap_0/m_hbm_araddr] [get_bd_pins axi_register_slice_4/s_axi_araddr]
  connect_bd_net -net axi_address_remap_0_m_hbm_awaddr [get_bd_pins axi_address_remap_0/m_hbm_awaddr] [get_bd_pins axi_register_slice_1/s_axi_awaddr]
  connect_bd_net -net axi_register_slice_2_m_axi_awaddr [get_bd_pins axi_address_remap_0/s_hbm_awaddr] [get_bd_pins axi_register_slice_2/m_axi_awaddr]
  connect_bd_net -net axi_register_slice_3_m_axi_araddr [get_bd_pins axi_address_remap_0/s_hbm_araddr] [get_bd_pins axi_register_slice_3/m_axi_araddr]
  connect_bd_net -net clk_in1_0_1 [get_bd_ports ap_clk_300] [get_bd_pins HiveNet_0h/m_axis_aclk1] [get_bd_pins axi_interconnect_0/M01_ACLK] [get_bd_pins axi_register_slice_1/aclk] [get_bd_pins axi_register_slice_2/aclk] [get_bd_pins axi_register_slice_3/aclk] [get_bd_pins axi_register_slice_4/aclk] [get_bd_pins axis_register_slice_0/aclk] [get_bd_pins axis_register_slice_1/aclk] [get_bd_pins axis_register_slice_2/aclk] [get_bd_pins axis_register_slice_3/aclk] [get_bd_pins ethernet_stack_0/ap_clk_0] [get_bd_pins registers_0/clk_in_300] [get_bd_pins xpm_cdc_gen_0/dest_clk]
  connect_bd_net -net clk_in1_0_2 [get_bd_ports ref_clock] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins HiveNet_0h/s_axis_aclk_0] [get_bd_pins axi_interconnect_0/M00_ACLK] [get_bd_pins axi_interconnect_0/M02_ACLK] [get_bd_pins axis_clock_converter_0/m_axis_aclk] [get_bd_pins axis_clock_converter_1/s_axis_aclk] [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins proc_sys_reset_150/slowest_sync_clk] [get_bd_pins registers_0/clk] [get_bd_pins xpm_cdc_gen_0/src_clk]
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins HiveNet_0h/m_axis_aclk] [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins proc_sys_reset_225/slowest_sync_clk]
  connect_bd_net -net ip_subnet_1 [get_bd_pins HiveNet_0h/ip_subnet] [get_bd_pins registers_0/subnet]
  connect_bd_net -net local_ID_1 [get_bd_pins HiveNet_0h/local_ID] [get_bd_pins registers_0/local_ID]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_ports resetn_300] [get_bd_pins HiveNet_0h/aresetn] [get_bd_pins axi_interconnect_0/M01_ARESETN] [get_bd_pins axi_register_slice_1/aresetn] [get_bd_pins axi_register_slice_2/aresetn] [get_bd_pins axi_register_slice_3/aresetn] [get_bd_pins axi_register_slice_4/aresetn] [get_bd_pins axis_register_slice_0/aresetn] [get_bd_pins axis_register_slice_1/aresetn] [get_bd_pins axis_register_slice_2/aresetn] [get_bd_pins axis_register_slice_3/aresetn] [get_bd_pins ethernet_stack_0/ap_rst_n_0]
  connect_bd_net -net registers_0_Ntranssmisions [get_bd_pins HiveNet_0h/nRetransmissions] [get_bd_pins registers_0/Ntranssmisions]
  connect_bd_net -net registers_0_arp_clean [get_bd_pins ethernet_stack_0/arp_clear_n_V_0] [get_bd_pins registers_0/arp_clean]
  connect_bd_net -net registers_0_dest_out [get_bd_pins ethernet_stack_0/id_src_V] [get_bd_pins registers_0/local_id_300]
  connect_bd_net -net registers_0_my_mac [get_bd_pins ethernet_stack_0/myMacAddress_V_0] [get_bd_pins registers_0/my_mac]
  connect_bd_net -net registers_0_packet_port [get_bd_pins ethernet_stack_0/roce_port_V_0] [get_bd_pins registers_0/packet_port_300]
  connect_bd_net -net registers_0_timeout [get_bd_pins HiveNet_0h/timeoutInNs] [get_bd_pins registers_0/timeout]
  connect_bd_net -net s_axis_aclk_0_1 [get_bd_ports ap_clk] [get_bd_pins axi_interconnect_0/ACLK] [get_bd_pins axi_interconnect_0/S00_ACLK] [get_bd_pins axi_register_slice_0/aclk] [get_bd_pins axis_clock_converter_0/s_axis_aclk] [get_bd_pins axis_clock_converter_1/m_axis_aclk]
  connect_bd_net -net s_axis_aresetn_0_1 [get_bd_ports ap_rst_n] [get_bd_pins axi_interconnect_0/ARESETN] [get_bd_pins axi_interconnect_0/S00_ARESETN] [get_bd_pins axi_register_slice_0/aresetn] [get_bd_pins axis_clock_converter_0/s_axis_aresetn] [get_bd_pins axis_clock_converter_1/m_axis_aresetn] [get_bd_pins proc_sys_reset_150/ext_reset_in] [get_bd_pins proc_sys_reset_225/ext_reset_in]
  connect_bd_net -net udp_port_1 [get_bd_pins HiveNet_0h/udp_port] [get_bd_pins registers_0/packet_port]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins axi_address_remap_0/start_address] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xpm_cdc_gen_0_dest_out [get_bd_pins ethernet_stack_0/myIpAddress_V_0] [get_bd_pins xpm_cdc_gen_0/dest_out]

  # Create address segments
  assign_bd_address -offset 0x00000000 -range 0x000400000000 -target_address_space [get_bd_addr_spaces HiveNet_0h/hier_0/asynchronousWriteToH_0/Data_m_axi_HBM] [get_bd_addr_segs HBM_write/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x000400000000 -target_address_space [get_bd_addr_spaces HiveNet_0h/hier_0/tx_checkTimeout_dev_0/Data_m_axi_header] [get_bd_addr_segs HBM_read/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x000400000000 -target_address_space [get_bd_addr_spaces HiveNet_0h/hier_0/tx_checkcurrent_data_0/Data_m_axi_HBM_a] [get_bd_addr_segs HBM_read/Reg] -force
  assign_bd_address -offset 0x00000080 -range 0x00000080 -target_address_space [get_bd_addr_spaces S00_AXI_0] [get_bd_addr_segs HiveNet_0h/axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x00000100 -range 0x00000080 -target_address_space [get_bd_addr_spaces S00_AXI_0] [get_bd_addr_segs registers_0/axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00000080 -target_address_space [get_bd_addr_spaces S00_AXI_0] [get_bd_addr_segs registers_0/register_map_full_0/s_axi_AXILiteS/Reg] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design "" $HBM_BANK


