# Test DV adapter + Switch 
## 1. set up Vitis 2021.2_released environemnts and env DVDIR

```sh
source /proj/xbuilds/2021.2_released/installs/lin64/Vitis/2021.2/settings64.sh
source /opt/xilinx/xrt/setup.sh
export DVDIR=/proj/rdi-xsj/staff/lingl/DataVortex/file_4_25_2022/Xilinx_2022_04_25/xo
```

## 2. build hw .xclbin with dv adapter for u55c on db1

```sh
navigate to tests/network/dv/echo

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

use xbutil to program u55c on db1 with the generated .xclbin
```

## 3. Open Vivado hw_manager on db1, navigate to tests/network/dv/echo

```sh
download the following bitstream via vivado hw_manager

/proj/rdi-xsj/staff/lingl/DataVortex/denny_switch_top_v0.5.14-1-g5bcd3ae.bit

from vivado tcl console, enter following commands

source jtag_cmds.tcl

port_status

qsfp_status
```

4. Run host code to retrieve DV registers. 

```sh
on db1, navigate to tests/network/dv/echo, and run

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe /proj/rdi-xsj/staff/lingl/nobkup/echo_2_inf/dvTestEcho.xclbin 0

or

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe /proj/rdi-xsj/staff/lingl/nobkup/echo_2_inf/dvTestEcho.xclbin 0
```

