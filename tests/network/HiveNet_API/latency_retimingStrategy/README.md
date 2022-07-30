# RoCE-V2 Latency Tests 

1. Prerequisits

Vitis **2022.1_released**

XRT **2.13.0**

U55C Shell **xilinx_u55c_gen3x16_xdma_3_202210_1**

2. build hw

```
cd hw
make all TARGET=hw

```

3. build test case

```
make host TARGET=hw
```

4. run test case

```

./build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/host.exe hw/binary_container_1.xclbin
```