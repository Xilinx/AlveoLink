# Test xnik kernel + adapter with two cards sending and receiving data to and from each other using hardware manager
## 1. set up Vitis 2021.2_released environemnts

## 2. build hw

```sh
navigate to tests/kernel/sync_adapter_udp

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=0
```

## 3. compile and run netConfig 

```sh
on db3(for running hw manager), navigate to tests/kernel/sync_adapter_udp

../../../network/udp/scripts/run_compileUsingHwManager.sh ./config_1_3.txt

../../../network/udp/scripts/run_netConfig.sh ./config_1_3.txt ./ip.txt
```

## 4. compile test host code and host code for hardware manager

```sh
on db3, navigate to tests/kernel/sync_adapter_udp

./run_compile.sh ./config_1_3.txt
```

## 5. run tests

```sh
on db3,  navigate to tests/kernel/sync_adapter_udp and run hardware manager
../../../kernel/builds/udp/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb03/manager.exe ./xsj-dxgradb03_0_sockets.txt ./ip.txt 100 1

run following commands repeatively:

on db1 start one terminal, navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe ./xsj-dxgradb01_0_sockets.txt ./ip.txt 128 4 2 100

on db1 start another terminal, navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe ./xsj-dxgradb01_1_sockets.txt ./ip.txt 128 4 2 100
```

# Run hw_emu on db4

```sh
make -f hw_emu.mk xclbin TARGET=hw_emu DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm
make -f hw_emu.mk emconfig TARGET=hw_emu PLATFORM_REPO_PATHS=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms
make -f hw_emu.mk host TARGET=hw_emu PLATFORM_REPO_PATHS=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms
XCL_EMULATION_MODE=hw_emu ./build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/host.exe ./sockets.txt ./ip.txt 128 8 0 100 1 2
```
