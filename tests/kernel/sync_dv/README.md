# Test DV adapter + Switch + XNIK hw manager + XNIK Sync kernel 
## 1. set up Vitis 2021.2_released environemnts and env DVDIR

```sh
source /proj/xbuilds/2021.2_released/installs/lin64/Vitis/2021.2/settings64.sh
source /opt/xilinx/xrt/setup.sh
export DVDIR=/proj/rdi-xsj/staff/lingl/DataVortex/Xilinx_2022_05_14
```

## 2. build hw .xclbin and host executable with dv adapter for u55c on db1

```sh
navigate to tests/kernel/sync_dv/compute

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2
make clean TARGET=hw
make host TARGET=hw

navigate to tests/kernel/sync_dv/manager

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2
make clean TARGET=hw
make host TARGET=hw

```

## 3. Open Vivado hw_manager on db1 (this step only needed when setting up DV Switch)

```sh
download the following bitstream via vivado hw_manager

/proj/rdi-xsj/staff/lingl/DataVortex/Xilinx_2022_05_14/denny_switch4_top_v2.0.2-17-g018db82.bit

from vivado tcl console, enter following commands

source /proj/rdi-xsj/staff/lingl/DataVortex/Xilinx_2022_05_14/jtag_cmds.tcl

port_status

qsfp_status
```

## 4. Run test 

```sh
1. on db1, start one terminal and navigate to tests/kernel/sync_dv/manager, and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/manager.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/sync_dv/manager/xnikSyncDV_manager.xclbin 0 200 1

2. on db1, start another terminal and navigate to tests/kernel/sync_dv/compute, and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/test.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/sync_dv/compute/xnikSyncDV_compute.xclbin 1 16537454

```

## 5. Run test in debug mode to read the stats registers during the run

```sh

1. on db1, start one terminal and navigate to tests/kernel/sync_dv/manager, and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/manager.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/sync_dv/manager/xnikSyncDV_manager.xclbin 0 200 1 debug

2. on db1, start another terminal and navigate to tests/kernel/sync_dv/compute, and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/test.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/sync_dv/compute/xnikSyncDV_compute.xclbin 1 16537454 debug

```
