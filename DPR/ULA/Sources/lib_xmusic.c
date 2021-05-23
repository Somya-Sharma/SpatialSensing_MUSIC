
#include "XUla_static.h"
#include "lib_xmusic.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "platform.h"
#include <stdbool.h>
#include "xscugic.h"
#include "xaxidma.h"
#include <stdio.h>
#include "math.h"

volatile static int RunExample = 0;
volatile static int ResultExample = 0;

XUla_static xmmult_dev;

XUla_static_Config xmmult_config = {
	0,
	XPAR_ULA_STATIC_0_S_AXI_CONTROL_BUS_BASEADDR
};

//Interrupt Controller Instance
XScuGic ScuGic;

// AXI DMA Instance
extern XAxiDma AxiDma;


int XMmultSetup(){
	return XUla_static_CfgInitialize(&xmmult_dev,&xmmult_config);
}

void XMmultStart(void *InstancePtr){
	XUla_static *pExample = (XUla_static *)InstancePtr;
	XUla_static_InterruptEnable(pExample,1);
	XUla_static_InterruptGlobalEnable(pExample);
	XUla_static_Start(pExample);
}


void XMmultIsr(void *InstancePtr){
	XUla_static *pExample = (XUla_static *)InstancePtr;

	//Disable the global interrupt
	XUla_static_InterruptGlobalDisable(pExample);
	//Disable the local interrupt
	XUla_static_InterruptDisable(pExample,0xffffffff);

	// clear the local interrupt
	XUla_static_InterruptClear(pExample,1);

	ResultExample = 1;
	// restart the core if it should run again
	if(RunExample){
		XMmultStart(pExample);
	}
}

int XMmultSetupInterrupt()
{
	//This functions sets up the interrupt on the ARM
	int result;
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (pCfg == NULL){
		print("Interrupt Configuration Lookup Failed\n\r");
		return XST_FAILURE;
	}
	result = XScuGic_CfgInitialize(&ScuGic,pCfg,pCfg->CpuBaseAddress);
	if(result != XST_SUCCESS){
		return result;
	}
	// self test
	result = XScuGic_SelfTest(&ScuGic);
	if(result != XST_SUCCESS){
		return result;
	}
	// Initialize the exception handler
	Xil_ExceptionInit();
	// Register the exception handler
	//print("Register the exception handler\n\r");
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&ScuGic);
	//Enable the exception handler
	Xil_ExceptionEnable();
	// Connect the Adder ISR to the exception table
	//print("Connect the Adder ISR to the Exception handler table\n\r");
	result = XScuGic_Connect(&ScuGic, XPAR_FABRIC_ULA_STATIC_0_INTERRUPT_INTR, (Xil_InterruptHandler)XMmultIsr, &xmmult_dev);
	if(result != XST_SUCCESS){
		return result;
	}
	//print("Enable the Adder ISR\n\r");
	XScuGic_Enable(&ScuGic,XPAR_FABRIC_ULA_STATIC_0_INTERRUPT_INTR);
	return XST_SUCCESS;
}

int Setup_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[DIMR][DIMC], float X_imag[DIMR][DIMC], float A_real[L][181], float A_imag[L][181], int res_hw[dma_size_out])
//Setup the Vivado HLS Block
{
	int status = XMmultSetup();
	if(status != XST_SUCCESS){
		print("Error: example setup failed\n");
		return XST_FAILURE;
	}

	status =  XMmultSetupInterrupt();
	if(status != XST_SUCCESS){
		print("Error: interrupt setup failed\n");
		return XST_FAILURE;
	}

	XMmultStart(&xmmult_dev);
//	xil_printf("post start\r\n");

	//flush the cache
	Xil_DCacheFlushRange((unsigned int)X_real, dma_size1);
	Xil_DCacheFlushRange((unsigned int)X_imag, dma_size1);
	Xil_DCacheFlushRange((unsigned int)A_real, dma_size2);
	Xil_DCacheFlushRange((unsigned int)A_imag, dma_size2);
	Xil_DCacheFlushRange((unsigned int)res_hw, dma_size_out);
	print("\rCache cleared\n\r");

	return 0;
}

void Start_HW_Accelerator(void)
{
	int status = XMmultSetup();
	if(status != XST_SUCCESS){
		print("Error: example setup failed\n");
		return XST_FAILURE;
	}
	status =  XMmultSetupInterrupt();
	if(status != XST_SUCCESS){
		print("Error: interrupt setup failed\n");
		return XST_FAILURE;
	}

	XMmultStart(&xmmult_dev);
}

int Run_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[DIMR][DIMC], float X_imag[DIMR][DIMC], float A_real[L][181], float A_imag[L][181], int res_hw[dma_size_out])
{

//	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) res_hw, dma_size_out, XAXIDMA_DEVICE_TO_DMA);

	//transfer X_in real to the Vivado HLS block
	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) X_real, dma_size1, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\r\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
//	xil_printf("transfer X real done \r\n");

	//transfer X_in imag to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) X_imag, dma_size1, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
//	xil_printf("transfer X imag done \r\n");

	//transfer A real to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma,(unsigned int) A_real,dma_size2,XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
//	xil_printf("transfer A real done \r\n");


	//transfer A imag to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma,(unsigned int) A_imag,dma_size2,XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\r\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
//	xil_printf("transfer A imag done \r\n");

	//get results from the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) res_hw, dma_size_out, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer from Vivado HLS block failed\r\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
//	xil_printf("transfer result done, waiting for dma now \r\n");

	//poll the DMA engine to verify transfers are complete
	/* Waiting for data processing */
	/* While this wait operation, the following action would be done
	 * First: Second matrix will be sent.
	 * After: Multiplication will be compute.
	 * Then: Output matrix will be sent from the accelerator to DDR and
	 * it will be stored at the base address that you set in the first SimpleTransfer
	 */
//	xil_printf("Device to DMA %d\r\n", XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));
//	xil_printf("DMA to device %d\r\n", XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));
	while ((XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)) || (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)))
	{
//		xil_printf("infinite\n");
	};
//	xil_printf("DMA no longer busy\r\n");

//	while (!ResultExample)
//		print("Waiting for core to finish\n\r");

	return 0;

}
