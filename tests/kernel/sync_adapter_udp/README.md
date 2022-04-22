# Test xnik kernel + adapter with two cards sending and receiving data to and from each other SW manager
## 1. set up Vitis 2021.2_released environemnts and AL_PATH on db4

```sh
source /proj/xbuilds/2021.2_released/installs/lin64/Vitis/2021.2/settings64.sh
source /opt/xilinx/xrt/setup.sh

export AL_PATH=/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink (note: please change this to your local AlveoLink repo path)
```

## 2. build hw

```sh
navigate to tests/kernel/sync_adapter_udp

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm INTERFACE=0
```

## 3. compile and run netConfig 

```sh
on db4(for running sw manager), navigate to tests/kernel/sync_adapter_udp

$AL_PATH/network/udp/scripts/run_compile.sh ./config_4_0_1.txt

$AL_PATH/network/udp/scripts/run_netConfig.sh ./config_4_0_1.txt ./ip.txt
```

## 4. compile test host code and sw manager

```sh
on db4, navigate to tests/kernel/sync_adapter_udp

./run_compileUsingSWmanager.sh ./config_4_0_1.txt
```

## 5. run tests

```sh
on db4,  navigate to tests/kernel/sync_adapter_udp and run sw manager
$AL_PATH/kernel/builds/udp/build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/manager.exe ./ip.txt 1 100 200

run following commands repeatively:

on db4 start one terminal, navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/host.exe ./xsj-dxgradb04_0_sockets.txt ./ip.txt 16537448 4 50 400

on db4 start another terminal, navigate to tests/kernel/sync_adapter_udp and run
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/host.exe ./xsj-dxgradb04_1_sockets.txt ./ip.txt 16537448 4 50 400
```

# Run hw_emu on db4

```sh
make -f hw_emu.mk xclbin TARGET=hw_emu DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm
make -f hw_emu.mk emconfig TARGET=hw_emu PLATFORM_REPO_PATHS=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms
make -f hw_emu.mk host TARGET=hw_emu PLATFORM_REPO_PATHS=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms
XCL_EMULATION_MODE=hw_emu ./build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb04/host.exe ./sockets.txt ./ip_hw_emu.txt 128 8 0 100 1 2
```
