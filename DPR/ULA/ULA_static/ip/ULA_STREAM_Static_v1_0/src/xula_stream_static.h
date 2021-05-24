// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XULA_STREAM_STATIC_H
#define XULA_STREAM_STATIC_H

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
#include "xula_stream_static_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_bus_BaseAddress;
} XUla_stream_static_Config;
#endif

typedef struct {
    u32 Control_bus_BaseAddress;
    u32 IsReady;
} XUla_stream_static;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XUla_stream_static_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XUla_stream_static_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XUla_stream_static_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XUla_stream_static_ReadReg(BaseAddress, RegOffset) \
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
int XUla_stream_static_Initialize(XUla_stream_static *InstancePtr, u16 DeviceId);
XUla_stream_static_Config* XUla_stream_static_LookupConfig(u16 DeviceId);
int XUla_stream_static_CfgInitialize(XUla_stream_static *InstancePtr, XUla_stream_static_Config *ConfigPtr);
#else
int XUla_stream_static_Initialize(XUla_stream_static *InstancePtr, const char* InstanceName);
int XUla_stream_static_Release(XUla_stream_static *InstancePtr);
#endif

void XUla_stream_static_Start(XUla_stream_static *InstancePtr);
u32 XUla_stream_static_IsDone(XUla_stream_static *InstancePtr);
u32 XUla_stream_static_IsIdle(XUla_stream_static *InstancePtr);
u32 XUla_stream_static_IsReady(XUla_stream_static *InstancePtr);
void XUla_stream_static_EnableAutoRestart(XUla_stream_static *InstancePtr);
void XUla_stream_static_DisableAutoRestart(XUla_stream_static *InstancePtr);


void XUla_stream_static_InterruptGlobalEnable(XUla_stream_static *InstancePtr);
void XUla_stream_static_InterruptGlobalDisable(XUla_stream_static *InstancePtr);
void XUla_stream_static_InterruptEnable(XUla_stream_static *InstancePtr, u32 Mask);
void XUla_stream_static_InterruptDisable(XUla_stream_static *InstancePtr, u32 Mask);
void XUla_stream_static_InterruptClear(XUla_stream_static *InstancePtr, u32 Mask);
u32 XUla_stream_static_InterruptGetEnabled(XUla_stream_static *InstancePtr);
u32 XUla_stream_static_InterruptGetStatus(XUla_stream_static *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
