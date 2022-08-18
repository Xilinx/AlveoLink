# AlveoLink
This repository contains IPs, Vitis kernels and software APIs that can be leveraged by Vitis users to build scale-out solutions on multiple Alveo cards. A set of example designs are also provided to demonstrate the usage of these components.

## Repository structure

~~~
├── network
│   └── roce_v2
├── kernel
├── examples
|   └── network 
└── img
~~~

* network: this folder contains the 100Gb/s networking IPs. The roce_v2 subfolder contains the lossless communication support built on roce_v2 protocal.
* kernel: this folder contains the Xilinx Network Interface Kernels (XNIK) for synchronization control and data caching support.
* examples: this folder contains Vitis example designs built on top of network and kernel components.
* img: this folder contains images.


