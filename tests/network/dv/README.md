# Test DV adapter + Switch 
## 1. set up Vitis 2021.2_released environemnts and env DVDIR

```sh
source /proj/xbuilds/2021.2_released/installs/lin64/Vitis/2021.2/settings64.sh
source /opt/xilinx/xrt/setup.sh
export DVDIR=/proj/rdi-xsj/staff/lingl/DataVortex/Xilinx_2022_05_09
```

## 2. build hw .xclbin with dv adapter for u55c on db1

```sh
navigate to tests/network/dv/echo

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

navigate to tests/network/dv/driver

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

```

## 3. Open Vivado hw_manager on db1 (this step only needed when setting up DV Switch)

```sh
download the following bitstream via vivado hw_manager

/proj/rdi-xsj/staff/lingl/DataVortex/Xilinx_2022_05_09/denny_switch4_top_v2.0.2-17-g018db82.bit

from vivado tcl console, enter following commands

source /proj/rdi-xsj/staff/lingl/DataVortex/Xilinx_2022_05_09/jtag_cmds.tcl

port_status

qsfp_status
```

4. Run DV test 

```sh
1. on db1, start one terminal and navigate to tests/network/dv/echo, and run
make clean TARGET=hw
make host TARGET=hw
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/echo.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/dv_test/echo_2_inf/dvTestEcho.xclbin 0

2. on db1, start another terminal and navigate to tests/network/dv/driver, and run
make clean TARGET=hw
make host TARGET=hw
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/driver.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/dv_test/driver_2_inf/dvTestDriver.xclbin 1 16

or

source ./run_benchmark.sh
```

5. run HW Emulation

```sh
on db1, start one terminal and navigate to tests/network/dv/driver

make -f hw_emu.mk run TARGET=hw_emu DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2
```
