# test xnikRX module in HLS cosim 

## 1. set up Vitis 2021.2_released environemnts and env DVDIR

```sh
source /proj/xbuilds/2021.2_released/installs/lin64/Vitis/2021.2/settings64.sh
source /opt/xilinx/xrt/setup.sh
```

## 2. run test

```sh
navigate to tests/hls_modules/xnikRX_DV

make cleanall DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm  COSIM=1 2>&1 | tee log_cosim

make run DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm  COSIM=1 2>&1 | tee log_cosim
```
## 3. view the waveform

```sh
navigate to tests/hls_modules/xnikRX_DV

xsim test.prj/sol/sim/verilog/uut_top.wdb -gui
```
