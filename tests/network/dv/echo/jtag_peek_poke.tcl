#set reg_field_data "ChangeMe"
#set reg_addr_data "ChangeMe"

set jtag_master hw_axi_1 

proc read { reg_addr } {
  global jtag_master
  set transaction "peek_tx"
  create_hw_axi_txn $transaction [ get_hw_axis $jtag_master ] -address $reg_addr -type read -force
  run_hw_axi $transaction -quiet
  set data [ get_property DATA  [get_hw_axi_txns $transaction]  ]
  puts "Read Data for $reg_addr=$data"
}

proc read_value { reg_addr } {
  global jtag_master
  set transaction "peek_tx"
  create_hw_axi_txn $transaction [ get_hw_axis $jtag_master ] -address $reg_addr -type read -force
  run_hw_axi $transaction -quiet
  set data [ get_property DATA  [get_hw_axi_txns $transaction]  ]
  return $data
}


proc write { reg_addr write_data  } {
  global jtag_master
  set transaction "peek_tx"
  create_hw_axi_txn $transaction [ get_hw_axis $jtag_master ] -address $reg_addr -data $write_data -type write -force
  run_hw_axi $transaction -quiet
  puts "Write Complete to $reg_addr"
}


proc plexus_peek { reg_name } {
  global jtag_master
  set transaction "peek_tx"
  set reg_data [ find_addr $reg_name ]
  
  if { [ llength $reg_data] == 3 } {
    puts "Reading [lindex $reg_data 0]: [lindex $reg_data 2] "
    create_hw_axi_txn $transaction [ get_hw_axis $jtag_master ] -address [lindex $reg_data 0] -type read -force
    run_hw_axi $transaction -quiet
    
    set data "0x[ get_property DATA  [get_hw_axi_txns $transaction]  ]"
    
    puts " Data=$data"
    parse_fields [lindex $reg_data 1] $data
  }
}
    
proc plexus_poke { reg_name write_data {parse_only ""} } {
  global jtag_master
  set transaction "peek_tx"
  set reg_data [ find_addr $reg_name ]
  
  if { [ llength $reg_data] == 3 } {
    puts "Writing [lindex $reg_data 0]: [lindex $reg_data 2] "
    parse_fields [lindex $reg_data 1] $write_data
    if { $parse_only == "" } {
       create_hw_axi_txn $transaction [ get_hw_axis $jtag_master ] -address [lindex $reg_data 0] -data $write_data -type write -force
       run_hw_axi $transaction -quiet
       puts "Write Complete to [lindex $reg_data 2]"
    }
  }
}

proc read_data { base_path } {
  global reg_addr_data
  global reg_field_data
  
  puts "base path = $base_path"
  
  puts "Opening: $base_path/fpga_registers_field.csv"
  set f [open "$base_path/fpga_registers_field.csv" r]
  puts "Opened: $base_path/fpga_registers_field.csv"
  set reg_field_data [split [read $f] "\n" ]
  close $f
  puts "Closing: $base_path/fpga_registers_field.csv"

  set f [open "$base_path/fpga_registers_addr.csv" r]
  set reg_addr_data [split [read $f] "\n" ]
  close $f
}

# Takes a string representation of a register, and parses the register spreadsheet CSV to find it's address
proc parse_fields {reg_name data_value} {
  global reg_field_data
  set reg_fields [ lsearch -inline -all -regexp $reg_field_data "^$reg_name,.*" ]
  puts "Fields for $reg_name from value: $data_value"
  foreach field_data $reg_fields {
    set csv_line [ split $field_data "," ]

    set value [ expr $data_value >> [lindex $csv_line 2] ]
    set value [ expr $value & [lindex $csv_line 3] ]
    
    puts "\t$value\t:[lindex $csv_line 1]"
  }
}


proc find_addr { {regHeir ""} { cropping 0 } } {
  global reg_addr_data
  set returnVal "Not Found"  
  #puts "Searching for $regHeir"
  set searchString [string map { "\[" "\\\[" } $regHeir ]
  set searchString [string map { "\]" "\\\]" } $searchString ]
  set searchString [string map { "\." "\\\." } $searchString ]
  set searchString "^${searchString}\[^\\.\\]]*,.*"
  #puts $searchString
  set regs_list [ lsearch -inline -all -regexp $reg_addr_data $searchString ]

  if { [llength $regs_list] == 1 } {
    set csv_line [ split [lindex $regs_list 0] "," ]
    if { [lindex $csv_line 1] == "true" } {
       # Return Address, Register Name, Instance Name
       set returnVal [list [lindex $csv_line 4] [lindex $csv_line 3] [lindex $csv_line 0] ]
       puts "Register Found: [lindex $csv_line 0]=$returnVal"
       return $returnVal
    } 
  }
  
  # Try and Find a Match
  if { [llength $regs_list] == 0} {
    if { $cropping == 1  && [string length $regHeir] > 0 } {
      return [find_addr [string range $regHeir 0 end-1 ] $cropping ]
    
    }
  
    if { [string index $regHeir end ] == "\]" } {
        return [find_addr "$regHeir." $cropping]
    }
    if { [string index $regHeir end-1 ] == "\]" && [string index $regHeir end ] == "." } {
      return [find_addr [string range $regHeir 0 [expr [ string last "\[" $regHeir ] - 1 ]] $cropping ]
    }
    if { [string length $regHeir] > 0 } {
      return [find_addr [string range $regHeir 0 end-1 ] 1 ]
    }
  } elseif { $cropping == 1 } {
    puts "Exact Match Not Found. Did you mean any of the following:"
  }
         
  #Print out the possible Matches
  foreach reg $regs_list {
    set returnVal "Multiple Found"  
    set csv_line [ split $reg "," ]
    set name [lindex $csv_line 0]
    set index [lindex $csv_line 2]
    
    if { $index != "-1" } {
      puts "$name\[[expr $index - 1 ]:0\]"
    } else { puts $name }
  }
  return $returnVal 
}

# Programs the FPGA with a bit file
proc program_fpga {} {
  set prg_dir "../builds"
  set prgfile "denny_switch_top"
  
  set_property PROGRAM.FILE "${prg_dir}/${prgfile}.bit" [get_hw_devices xcvu29p_0]
  program_hw_devices [get_hw_devices xcvu37p_0]

  refresh_hw_device [lindex [get_hw_devices xcvu37p_0] 0]
}

#open_hw_manager
#connect_hw_server -allow_non_jtag
#open_hw_target
#current_hw_device [get_hw_devices xcvu37p_0]
#refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xcvu37p_0] 0]
