# Test help options

## To do partition for pair or graph only

use --partitionOption pair or --partitionOption graph if would like to partition pair or graph only.If only partition pair, make sure you have graph partiton in previous run, otherwise it won't work.

```sh
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/partition.exe --graph /proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT/as-Skitter-wt.mtx --pair /proj/xsjhdstaff5/yunleiz/zmq/push/L2/nHop_v6/gen/pair10k/as-Skitter-twoHopPair.mtx --numKernel 2 --partitionOption pair
```

The partitioned graph and pair's folder name is based on its pair name, so if you know you are partitioning the same graph with different pair(that name is different from the previous one), and you would like to use the option to partition pair only, make sure you have the partitioned graph in that folder already (copy them or rename the previous folder).

## To partition pair into groups

use --pairGroup n to partition pairs into n groups first, then partition them based on number of kernels, if use pairGroup=1, there will be no sub folders, otherwise, for each pair group, pair files will be put in pair_group_*

```sh
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/partition.exe --graph /proj/xsjhdstaff5/yunleiz/Demo_For_webinary_WT/as-Skitter-wt.mtx --pair /proj/xsjhdstaff5/yunleiz/zmq/push/L2/nHop_v6/gen/pair10k/as-Skitter-twoHopPair.mtx --numKernel 2 --pairGroup 10
```

when running hw, could use --pairGroupId to specific which group to run.

```sh
./build_dir.hw.xilinx_u55c_gen3x16_xdma_2_202110_1/xsj-dxgradb01/host.exe --xclbin /proj/rdi-xsj/staff/lingl/nobkup/xclbins/nhop_xnik_dv/nhop_dv.xclbin --devId 1 --hostId 0 --startId 0 --numIds 2 --numKernel 2 --filepath ./soc-LiveJournal1-twoHopPair --netfile ./netconfig_01.txt --hop 4  --batch 512 --pairGroupId 0

```
