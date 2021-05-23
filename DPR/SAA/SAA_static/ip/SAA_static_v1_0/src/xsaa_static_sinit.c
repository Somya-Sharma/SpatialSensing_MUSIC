// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xsaa_static.h"

extern XSaa_static_Config XSaa_static_ConfigTable[];

XSaa_static_Config *XSaa_static_LookupConfig(u16 DeviceId) {
	XSaa_static_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XSAA_STATIC_NUM_INSTANCES; Index++) {
		if (XSaa_static_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XSaa_static_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XSaa_static_Initialize(XSaa_static *InstancePtr, u16 DeviceId) {
	XSaa_static_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XSaa_static_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XSaa_static_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

