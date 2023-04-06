// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Tool Version Limit: 2022.04
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xvlan_pfc_inserter.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XVlan_pfc_inserter_CfgInitialize(XVlan_pfc_inserter *InstancePtr, XVlan_pfc_inserter_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XVlan_pfc_inserter_Set_insert_vlan(XVlan_pfc_inserter *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVlan_pfc_inserter_WriteReg(InstancePtr->Control_BaseAddress, XVLAN_PFC_INSERTER_CONTROL_ADDR_INSERT_VLAN_DATA, Data);
}

u32 XVlan_pfc_inserter_Get_insert_vlan(XVlan_pfc_inserter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVlan_pfc_inserter_ReadReg(InstancePtr->Control_BaseAddress, XVLAN_PFC_INSERTER_CONTROL_ADDR_INSERT_VLAN_DATA);
    return Data;
}

void XVlan_pfc_inserter_Set_pfc_code(XVlan_pfc_inserter *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVlan_pfc_inserter_WriteReg(InstancePtr->Control_BaseAddress, XVLAN_PFC_INSERTER_CONTROL_ADDR_PFC_CODE_DATA, Data);
}

u32 XVlan_pfc_inserter_Get_pfc_code(XVlan_pfc_inserter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVlan_pfc_inserter_ReadReg(InstancePtr->Control_BaseAddress, XVLAN_PFC_INSERTER_CONTROL_ADDR_PFC_CODE_DATA);
    return Data;
}

void XVlan_pfc_inserter_Set_vlan_id(XVlan_pfc_inserter *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XVlan_pfc_inserter_WriteReg(InstancePtr->Control_BaseAddress, XVLAN_PFC_INSERTER_CONTROL_ADDR_VLAN_ID_DATA, Data);
}

u32 XVlan_pfc_inserter_Get_vlan_id(XVlan_pfc_inserter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XVlan_pfc_inserter_ReadReg(InstancePtr->Control_BaseAddress, XVLAN_PFC_INSERTER_CONTROL_ADDR_VLAN_ID_DATA);
    return Data;
}

