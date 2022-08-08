# Test Roce_V2 IP with an HLS kernel

## 1. set up Vitis 2022.1_released environemnts and env AL_PATH on db3

```sh
source /proj/xbuilds/2022.1_released/installs/lin64/Vitis/2022.1/settings64.sh
source /opt/xilinx/xrt/setup.sh
export AL_PATH=/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink
```

## 2. build hw .xclbin and host executable with roce_v2 for u55c on db3

```sh
navigate to tests/network/roce_v2/generator_collector

make xclbin TARGET=hw DEVICE=/proj/xbuilds/2022.1_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_3_202210_1/xilinx_u55c_gen3x16_xdma_3_202210_1.xpfm INTERFACE=3

```

