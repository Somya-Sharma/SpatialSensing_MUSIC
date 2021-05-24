// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xsaa_static.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XSaa_static_CfgInitialize(XSaa_static *InstancePtr, XSaa_static_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_bus_BaseAddress = ConfigPtr->Control_bus_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XSaa_static_Start(XSaa_static *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL) & 0x80;
    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL, Data | 0x01);
}

u32 XSaa_static_IsDone(XSaa_static *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XSaa_static_IsIdle(XSaa_static *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XSaa_static_IsReady(XSaa_static *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XSaa_static_EnableAutoRestart(XSaa_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL, 0x80);
}

void XSaa_static_DisableAutoRestart(XSaa_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_AP_CTRL, 0);
}

void XSaa_static_InterruptGlobalEnable(XSaa_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_GIE, 1);
}

void XSaa_static_InterruptGlobalDisable(XSaa_static *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_GIE, 0);
}

void XSaa_static_InterruptEnable(XSaa_static *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_IER);
    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_IER, Register | Mask);
}

void XSaa_static_InterruptDisable(XSaa_static *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_IER);
    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_IER, Register & (~Mask));
}

void XSaa_static_InterruptClear(XSaa_static *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaa_static_WriteReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_ISR, Mask);
}

u32 XSaa_static_InterruptGetEnabled(XSaa_static *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_IER);
}

u32 XSaa_static_InterruptGetStatus(XSaa_static *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSaa_static_ReadReg(InstancePtr->Control_bus_BaseAddress, XSAA_STATIC_CONTROL_BUS_ADDR_ISR);
}

