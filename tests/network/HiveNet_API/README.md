# RoCE-V2 Latency Tests 

1. Prerequisits

XRT **2.13.0**

U55C Shell **xilinx_u55c_gen3x16_xdma_3_202210_1**

2. build test case

```
make host TARGET=hw
```

3. run test case

```
./build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/host.exe binary_container_1.xclbin
```