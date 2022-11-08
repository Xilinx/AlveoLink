# run nHop on 6 Alveo U55C cards
# 10 nHop kernels running on  on host01,3,4  hw manager and traffic manager running on host01 card 0
 
## 1. set up Vitis 2021.2_released environemnts and env DVDIR and AL_PATH

```sh
source /opt/xilinx/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
export DVDIR=xos_dir
export AL_PATH=$PWD/../../AlveoLink
```


## 2. build hw .xclbin dv dapter for u55c

```sh
navigate to examples/nhop_xnik_dv

make xclbin TARGET=hw DEVICE=/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

navigate to AlveoLink/tests/kernel/sync_dv/tm_sync_managers

make xclbin TARGET=hw DEVICE=/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

navigate to AlveoLink/tests/kernel/sync_dv/tm_managers

make xclbin TARGET=hw DEVICE=/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=2

```

## 3. compile host executable

```sh
on host01 navigate to examples/nhop_xnik_dv

./run_compile.sh

on host01 navigate to AlveoLink/tests/kernel/sync_dv/tm_sync_managers

make clean TARGET=hw HOST=xsj-dxgradb01
make host TARGET=hw HOST=xsj-dxgradb01

```
## 4. compile partition and aggregation executables

```sh
navigate to examples/nhop_xnik_dv

make -f partition.Makefile host TARGET=hw  PLATFORM_REPO_PATHS=/opt/xilinx/platforms
make -f merge.Makefile host TARGET=hw  PLATFORM_REPO_PATHS=/opt/xilinx/platforms
```

## 5. Partition input graph and pairs

```sh
 ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/partition.exe --graph europe_osm-wt.mtx --pair europe_osm-twoHopPair.mtx --numKernel 6 
```

## 6. Run test 

```sh
1. on host01, start one terminal and navigate to AlveoLink/tests/kernel/sync_dv/tm_sync_managers, and run (note: wait until you see the link is up before going to step 2)

./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/manager.exe xnikSyncTraffic_managers.xclbin 0 1048576 65536 3 4 (note: wait until you see INFO: my id is: 10 my TM id is: 10 before going to step 3)

2. on host01, start another terminal and navigate to AlveoLink/tests/kernel/sync_dv/tm_managers, and run

./run_tm_managers.sh 524288

3. on host04, start one terminal and navigate to examples/nhop_xnik_dv, and run

./run_nhop.sh 6 ./europe_osm-twoHopPair 3 32
```

## 7. aggregate Results

```sh
build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/merge.exe --filepath ./europe_osm-twoHopPair --numKernel 6
```
