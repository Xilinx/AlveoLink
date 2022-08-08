rm -r xup_vitis_network_example
rm -r build

mkdir build
git clone https://github.com/Xilinx/xup_vitis_network_example.git
cd xup_vitis_network_example/
git checkout c16cba6598099f87e203659d7f8d6a536d97ef86
cp ../template.xml ./Ethernet
cp ../bd_cmac.tcl ./Ethernet
cd Ethernet
make all DEVICE=xilinx_u55c_gen3x16_xdma_base_3 INTERFACE=3
cp _x.xilinx_u55c_gen3x16_xdma_base_3/cmac_0.xo ../../build/cmac_0.xo
cp _x.xilinx_u55c_gen3x16_xdma_base_3/cmac_1.xo ../../build/cmac_1.xo
cd ..
cd ..
