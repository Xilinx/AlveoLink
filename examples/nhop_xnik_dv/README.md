# DV+XNIK(with HW manager)+nHop+traffic manager
# 2 nHop kernels running on card1 on db1, hw manager and traffic manager running on card 0
 
## 1. set up Vitis 2021.2_released environemnts and env DVDIR and AL_PATH

```sh
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
export DVDIR=/proj/rdi-xsj/staff/lingl/DataVortex/packages/Aug_24_2022/xos
export AL_PATH=/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink
```


## 2. build hw .xclbin and host executable with dv adapter for u55c on db1

```sh
navigate to graphanalytics/L2/nhop_xnik_dv

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

make clean TARGET=hw
make host TARGET=hw

navigate to AlveoLink/tests/kernel/sync_dv/tm_sync_managers

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

make clean TARGET=hw HOST=xsj-dxgradb01
make host TARGET=hw HOST=xsj-dxgradb01

```
## 3. compile partition and aggregation executables

```sh
navigate to graphanalytics/L2/nhop_xnik_dv

make -f partition.Makefile host TARGET=hw  PLATFORM_REPO_PATHS=/proj/xbuilds/2022.1_qualified_latest/internal_platforms
make -f merge.Makefile host TARGET=hw  PLATFORM_REPO_PATHS=/proj/xbuilds/2022.1_qualified_latest/internal_platforms
```

## 4. Partition input graph and pairs

```sh
 ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/partition.exe --graph /proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT/as-Skitter-wt.mtx --pair /proj/xsjhdstaff5/yunleiz/zmq/push/L2/nHop_v6/gen/pair10k/as-Skitter-twoHopPair.mtx --numKernel 2
```

## 5. Run test 

```sh
1. on db1, start one terminal and navigate to AlveoLink/tests/kernel/sync_dv/tm_sync_managers, and run (note: wait until you see the link is up before going to step 2)

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/manager.exe /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/xnikSyncTraffic_managers.xclbin 0 1048576 65536 3 12

2. on db1, start another terminal and navigate to graphanalytics/L2/nhop_xnik_dv, and run

 ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/nhop_dv.xclbin --devId 1 --hostId 0 --startId 0 --numIds 2 --numKernel 2 --filepath ./as-Skitter-twoHopPair --netfile ./netconfig_01.txt --hop 3  --batch 512 

```

## 6. aggregate Results

```sh
build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/merge.exe --filepath ./as-Skitter-twoHopPair --numKernel 2
```

## 7. run hw_emu for C2C (2 kernels) with hw manager

```sh
navigate to L2/nhop_xnik_dv/nhop_manager_hw_emu, and run

make run TARGET=hw_emu DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2
```
