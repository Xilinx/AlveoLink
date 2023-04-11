#   Copyright (c) 2021, Xilinx, Inc.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are met:
#
#   1.  Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#
#   2.  Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#   3.  Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived from
#       this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION). HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


###### Create Block Design ######
set design_name "cmac_bd"

create_bd_design ${design_name}
open_bd_design ${design_name}


##### Create and configure CMAC IP #####

# Default GT reference frequency
set gt_ref_clk 156.25
set freerunningclock 100
if {${projPart} eq "xcu50-fsvh2104-2L-e"} {
  # Possible core_selection CMACE4_X0Y3 and CMACE4_X0Y4
  set gt_ref_clk 161.1328125
  set core_selection  CMACE4_X0Y3
  set group_selection X0Y28~X0Y31
  set interface_number 0
} elseif {${projPart} eq "xcu55c-fsvh2892-2L-e"} {
  set gt_ref_clk 161.1328125
  switch ${interface} {
    "1" {
      # Possible core_selection CMACE4_X0Y3 and CMACE4_X0Y4
      set core_selection  CMACE4_X0Y4
      set group_selection X0Y28~X0Y31
      set interface_number 1
    }
    default {
      # Possible core_selection CMACE4_X0Y2; CMACE4_X0Y3; CMACE4_X0Y4
      set core_selection  CMACE4_X0Y2
      set group_selection X0Y24~X0Y27
      set interface_number 0
    }
  }
} elseif {${projPart} eq "xcu200-fsgd2104-2-e"} {
  switch ${interface} {
    "1" {
      # Possible core_selection CMACE4_X0Y6 and CMACE4_X0Y7
      set core_selection  CMACE4_X0Y6
      set group_selection X1Y44~X1Y47
      set interface_number 1
    }
    default {
      # Possible core_selection CMACE4_X0Y6; CMACE4_X0Y7 and CMACE4_X0Y8
      set core_selection  CMACE4_X0Y8
      set group_selection X1Y48~X1Y51
      set interface_number 0
    }
  }
} elseif {${projPart} eq "xcu250-figd2104-2L-e"} {
  switch ${interface} {
    "1" {
      # Possible core_selection CMACE4_X0Y6; CMACE4_X0Y7 and CMACE4_X0Y8
      set core_selection  CMACE4_X0Y6
      set group_selection X1Y40~X1Y43
      set interface_number 1
    }
    default {
      # Possible core_selection CMACE4_X0Y7 and CMACE4_X0Y8
      set core_selection  CMACE4_X0Y7
      set group_selection X1Y44~X1Y47
      set interface_number 0
    }
  }
} elseif {${projPart} eq "xcu280-fsvh2892-2L-e"} {
  set freerunningclock 50
  switch ${interface} {
    "1" {
      # Possible core_selection CMACE4_X0Y6 and CMACE4_X0Y7
      set core_selection  CMACE4_X0Y6
      set group_selection X0Y44~X0Y47
      set interface_number 1
    }
    default {
      # Possible core_selection CMACE4_X0Y5; CMACE4_X0Y6 and CMACE4_X0Y7
      set core_selection  CMACE4_X0Y5
      set group_selection X0Y40~X0Y43
      set interface_number 0
    }
  }
} else {
  puts "unknown part"
  return -1
}




set list_check_ips "\ 
xilinx.com:ip:axis_clock_converter:1.1\
xilinx.com:ip:axi_gpio:2.0\
xilinx.com:ip:axis_data_fifo:2.0\
grovf.com:libhivenet:cmac_flowcontrol_rx_dev:1.0\
grovf.com:libhivenet:cmac_flowcontrol_tx_dev:1.0\
xilinx.com:ip:cmac_usplus:3.1\
xilinx.com:ip:fifo_generator:13.2\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:smartconnect:1.0\
xilinx.com:ip:util_vector_logic:2.0\
xilinx.com:ip:xlconstant:1.1\
xilinx.com:ip:xpm_cdc_gen:1.0\
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


set list_check_mods "\ 
cmac_sync\
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

# Create instance: xlconstant_2, and set properties
set xlconstant_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_2 ]
set_property -dict [ list \
  CONFIG.CONST_VAL {0x0000FFFF} \
  CONFIG.CONST_WIDTH {32} \
] $xlconstant_2

# Create instance: xlconstant_3, and set properties
set xlconstant_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_3 ]
set_property -dict [ list \
  CONFIG.CONST_VAL {0} \
  CONFIG.CONST_WIDTH {32} \
] $xlconstant_3

# Create instance: axis_data_fifo_0, and set properties
set axis_data_fifo_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_data_fifo:2.0 axis_data_fifo_0 ]
set_property -dict [ list \
  CONFIG.FIFO_DEPTH {8192} \
  CONFIG.FIFO_MEMORY_TYPE {ultra} \
  CONFIG.HAS_TKEEP {1} \
  CONFIG.HAS_TLAST {1} \
  CONFIG.HAS_WR_DATA_COUNT {1} \
  CONFIG.TDATA_NUM_BYTES {64} \
] $axis_data_fifo_0

# Create instance: cmac_flowcontrol_rx_0, and set properties
set cmac_flowcontrol_rx_0 [ create_bd_cell -type ip -vlnv grovf.com:libhivenet:cmac_flowcontrol_rx_dev:1.0 cmac_flowcontrol_rx_0 ]


set cmac_name cmac_uplus_${interface_number}
set cmac_rx_clk cmac_bd_cmac_uplus_${interface_number}_0_gt_rxusrclk2
set gt_clk_freq [expr int(${gt_ref_clk} * 1000000)]
puts "Generating IPI for ${cmac_name} with GT clock running at ${gt_clk_freq} Hz"

set cmac_ip [create_bd_cell -type ip -vlnv xilinx.com:ip:cmac_usplus ${cmac_name}]
set_property -dict [ list \
  CONFIG.CMAC_CAUI4_MODE             {1} \
  CONFIG.NUM_LANES                   {4x25} \
  CONFIG.GT_REF_CLK_FREQ             $gt_ref_clk \
  CONFIG.CMAC_CORE_SELECT            $core_selection \
  CONFIG.GT_GROUP_SELECT             $group_selection \
  CONFIG.GT_DRP_CLK                  $freerunningclock \
  CONFIG.USER_INTERFACE              {AXIS} \
  CONFIG.INCLUDE_SHARED_LOGIC        {2} \
  CONFIG.LANE5_GT_LOC                {NA} \
  CONFIG.LANE6_GT_LOC                {NA} \
  CONFIG.LANE7_GT_LOC                {NA} \
  CONFIG.LANE8_GT_LOC                {NA} \
  CONFIG.LANE9_GT_LOC                {NA} \
  CONFIG.LANE10_GT_LOC               {NA} \
  CONFIG.OPERATING_MODE              {Duplex} \
  CONFIG.TX_FLOW_CONTROL             {1} \
  CONFIG.RX_FLOW_CONTROL             {1} \
  CONFIG.ENABLE_AXI_INTERFACE        {1} \
  CONFIG.INCLUDE_STATISTICS_COUNTERS {1} \
  CONFIG.RX_CHECK_ACK                {0} \
  CONFIG.ENABLE_TIME_STAMPING        {0} \
  CONFIG.TX_PTP_1STEP_ENABLE         {2} \
  CONFIG.PTP_TRANSPCLK_MODE          {0} \
  CONFIG.TX_PTP_LATENCY_ADJUST       {0} \
  CONFIG.ENABLE_PIPELINE_REG         {1} \
  CONFIG.INCLUDE_RS_FEC              {1} \
] $cmac_ip

###### Create interface ports ######

set clk_gt_freerun [ create_bd_port -dir I -type clk -freq_hz [expr ${freerunningclock} * 1000000] clk_gt_freerun ]
    

set M_AXIS [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M_AXIS ]
set_property -dict [ list \
  CONFIG.FREQ_HZ {300000000} \
] $M_AXIS

set S_AXILITE [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXILITE ]
set_property -dict [ list \
  CONFIG.ADDR_WIDTH {14} \
  CONFIG.ARUSER_WIDTH {0} \
  CONFIG.AWUSER_WIDTH {0} \
  CONFIG.BUSER_WIDTH {0} \
  CONFIG.DATA_WIDTH {32} \
  CONFIG.FREQ_HZ {300000000} \
  CONFIG.HAS_BRESP {1} \
  CONFIG.HAS_BURST {0} \
  CONFIG.HAS_CACHE {0} \
  CONFIG.HAS_LOCK {0} \
  CONFIG.HAS_PROT {0} \
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
] $S_AXILITE

set S_AXIS [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS ]
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
  CONFIG.TUSER_WIDTH {16} \
] $S_AXIS

set gt_ref_clk [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 gt_ref_clk ]
set_property -dict [ list \
  CONFIG.FREQ_HZ $gt_clk_freq \
] $gt_ref_clk

set gt_serial_port [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gt_rtl:1.0 gt_serial_port ]


# Create ports
set ap_clk [ create_bd_port -dir I -type clk -freq_hz 300000000 ap_clk ]
set_property -dict [ list \
  CONFIG.ASSOCIATED_BUSIF {S_AXIS:M_AXIS:S_AXILITE} \
  CONFIG.ASSOCIATED_RESET {ap_rst_n} \
] $ap_clk
set ap_rst_n [ create_bd_port -dir I -type rst ap_rst_n ]
set_property -dict [ list \
  CONFIG.POLARITY {ACTIVE_LOW} \
] $ap_rst_n

###### Add IP ######

# Create instance: acc_kernel_tx_cdc, and set properties
set acc_kernel_tx_cdc [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter acc_kernel_tx_cdc ]
set_property -dict [ list \
  CONFIG.HAS_TKEEP {1} \
  CONFIG.HAS_TLAST {1} \
  CONFIG.IS_ACLK_ASYNC {1} \
  CONFIG.TDATA_NUM_BYTES {64} \
  CONFIG.TUSER_WIDTH.VALUE_SRC USER \
  CONFIG.TUSER_WIDTH {0} \
] $acc_kernel_tx_cdc

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_ALL_OUTPUTS {1} \
   CONFIG.C_DOUT_DEFAULT {0x00000100} \
   CONFIG.C_GPIO_WIDTH {9} \
 ] $axi_gpio_0

  # Create instance: cmac_flowcontrol_tx_0, and set properties
  set cmac_flowcontrol_tx_0 [ create_bd_cell -type ip -vlnv grovf.com:libhivenet:cmac_flowcontrol_tx_dev:1.0 cmac_flowcontrol_tx_0 ]

# Create instance: fifo_cmac_rx_cdc, and set properties
set fifo_cmac_rx_cdc [ create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator fifo_cmac_rx_cdc ]
set_property -dict [ list \
  CONFIG.Clock_Type_AXI {Independent_Clock} \
  CONFIG.Empty_Threshold_Assert_Value_axis {509} \
  CONFIG.Empty_Threshold_Assert_Value_rach {13} \
  CONFIG.Empty_Threshold_Assert_Value_rdch {1018} \
  CONFIG.Empty_Threshold_Assert_Value_wach {13} \
  CONFIG.Empty_Threshold_Assert_Value_wdch {1018} \
  CONFIG.Empty_Threshold_Assert_Value_wrch {13} \
  CONFIG.Enable_TLAST {true} \
  CONFIG.FIFO_Implementation_axis {Independent_Clocks_Block_RAM} \
  CONFIG.FIFO_Implementation_rach {Independent_Clocks_Distributed_RAM} \
  CONFIG.FIFO_Implementation_rdch {Independent_Clocks_Builtin_FIFO} \
  CONFIG.FIFO_Implementation_wach {Independent_Clocks_Distributed_RAM} \
  CONFIG.FIFO_Implementation_wdch {Independent_Clocks_Builtin_FIFO} \
  CONFIG.FIFO_Implementation_wrch {Independent_Clocks_Distributed_RAM} \
  CONFIG.Full_Threshold_Assert_Value_axis {511} \
  CONFIG.Full_Threshold_Assert_Value_rach {15} \
  CONFIG.Full_Threshold_Assert_Value_wach {15} \
  CONFIG.Full_Threshold_Assert_Value_wrch {15} \
  CONFIG.HAS_TKEEP {true} \
  CONFIG.INTERFACE_TYPE {AXI_STREAM} \
  CONFIG.Input_Depth_axis {512} \
  CONFIG.Reset_Type {Asynchronous_Reset} \
  CONFIG.TDATA_NUM_BYTES {64} \
  CONFIG.TKEEP_WIDTH {64} \
  CONFIG.TSTRB_WIDTH {64} \
  CONFIG.TUSER_WIDTH {0} \
] $fifo_cmac_rx_cdc

# Create instance: fifo_cmac_tx, and set properties
set fifo_cmac_tx [ create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator fifo_cmac_tx ]
set_property -dict [ list \
  CONFIG.Clock_Type_AXI {Common_Clock} \
  CONFIG.Empty_Threshold_Assert_Value_axis {510} \
  CONFIG.Empty_Threshold_Assert_Value_rach {14} \
  CONFIG.Empty_Threshold_Assert_Value_rdch {1022} \
  CONFIG.Empty_Threshold_Assert_Value_wach {14} \
  CONFIG.Empty_Threshold_Assert_Value_wdch {1022} \
  CONFIG.Empty_Threshold_Assert_Value_wrch {14} \
  CONFIG.Enable_TLAST {true} \
  CONFIG.FIFO_Application_Type_axis {Packet_FIFO} \
  CONFIG.FIFO_Implementation_axis {Common_Clock_Block_RAM} \
  CONFIG.FIFO_Implementation_rach {Common_Clock_Distributed_RAM} \
  CONFIG.FIFO_Implementation_rdch {Common_Clock_Builtin_FIFO} \
  CONFIG.FIFO_Implementation_wach {Common_Clock_Distributed_RAM} \
  CONFIG.FIFO_Implementation_wdch {Common_Clock_Builtin_FIFO} \
  CONFIG.FIFO_Implementation_wrch {Common_Clock_Distributed_RAM} \
  CONFIG.Full_Threshold_Assert_Value_axis {511} \
  CONFIG.Full_Threshold_Assert_Value_rach {15} \
  CONFIG.Full_Threshold_Assert_Value_wach {15} \
  CONFIG.Full_Threshold_Assert_Value_wrch {15} \
  CONFIG.HAS_TKEEP {true} \
  CONFIG.INTERFACE_TYPE {AXI_STREAM} \
  CONFIG.Input_Depth_axis {512} \
  CONFIG.Reset_Type {Asynchronous_Reset} \
  CONFIG.TDATA_NUM_BYTES {64} \
  CONFIG.TKEEP_WIDTH {64} \
  CONFIG.TSTRB_WIDTH {64} \
  CONFIG.TUSER_WIDTH {0} \
] $fifo_cmac_tx

# Create instance: cmac_sync, and set properties
set cmac_sync [ create_bd_cell -type module -reference cmac_sync cmac_sync ]
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]


# Create instance: util_vector_logic_0, and set properties
set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_0 ]
set_property -dict [ list \
  CONFIG.C_OPERATION {not} \
  CONFIG.C_SIZE {1} \
  CONFIG.LOGO_FILE {data/sym_notgate.png} \
] $util_vector_logic_0

# Create instance: util_vector_logic_1, and set properties
set util_vector_logic_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_1 ]
set_property -dict [ list \
  CONFIG.C_OPERATION {not} \
  CONFIG.C_SIZE {1} \
  CONFIG.LOGO_FILE {data/sym_notgate.png} \
] $util_vector_logic_1

# Create instance: util_vector_logic_2, and set properties
set util_vector_logic_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_2 ]
set_property -dict [ list \
  CONFIG.C_OPERATION {not} \
  CONFIG.C_SIZE {1} \
  CONFIG.LOGO_FILE {data/sym_notgate.png} \
] $util_vector_logic_2

# Create instance: xlconstant_0, and set properties
set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant: xlconstant_0 ]
set_property -dict [ list \
  CONFIG.CONST_VAL {0} \
] $xlconstant_0

   # Create instance: xlconstant_5, and set properties
  set xlconstant_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_5 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $xlconstant_5

  # Create instance: smartconnect, and set properties
  set smartconnect [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smartconnect ]
  set_property -dict [ list \
   CONFIG.NUM_CLKS {2} \
   CONFIG.NUM_MI {2} \
   CONFIG.NUM_SI {2} \
 ] $smartconnect

  # Create instance: xpm_cdc_gen_0, and set properties
  set xpm_cdc_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_0 ]
  set_property -dict [ list \
   CONFIG.DEST_SYNC_FF {2} \
   CONFIG.WIDTH {32} \
 ] $xpm_cdc_gen_0

   # Create instance: xpm_cdc_gen_1, and set properties
  set xpm_cdc_gen_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_1 ]
  set_property -dict [ list \
   CONFIG.DEST_SYNC_FF {5} \
   CONFIG.WIDTH {9} \
 ] $xpm_cdc_gen_1

   # Create instance: xpm_cdc_gen_2, and set properties
  set xpm_cdc_gen_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_2 ]
  set_property -dict [ list \
   CONFIG.DEST_SYNC_FF {5} \
   CONFIG.WIDTH {1} \
 ] $xpm_cdc_gen_2

  # Create instance: xpm_cdc_gen_3, and set properties
  set xpm_cdc_gen_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xpm_cdc_gen:1.0 xpm_cdc_gen_3 ]
  set_property -dict [ list \
   CONFIG.DEST_SYNC_FF {5} \
   CONFIG.INIT_SYNC_FF {false} \
   CONFIG.WIDTH {9} \
 ] $xpm_cdc_gen_3
 
   # Create interface connections
  connect_bd_intf_net -intf_net S_AXILITE_1 [get_bd_intf_ports S_AXILITE] [get_bd_intf_pins smartconnect/S00_AXI]
  connect_bd_intf_net -intf_net S_AXIS_1 [get_bd_intf_ports S_AXIS] [get_bd_intf_pins acc_kernel_tx_cdc/S_AXIS]
  connect_bd_intf_net -intf_net acc_kernel_tx_cdc_M_AXIS [get_bd_intf_pins acc_kernel_tx_cdc/M_AXIS] [get_bd_intf_pins cmac_flowcontrol_tx_0/tx_i]
  connect_bd_intf_net -intf_net axis_data_fifo_0_M_AXIS [get_bd_intf_ports M_AXIS] [get_bd_intf_pins axis_data_fifo_0/M_AXIS]
  connect_bd_intf_net -intf_net cmac_flowcontrol_tx_0_tx_o [get_bd_intf_pins cmac_flowcontrol_tx_0/tx_o] [get_bd_intf_pins fifo_cmac_tx/S_AXIS]
  connect_bd_intf_net -intf_net cmac_sync_s_axi [get_bd_intf_pins cmac_sync/s_axi] [get_bd_intf_pins smartconnect/S01_AXI]
  connect_bd_intf_net -intf_net ${cmac_name}_axis_rx [get_bd_intf_pins ${cmac_name}/axis_rx] [get_bd_intf_pins fifo_cmac_rx_cdc/S_AXIS]
  connect_bd_intf_net -intf_net ${cmac_name}_gt_serial_port [get_bd_intf_ports gt_serial_port] [get_bd_intf_pins ${cmac_name}/gt_serial_port]
  connect_bd_intf_net -intf_net fifo_cmac_rx_cdc_M_AXIS [get_bd_intf_pins axis_data_fifo_0/S_AXIS] [get_bd_intf_pins fifo_cmac_rx_cdc/M_AXIS]
  connect_bd_intf_net -intf_net fifo_cmac_tx_M_AXIS [get_bd_intf_pins ${cmac_name}/axis_tx] [get_bd_intf_pins fifo_cmac_tx/M_AXIS]
  connect_bd_intf_net -intf_net gt_ref_clk_1 [get_bd_intf_ports gt_ref_clk] [get_bd_intf_pins ${cmac_name}/gt_ref_clk]
  connect_bd_intf_net -intf_net smartconnect_M00_AXI [get_bd_intf_pins ${cmac_name}/s_axi] [get_bd_intf_pins smartconnect/M00_AXI]
  connect_bd_intf_net -intf_net smartconnect_M01_AXI [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins smartconnect/M01_AXI]

  # Create port connections
  connect_bd_net -net ap_rst_n [get_bd_ports ap_rst_n] [get_bd_pins acc_kernel_tx_cdc/s_axis_aresetn] [get_bd_pins axis_data_fifo_0/s_axis_aresetn] [get_bd_pins proc_sys_reset_0/ext_reset_in] [get_bd_pins smartconnect/aresetn] [get_bd_pins util_vector_logic_1/Op1]
  connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins axi_gpio_0/gpio_io_o] [get_bd_pins xpm_cdc_gen_3/src_in]
  connect_bd_net -net axis_data_fifo_0_axis_wr_data_count [get_bd_pins axis_data_fifo_0/axis_wr_data_count] [get_bd_pins xpm_cdc_gen_0/src_in]
  connect_bd_net -net clk_gt_freerun_net [get_bd_ports clk_gt_freerun] [get_bd_pins ${cmac_name}/init_clk]
  connect_bd_net -net cmac_flowcontrol_rx_0_cmac_tx_pause_req_o [get_bd_pins cmac_flowcontrol_rx_0/cmac_tx_pause_req_o] [get_bd_pins ${cmac_name}/ctl_tx_pause_req]
  connect_bd_net -net cmac_stat_rx_aligned [get_bd_pins cmac_sync/stat_rx_aligned] [get_bd_pins ${cmac_name}/stat_rx_aligned]
  connect_bd_net -net ${cmac_name}_stat_rx_pause [get_bd_pins ${cmac_name}/stat_rx_pause] [get_bd_pins xpm_cdc_gen_2/src_in]
  connect_bd_net -net ${cmac_name}_stat_rx_pause_req [get_bd_pins ${cmac_name}/stat_rx_pause_req] [get_bd_pins xpm_cdc_gen_1/src_in]
  connect_bd_net -net ${cmac_name}_usr_rx_clk [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins cmac_flowcontrol_rx_0/ap_clk] [get_bd_pins ${cmac_name}/gt_rxusrclk2] [get_bd_pins ${cmac_name}/rx_clk] [get_bd_pins fifo_cmac_rx_cdc/s_aclk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] [get_bd_pins xpm_cdc_gen_0/dest_clk] [get_bd_pins smartconnect/aclk1] [get_bd_pins xpm_cdc_gen_1/src_clk] [get_bd_pins xpm_cdc_gen_2/src_clk] [get_bd_pins xpm_cdc_gen_3/dest_clk] [get_bd_pins xpm_cdc_gen_3/src_clk]
  connect_bd_net -net ${cmac_name}_usr_tx_clk [get_bd_pins acc_kernel_tx_cdc/m_axis_aclk] [get_bd_pins cmac_flowcontrol_tx_0/ap_clk] [get_bd_pins ${cmac_name}/gt_txusrclk2] [get_bd_pins fifo_cmac_tx/s_aclk] [get_bd_pins xpm_cdc_gen_1/dest_clk] [get_bd_pins xpm_cdc_gen_2/dest_clk]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins proc_sys_reset_0/peripheral_aresetn]
  connect_bd_net -net proc_sys_reset_0_peripheral_reset [get_bd_pins cmac_flowcontrol_rx_0/ap_rst] [get_bd_pins proc_sys_reset_0/peripheral_reset]
  connect_bd_net -net s_aclk_0_1 [get_bd_ports ap_clk] [get_bd_pins acc_kernel_tx_cdc/s_axis_aclk] [get_bd_pins axis_data_fifo_0/s_axis_aclk] [get_bd_pins cmac_sync/s_axi_aclk] [get_bd_pins ${cmac_name}/s_axi_aclk] [get_bd_pins fifo_cmac_rx_cdc/m_aclk] [get_bd_pins smartconnect/aclk] [get_bd_pins xpm_cdc_gen_0/src_clk]
  connect_bd_net -net usr_rx_reset [get_bd_pins cmac_sync/usr_rx_reset] [get_bd_pins ${cmac_name}/usr_rx_reset] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net usr_tx_reset [get_bd_pins cmac_sync/usr_tx_reset] [get_bd_pins ${cmac_name}/usr_tx_reset] [get_bd_pins util_vector_logic_2/Op1]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins fifo_cmac_rx_cdc/s_aresetn] [get_bd_pins util_vector_logic_0/Res]
  connect_bd_net -net util_vector_logic_1_Res [get_bd_pins cmac_sync/s_axi_sreset] [get_bd_pins ${cmac_name}/s_axi_sreset] [get_bd_pins ${cmac_name}/sys_reset] [get_bd_pins util_vector_logic_1/Res]
  connect_bd_net -net util_vector_logic_2_Res [get_bd_pins acc_kernel_tx_cdc/m_axis_aresetn] [get_bd_pins cmac_flowcontrol_tx_0/ap_rst_n] [get_bd_pins fifo_cmac_tx/s_aresetn] [get_bd_pins util_vector_logic_2/Res]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins ${cmac_name}/core_drp_reset] [get_bd_pins ${cmac_name}/core_rx_reset] [get_bd_pins ${cmac_name}/core_tx_reset] [get_bd_pins ${cmac_name}/drp_clk] [get_bd_pins ${cmac_name}/gtwiz_reset_rx_datapath] [get_bd_pins ${cmac_name}/gtwiz_reset_tx_datapath] [get_bd_pins ${cmac_name}/pm_tick] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_2_dout [get_bd_pins cmac_flowcontrol_rx_0/fifo_rx_max] [get_bd_pins xlconstant_2/dout]
  connect_bd_net -net xlconstant_3_dout [get_bd_pins cmac_flowcontrol_rx_0/fifo_rx_min] [get_bd_pins xlconstant_3/dout]
  connect_bd_net -net xlconstant_5_dout [get_bd_pins cmac_flowcontrol_rx_0/drop_err] [get_bd_pins cmac_flowcontrol_rx_0/drop_nrdy] [get_bd_pins xlconstant_5/dout]
  connect_bd_net -net xpm_cdc_gen_0_dest_out [get_bd_pins cmac_flowcontrol_rx_0/fifo_rx_rd_data_count] [get_bd_pins xpm_cdc_gen_0/dest_out]
  connect_bd_net -net xpm_cdc_gen_1_dest_out [get_bd_pins cmac_flowcontrol_tx_0/cmac_rx_pause_req] [get_bd_pins xpm_cdc_gen_1/dest_out]
  connect_bd_net -net xpm_cdc_gen_2_dest_out [get_bd_pins cmac_flowcontrol_tx_0/stat_rx_pause] [get_bd_pins xpm_cdc_gen_2/dest_out]
  connect_bd_net -net xpm_cdc_gen_3_dest_out [get_bd_pins cmac_flowcontrol_rx_0/cmac_pfc_val] [get_bd_pins xpm_cdc_gen_3/dest_out]

  assign_bd_address -offset 0x00000000 -range 0x00001000 -target_address_space [get_bd_addr_spaces cmac_sync/s_axi] [get_bd_addr_segs ${cmac_name}/s_axi/Reg] -force
  assign_bd_address -offset 0x00002000 -range 0x00001000 -target_address_space [get_bd_addr_spaces S_AXILITE] [get_bd_addr_segs axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00002000 -target_address_space [get_bd_addr_spaces S_AXILITE] [get_bd_addr_segs ${cmac_name}/s_axi/Reg] -force

  # Exclude Address Segments
  exclude_bd_addr_seg -offset 0x00000000 -range 0x00001000 -target_address_space [get_bd_addr_spaces cmac_sync/s_axi] [get_bd_addr_segs axi_gpio_0/S_AXI/Reg]

###### Validate and save the IPI ######

startgroup
set_property -dict [list CONFIG.CONST_VAL {6000}] [get_bd_cells xlconstant_2]
endgroup
startgroup
set_property -dict [list CONFIG.CONST_VAL {3000}] [get_bd_cells xlconstant_3]
endgroup

validate_bd_design
save_bd_design