// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xula_stream_static.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XUla_stream_static_CfgInitialize(XUla_stream_static *InstancePtr, XUla_stream_static_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_bus_BaseAddress = ConfigPtr->Control_bus_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XUla_stream_static_Start(XUla_stream_static *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL) & 0x80;
    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL, Data | 0x01);
}

u32 XUla_stream_static_IsDone(XUla_stream_static *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XUla_stream_static_IsIdle(XUla_stream_static *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XUla_stream_static_IsReady(XUla_stream_static *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XUla_stream_static_EnableAutoRestart(XUla_stream_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL, 0x80);
}

void XUla_stream_static_DisableAutoRestart(XUla_stream_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_AP_CTRL, 0);
}

void XUla_stream_static_InterruptGlobalEnable(XUla_stream_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_GIE, 1);
}

void XUla_stream_static_InterruptGlobalDisable(XUla_stream_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_GIE, 0);
}

void XUla_stream_static_InterruptEnable(XUla_stream_static *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_IER);
    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_IER, Register | Mask);
}

void XUla_stream_static_InterruptDisable(XUla_stream_static *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_IER);
    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_IER, Register & (~Mask));
}

void XUla_stream_static_InterruptClear(XUla_stream_static *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XUla_stream_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_ISR, Mask);
}

u32 XUla_stream_static_InterruptGetEnabled(XUla_stream_static *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_IER);
}

u32 XUla_stream_static_InterruptGetStatus(XUla_stream_static *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XUla_stream_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XULA_STREAM_STATIC_CONTROL_BUS_ADDR_ISR);
}

