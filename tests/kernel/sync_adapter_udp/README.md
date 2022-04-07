# Test xnik kernel + adapter with two cards sending and receiving data to and from each other
## 1. set up Vitis 2021.2_released environemnts

## 2. build hw

```sh
navigate to tests/kernel/sync_adapter_udp

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=0
```

## 3. compile and run netConfig 

```sh
on db4, navigate to tests/kernel/sync_adapter_udp

../../../network/udp/scripts/run_compile.sh ./config_1_3.txt

../../../network/udp/scripts/run_netConfig.sh ./config_1_3.txt ./ip.txt
```

## 4. compile test host code and server code

```sh
on db4, navigate to tests/kernel/sync_adapter_udp

./run_compile.sh ./config_1_3.txt
```

## 5. run tests

```sh
on db4,  navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/server.exe ./ip.txt 2 100

run following commands repeatively without restarting server:

on db1, navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe ./xsj-dxgradb01_0_sockets.txt ./ip.txt 16 4 50

on db3, navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb03/host.exe ./xsj-dxgradb03_0_sockets.txt ./ip.txt 16 4 50
```

# Run hw_emu on db4

```sh
make -f hw_emu.mk xclbin TARGET=hw_emu
make -f hw_emu.mk emconfig TARGET=hw_emu
make -f hw_emu.mk emconfig TARGET=hw_emu PLATFORM_REPO_PATHS=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms
XCL_EMULATION_MODE=hw_emu ./build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/host.exe ./sockets.txt ./ip.txt 16 4 50
```
