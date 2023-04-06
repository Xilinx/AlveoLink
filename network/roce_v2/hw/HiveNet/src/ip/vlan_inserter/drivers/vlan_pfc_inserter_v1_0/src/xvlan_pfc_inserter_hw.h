// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Tool Version Limit: 2022.04
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of insert_vlan
//        bit 0  - insert_vlan[0] (Read/Write)
//        others - reserved
// 0x14 : reserved
// 0x18 : Data signal of pfc_code
//        bit 2~0 - pfc_code[2:0] (Read/Write)
//        others  - reserved
// 0x1c : reserved
// 0x20 : Data signal of vlan_id
//        bit 11~0 - vlan_id[11:0] (Read/Write)
//        others   - reserved
// 0x24 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XVLAN_PFC_INSERTER_CONTROL_ADDR_INSERT_VLAN_DATA 0x10
#define XVLAN_PFC_INSERTER_CONTROL_BITS_INSERT_VLAN_DATA 1
#define XVLAN_PFC_INSERTER_CONTROL_ADDR_PFC_CODE_DATA    0x18
#define XVLAN_PFC_INSERTER_CONTROL_BITS_PFC_CODE_DATA    3
#define XVLAN_PFC_INSERTER_CONTROL_ADDR_VLAN_ID_DATA     0x20
#define XVLAN_PFC_INSERTER_CONTROL_BITS_VLAN_ID_DATA     12

