set git_repo "/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink/tests/network/dv/echo"

source "$git_repo/jtag_peek_poke.tcl"


#read_data "c:/FPGAs/IH/denny_network/common/Register/Outputs/script_input"

# Returns integer value of specific bits start at bit from and ending at bit to
proc BitField {value from to} {
    if {$from > $to} {
        error "reversed bit field description"
    }
    # How many bits wide is the field?
    set width [expr {$to - $from + 1}]
    # Convert the width into a bit mask in the lowest bits
    set mask [expr {(1 << $width) - 1}]
    # Shift and mask the value to get the field
    expr {($value >> $from) & $mask}
}

# Returns hex value of specific bits start at bit from and ending at bit to
proc HexBitField {value from to} {
    hex [BitField $value $from $to]
}
 

# Reads version and build registers
proc read_rev {} {
  puts ""
  puts "HW       Version: [read_value 0]"
  puts "Register Version: [read_value 4]"
  puts "SC       Version: [read_value 8]"
  puts "FPGA     Version: [read_value C]"
  puts "Build DATE: [read_value 10]"
  puts "Build Time: [read_value 14]"
  puts "Commit DATE: [read_value 18]"
  puts "Commit Time: [read_value 1C]"
  puts "Hash Short: [read_value 20]"
  puts "Local Repo: [read_value 24]"
  puts "FPGA Temp : [read_value 38]"
}


# Read and write scratchpad register
proc test_scratch {} {
  puts ""
  puts "Scratchpad Test"
  puts "---------------"
  set spad [read_value 3c]
  if { $spad != 0 } {
    puts "Initial read of scratch pad: $spad"
  }

  write 3c deadbeef
  set spad [read_value 3c]
  if { $spad != "deadbeef" } {
    puts "Scratch pad should read deadbeef but reads: $spad"
  }

  write 3c 01234567
  set spad [read_value 3c]
  if { $spad != "01234567" } {
    puts "Scratch pad should read 01234567 but reads: $spad"
  }
}


# Convert decimal to hex
proc hex {dec} {
  return [format %x $dec]
}

# Convert hex to decimal
proc dec {hex} {
  return [expr 0x$hex]
}

# Keeps leading 0s for 32b decimal to hex value
proc hex32 {dec} {
  binary scan [binary format "I" $dec] "H*" hexval
  return $hexval
}


# Returns the decimal value of a 64b counter
proc read_one_cntr {hexaddr} {

  # Read the low and high count words and add them
  set lo_cnt [dec [read_value $hexaddr]]
  set hexaddr [hex [expr {4 + "0x$hexaddr"}]]
  set hi_cnt [dec [read_value $hexaddr]]
  
  return [expr {$lo_cnt + $hi_cnt}]
}

# Returns the decimal value of an Rx 64b counter
proc read_rx_cntr {cntr_num} {

  # Rx counters start address
  set hexaddr 40
  
  # Add addr offset for the particular counter
  set hexaddr [hex [expr {8*$cntr_num + "0x$hexaddr"}]]
    
  return [read_one_cntr $hexaddr]
}

# Returns the decimal value of an Rx 64b counter
proc read_tx_cntr {cntr_num} {

  # Tx counters start address
  set hexaddr c0
  
  # Add addr offset for the particular counter
  set hexaddr [hex [expr {8*$cntr_num + "0x$hexaddr"}]]
    
  return [read_one_cntr $hexaddr]
}


# Read all the counters
proc read_cntrs {} {

  puts ""
  puts "RX & Tx Counters - Decimal & Hex"
  puts "-------------------------------"
  
  for {set i 0} {$i < 4} {incr i} {
    for {set j 0} {$j < 4} {incr j} {
    
      set cntr_num [expr {$i*4 + $j}]
      set rx_cnt [read_rx_cntr $cntr_num]
      set tx_cnt [read_tx_cntr $cntr_num]
      
      puts "Q{$i}L$j - Rx:$rx_cnt Tx:$tx_cnt | Rx:[hex $rx_cnt] Tx:[hex $tx_cnt]"
    }
    puts ""
  }
}


# Read all the Rx counters in 1 QSFP and return the sum
proc qsfp_rx_cntr {qsfp_num} {

  set rx_sum 0

  for {set i 0} {$i < 4} {incr i} {
    
    set cntr_num [expr {$qsfp_num*4 + $i}]
    set rx_cnt [read_rx_cntr $cntr_num]
    set rx_sum [expr {$rx_sum + $rx_cnt}]
  }
  return $rx_sum
}


# Read all the Tx counters in 1 QSFP and return the sum
proc qsfp_tx_cntr {qsfp_num} {

  set tx_sum 0

  for {set i 0} {$i < 4} {incr i} {
    
    set cntr_num [expr {$qsfp_num*4 + $i}]
    set tx_cnt [read_tx_cntr $cntr_num]
    set tx_sum [expr {$tx_sum + $tx_cnt}]
  }
  return $tx_sum
}



# Read top level port registers
proc port_status {} {
  puts ""
  puts "PORT STATUS"
  puts "-----------"
  puts "Ports Disables:   [read_value 180]"
  puts "Card Map:         [read_value 184]"
  puts "Port Error:       [read_value 188]"
  puts "Switch interrupt: [read_value 18c]"
}


# Lists QSFP info per QSFP
proc qsfp_status {} {
  set addr 140
  set rx_total 0
  set tx_total 0
  set err_cnt 0
  for {set i 0} {$i < 4} {incr i} {
    puts ""
    puts "QSFP$i STATUS"
    puts "----------------------"
    set rx_cnt [qsfp_rx_cntr $i]
    set tx_cnt [qsfp_tx_cntr $i]
    set rx_total [expr {$rx_total + $rx_cnt}]
    set tx_total [expr {$tx_total + $tx_cnt}]
    puts "Rx Cnts:$rx_cnt, Tx Cnts:$tx_cnt"
    set reg [dec [read_value $addr]]
    puts "External pins - Interrupt:[BitField $reg 25 25] , Present:[BitField $reg 24 24]"
    puts "Xcvr Info - RST_O:[HexBitField $reg 20 23], GTPPL_LOCK:[HexBitField $reg 16 19], SOFTERR:[HexBitField $reg 12 15], HARDERR:[HexBitField $reg 8 11], LANEUP:[HexBitField $reg 4 7], CH_UP:[HexBitField $reg 0 3]"
    set addr [hex [expr {4 + "0x$addr"}]]
    
    set reg [dec [read_value $addr]]
    puts ""
    puts "Cable control pins - Reset:[BitField $reg 0 0], Low power mode:[BitField $reg 1 1], Mode select:[BitField $reg 7 7]"
    puts "PHY control - Reset:[BitField $reg 2 2], Powerdown:[BitField $reg 3 3], loopback:[BitField $reg 4 6], Gt_rxcdrovrden_in:[BitField $reg 13 13]"
    set addr [hex [expr {4 + "0x$addr"}]]
    
    set near_err [dec [read_value $addr]]
    set err_cnt [expr $err_cnt + "0x[BitField $near_err 0 7]" + "0x[BitField $near_err 8 15]" + "0x[BitField $near_err 16 23]" + "0x[BitField $near_err 24 31]"]
    set addr [hex [expr {4 + "0x$addr"}]]
    set far_err [read_value $addr]
    set err_cnt [expr $err_cnt + "0x[BitField $far_err 0 7]" + "0x[BitField $far_err 8 15]" + "0x[BitField $far_err 16 23]" + "0x[BitField $far_err 24 31]"]
    set addr [hex [expr {4 + "0x$addr"}]]
    puts "Near Error Cnt:[dec $near_err], Far Error Cnt:[dec $far_err]"
  }
  puts "TOTAL COUNTS: RX:$rx_total, TX:$tx_total  -- Difference:[expr $rx_total - $tx_total]"
  puts "TOTAL ERRORS: $err_cnt"
}


# Clears all QSFP packet and error counters
proc clr_cntrs {} {
  set addr 144
  for {set i 0} {$i < 4} {incr i} {
    write $addr 00001F00 
    set addr [hex [expr {4*4 + "0x$addr"}]]
  }
}  
 
# Disables a QSFP. num specifies QSFP to disable.
# The other QSFPs in the register are masked during the write.
proc disable_qsfp {num} {
  set disable_reg [read_value 180]
  set mask [expr 1<<$num]
  write 180 [hex32 [expr "0x$disable_reg" | $mask]]
}

# Enables a QSFP 
proc enable_qsfp {num} {
  set disable_reg [read_value 180]
  # invert the mask so can AND the register with the mask to clear a disable
  set mask [expr ~(1<<$num)]
  write 180 [hex32 [expr "0x$disable_reg" & $mask]]
}

  
# Execute the functions when fill sourced
read_rev
#test_scratch
read_cntrs
port_status
qsfp_status




