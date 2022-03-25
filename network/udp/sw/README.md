# This folder contains software code to build netConfig.exe and arpDisc.exe. netConfig.exe is used to configure UDP-based network system. arpDisc.exe is used to run ARP discovery on each of the Alveo card.

# Build steps

## 1. Set up Vitis 2021.2 released environment

```sh
source /opt/xilinx/xrt/setup.sh
```

## 2. Build executabe under this directory

```sh
make cleanall
make host
```
