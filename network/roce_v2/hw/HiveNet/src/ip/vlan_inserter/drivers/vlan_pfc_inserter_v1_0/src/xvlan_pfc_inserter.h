// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Tool Version Limit: 2022.04
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XVLAN_PFC_INSERTER_H
#define XVLAN_PFC_INSERTER_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xvlan_pfc_inserter_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u64 Control_BaseAddress;
} XVlan_pfc_inserter_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XVlan_pfc_inserter;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XVlan_pfc_inserter_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XVlan_pfc_inserter_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XVlan_pfc_inserter_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XVlan_pfc_inserter_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XVlan_pfc_inserter_Initialize(XVlan_pfc_inserter *InstancePtr, u16 DeviceId);
XVlan_pfc_inserter_Config* XVlan_pfc_inserter_LookupConfig(u16 DeviceId);
int XVlan_pfc_inserter_CfgInitialize(XVlan_pfc_inserter *InstancePtr, XVlan_pfc_inserter_Config *ConfigPtr);
#else
int XVlan_pfc_inserter_Initialize(XVlan_pfc_inserter *InstancePtr, const char* InstanceName);
int XVlan_pfc_inserter_Release(XVlan_pfc_inserter *InstancePtr);
#endif


void XVlan_pfc_inserter_Set_insert_vlan(XVlan_pfc_inserter *InstancePtr, u32 Data);
u32 XVlan_pfc_inserter_Get_insert_vlan(XVlan_pfc_inserter *InstancePtr);
void XVlan_pfc_inserter_Set_pfc_code(XVlan_pfc_inserter *InstancePtr, u32 Data);
u32 XVlan_pfc_inserter_Get_pfc_code(XVlan_pfc_inserter *InstancePtr);
void XVlan_pfc_inserter_Set_vlan_id(XVlan_pfc_inserter *InstancePtr, u32 Data);
u32 XVlan_pfc_inserter_Get_vlan_id(XVlan_pfc_inserter *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
