# Test Switch + XNIK hw manager + XNIK Sync kernel in hw emulation 
## 1. set up env

```sh
source /proj/xbuilds/2021.2_released/installs/lin64/Vitis/2021.2/settings64.sh
source /opt/xilinx/xrt/setup.sh
```

## 2. build and run hw_emu on db1 

```sh
navigate to tests/kernel/sync_dv/compute_manager_hw_emu

make run TARGET=hw_emu DEVICE=/proj/xbuilds/2021.2_released/xbb/dsadev/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_2_202110_1/xilinx_u55c_gen3x16_xdma_2_202110_1.xpfm  INTERFACE=2 2>&1 | tee log_hw_emu

```

## 3. run hw_emu and observe the waveform 

```sh
navigate to tests/kernel/sync_dv/compute_manager_hw_emu

mv xrt.ini.debug xrt.ini
XCL_EMULATION_MODE=hw_emu build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/manager_compute.exe build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/xnikSyncDV_manager_compute.xclbin 0 8 10 1 0

Once the vivado launches, and shows a default waveform, you can press the "stop/break" button to stop the waveform, and open a new .wcfg file that contains the signals you are interested in. For example, the compute_manager.wcfg file under AlveoLink/tests/kernel/sync_dv/ directory.

```
