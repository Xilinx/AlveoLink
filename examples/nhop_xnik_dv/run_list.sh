#!/usr/bin/env bash

# usgae ./run_list.sh partiton/merge/k2k matrix_list.txt numberKernel

graph_dir=/proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT
#pair_dir=/proj/xsjhdstaff5/siyangw/nobkup/git_hub_test/pair_small
pair_dir=/proj/xsjhdstaff5/yunleiz/zmq/push/L2/nHop_v5/pair10k

if [ "$3" ]; then
  hop=$3
else
  hop=2
fi

if [ "$4" ]; then
  batch=$4
else
  batch=512
fi


if [ $1 = 'partition' ]
then
    while IFS=' ' read -r graph
    do
        rm -rf ${graph}*
        ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/partition.exe --graph ${graph_dir}/${graph}-wt.mtx --pair ${pair_dir}/${graph}-twoHopPair.mtx --numKernel $3 
    done <"$2"
fi

if [ $1 = 'merge' ]
then
    while IFS=' ' read -r graph
    do
        ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/merge.exe --filepath ./${graph}-twoHopPair --numKernel $3 | tee ${graph}-merge.txt
    done <"$2"
fi

if [ $1 = 'diff' ]
then
    while IFS=' ' read -r graph
    do
        echo diff ./${graph}-twoHopPair.hop /wrk/xsjhdnobkup2/yifei/git_graphanalytics/graphanalytics_dev/graphanalytics/L2/nHop_v7_swSwitching/3hop_golden/${graph}-twoHopPair_golden.hop

        diff ./${graph}-twoHopPair.hop /wrk/xsjhdnobkup2/yifei/git_graphanalytics/graphanalytics_dev/graphanalytics/L2/nHop_v7_swSwitching/3hop_golden/${graph}-twoHopPair_golden.hop
    done <"$2"
fi

if [ $1 = 'k2k' ]
then
    echo 'k2k report' > k2k.txt
    while IFS=' ' read -r graph
    do
        echo ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nHop_k2k/nHop_kernel.xclbin --filepath ./${graph}-twoHopPair --numKernel 2 --hop ${hop}  --batch ${batch}
        ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nHop_k2k/nHop_kernel.xclbin --filepath ./${graph}-twoHopPair --numKernel 2 --hop ${hop}  --batch ${batch} | tee ${graph}-k2k_tmp.txt
        egrep 'pair  file path' ${graph}-k2k_tmp.txt >> k2k.txt
        egrep 'kernel numLocal' ${graph}-k2k_tmp.txt >> k2k.txt 
        egrep 'INFO : disturbute nHop compute kernel time' ${graph}-k2k_tmp.txt >> k2k.txt 
    done <"$2"
fi

if [ $1 = 'c2c' ]
then
    echo 'c2c report' > c2c.txt
    while IFS=' ' read -r graph
    do  
        echo ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/nhop_dv.xclbin --devId 1 --numDevs 2 --startId 0 --numIds 2 --filepath ./${graph}-twoHopPair --hop ${hop}  --batch ${batch}
        ./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/nhop_dv.xclbin --devId 1 --numDevs 2 --startId 0 --numIds 2 --filepath ./${graph}-twoHopPair --hop ${hop}  --batch ${batch} | tee ${graph}-c2c_tmp.txt
        egrep 'pair  file path' ${graph}-c2c_tmp.txt >> c2c.txt
        egrep 'kernel numLocal' ${graph}-c2c_tmp.txt >> c2c.txt 
        egrep 'INFO : disturbute nHop compute kernel time' ${graph}-c2c_tmp.txt >> c2c.txt 
    done <"$2"
fi
