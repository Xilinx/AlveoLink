# Test DV adapter + Switch + XNIK hw manager + XNIK Sync kernel 
## 1. set up env

```sh
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
export DVDIR=/proj/rdi-xsj/staff/lingl/DataVortex/packages/Aug_24_2022/xos
export AL_PATH=/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink
```

## 2. build hw .xclbin and host executable with dv adapter for u55c on db4

```sh
navigate to tests/kernel/sync_dv/compute

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2
make clean TARGET=hw
make host TARGET=hw

navigate to tests/kernel/sync_dv/tm_sync_managers

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2
make clean TARGET=hw HOST=xsj-dxgradb04
make host TARGET=hw HOST=xsj-dxgradb04
```

## 3. Run test 

```sh
1. on db4, start one terminal and navigate to tests/kernel/sync_dv/tm_sync_managers, and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/manager.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/latest/xnikSyncTraffic_managers.xclbin 0 32768 2 4

2. on db4, start another terminal and navigate to tests/kernel/sync_dv/compute, and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/test.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/sync_dv/compute/xnikSyncDV_compute.xclbin 1 2 ./netconfig_04.txt 25600

```
