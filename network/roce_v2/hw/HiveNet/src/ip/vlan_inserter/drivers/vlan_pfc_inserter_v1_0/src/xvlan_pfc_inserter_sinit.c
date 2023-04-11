// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Tool Version Limit: 2022.04
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xvlan_pfc_inserter.h"

extern XVlan_pfc_inserter_Config XVlan_pfc_inserter_ConfigTable[];

XVlan_pfc_inserter_Config *XVlan_pfc_inserter_LookupConfig(u16 DeviceId) {
	XVlan_pfc_inserter_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XVLAN_PFC_INSERTER_NUM_INSTANCES; Index++) {
		if (XVlan_pfc_inserter_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XVlan_pfc_inserter_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XVlan_pfc_inserter_Initialize(XVlan_pfc_inserter *InstancePtr, u16 DeviceId) {
	XVlan_pfc_inserter_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XVlan_pfc_inserter_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XVlan_pfc_inserter_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

