// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xula_stream_static.h"

extern XUla_stream_static_Config XUla_stream_static_ConfigTable[];

XUla_stream_static_Config *XUla_stream_static_LookupConfig(u16 DeviceId) {
	XUla_stream_static_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XULA_STREAM_STATIC_NUM_INSTANCES; Index++) {
		if (XUla_stream_static_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XUla_stream_static_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XUla_stream_static_Initialize(XUla_stream_static *InstancePtr, u16 DeviceId) {
	XUla_stream_static_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XUla_stream_static_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XUla_stream_static_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

