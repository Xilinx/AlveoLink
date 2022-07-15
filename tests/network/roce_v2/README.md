# RoCE-V2 Latency Tests 

## Prerequisits

Vitis **2022.1_released**

XRT **2.13.0**

U55C Shell **xilinx_u55c_gen3x16_xdma_3_202210_1**


## Settings

```
export AL_PATH=/proj/rdi-xsj/staff/lingl/nobkup/FaaSApps/AlveoLink
```

## Build HW

```
cd ./HiveNet/hw
make build TARGET=hw
```

## Run SW

### Run Echo Kernel 
```
$cd ./HiveNet/sw
$make run_col

$Do you want to enable RS-FEC? (answer 0/1)
1

$Do you want to turn on generator? (answer 0/1)
$0
```

### Run Generator
```
cd ./hw
make run_gen

$Do you want to enable RS-FEC? (answer 0/1)
1

$Do you want to turn on generator? (answer 0/1)
$1

```
