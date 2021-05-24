// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XSAA_STATIC_H
#define XSAA_STATIC_H

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
#include "xsaa_static_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_bus_BaseAddress;
} XSaa_static_Config;
#endif

typedef struct {
    u32 Control_bus_BaseAddress;
    u32 IsReady;
} XSaa_static;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XSaa_static_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XSaa_static_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XSaa_static_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XSaa_static_ReadReg(BaseAddress, RegOffset) \
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
int XSaa_static_Initialize(XSaa_static *InstancePtr, u16 DeviceId);
XSaa_static_Config* XSaa_static_LookupConfig(u16 DeviceId);
int XSaa_static_CfgInitialize(XSaa_static *InstancePtr, XSaa_static_Config *ConfigPtr);
#else
int XSaa_static_Initialize(XSaa_static *InstancePtr, const char* InstanceName);
int XSaa_static_Release(XSaa_static *InstancePtr);
#endif

void XSaa_static_Start(XSaa_static *InstancePtr);
u32 XSaa_static_IsDone(XSaa_static *InstancePtr);
u32 XSaa_static_IsIdle(XSaa_static *InstancePtr);
u32 XSaa_static_IsReady(XSaa_static *InstancePtr);
void XSaa_static_EnableAutoRestart(XSaa_static *InstancePtr);
void XSaa_static_DisableAutoRestart(XSaa_static *InstancePtr);


void XSaa_static_InterruptGlobalEnable(XSaa_static *InstancePtr);
void XSaa_static_InterruptGlobalDisable(XSaa_static *InstancePtr);
void XSaa_static_InterruptEnable(XSaa_static *InstancePtr, u32 Mask);
void XSaa_static_InterruptDisable(XSaa_static *InstancePtr, u32 Mask);
void XSaa_static_InterruptClear(XSaa_static *InstancePtr, u32 Mask);
u32 XSaa_static_InterruptGetEnabled(XSaa_static *InstancePtr);
u32 XSaa_static_InterruptGetStatus(XSaa_static *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
