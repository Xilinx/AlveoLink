# Test Roce_V2 IP with an HLS kernel

## 1. set up Vitis 2022.1_released environemnts and env AL_PATH on db2

```sh
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
export AL_PATH=/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink
```

## 2. build hw .xclbin with roce_v2 for u55c on db2

```sh
navigate to tests/network/roce_v2/generator_collector

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2022.1_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_3_202210_1/xilinx_u55c_gen3x16_xdma_3_202210_1.xpfm INTERFACE=3

```

## 3. build host executable for u55c on db2

```sh
make clean TARGET=hw
make host TARGET=hw
```

## 4. run the test

```sh
./build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/xsj-dxgradb02/HiveNet_test.exe -x /proj/rdi-xsj/staff/lingl/nobkup/xclbins/network_tests/roce_v2/generator_collector/hiveNetTest.xclbin -s 0 -c 53
```

### Allowed options:
  * -h, --help                          produce help message
  * -d, --dev_1 arg (=0)                Device 1 index 
  * -k, --dev_2 arg (=1)                Device 2 index
  * -x, --xclbin arg (=binary_container_1.xclbin)
  * -s, --sender_id arg (=1)            sender device id
  * -c, --cnt arg (=32)                 packet count



