# Test xnik kernel with two cards sending and receiving data to and from each other
## 1. set up Vitis 2021.2_released environemnts

## 2. build hw

```sh
navigate to tests/kernel/sync_udp_test

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=0
```

## 3. compile and run netConfig 

```sh
on db4, navigate to tests/kernel/sync_udp_test

../../../network/udp/scripts/run_compile.sh ./config_1_3.txt

../../../network/udp/scripts/run_netConfig.sh ./config_1_3.txt ./ip.txt
```

## 4. compile test host code and manager code

```sh
on db4, navigate to tests/kernel/sync_udp_test

./run_compile.sh ./config_1_3.txt
```

## 5. run tests

```sh
on db4,  navigate to tests/kernel/sync_udp_test and run
../../../kernel/sw/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/manager.exe ./ip.txt 2 100

on db1, navigate to tests/kernel/sync_udp_test and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe ./xsj-dxgradb01_0_sockets.txt ./ip.txt 16537448 4 50

on db3, navigate to tests/kernel/sync_udp_test and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb03/host.exe ./xsj-dxgradb03_0_sockets.txt ./ip.txt 16537448 4 50
```
