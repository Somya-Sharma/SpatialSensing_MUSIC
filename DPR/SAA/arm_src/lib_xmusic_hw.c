
#include "xsaa_static.h"
#include "lib_xmusic_hw.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "platform.h"
#include "xaxidma.h"
#include <stdio.h>

//IP Instance creation
XSaa_static xula_dev;

//Initialise the configuration parameters of the IP
XSaa_static_Config xula_config = {
	0,
	XPAR_SAA_STATIC_0_S_AXI_CONTROL_BUS_BASEADDR
};

//AXI DMA Instance
extern XAxiDma AxiDma;

//Assign the configuration to the IP instance
int XUlaSetup()
{
	return XSaa_static_CfgInitialize(&xula_dev, &xula_config);
}

//Start the IP before it computes over the input data
void XUlaStart(void *InstancePtr)
{
	XSaa_static *pExample = (XSaa_static *)InstancePtr;
	XSaa_static_Start(pExample);
}

//SetUp the IP and flush the cache to store the input and output data since matrix size is large
int Setup_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[L][K], float X_imag[L][K], float A_real[LL][181], float A_imag[LL][181], int res_hw[dma_size_out])
{
	int status = XUlaSetup();
	if(status != XST_SUCCESS){
		print("Error: example setup failed\n");
		return XST_FAILURE;
	}

	XUlaStart(&xula_dev);

	//flush the cache
	Xil_DCacheFlushRange((unsigned int)X_real, dma_size1);
	Xil_DCacheFlushRange((unsigned int)X_imag, dma_size1);
	Xil_DCacheFlushRange((unsigned int)A_real, dma_size2);
	Xil_DCacheFlushRange((unsigned int)A_imag, dma_size2);
	Xil_DCacheFlushRange((unsigned int)res_hw, dma_size_out);
	print("\rCache cleared\n\r");

	return 0;
}

//Start the IP
void Start_HW_Accelerator(void)
{
	int status = XUlaSetup();
	if(status != XST_SUCCESS)
	{
		print("Error: example setup failed\n");
		return XST_FAILURE;
	}

	XUlaStart(&xula_dev);
}

//transfer the input data from DMA to the HLS IP and recieve the output data from the IP to DMA
int Run_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[L][K], float X_imag[L][K], float A_real[LL][181], float A_imag[LL][181], int res_hw[dma_size_out])
{

//transfer X_real to the Vivado HLS block
	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) X_real, dma_size1, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\r\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

//transfer X_imag to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) X_imag, dma_size1, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

//transfer A_real to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) A_real, dma_size2, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

//transfer A_imag to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) A_imag, dma_size2, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer to Vivado HLS block failed\r\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

//get results from the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) res_hw, dma_size_out, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS)
	{
		print("Error: DMA transfer from Vivado HLS block failed\r\n");
		return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

//poll the DMA engine to verify transfers are complete
/* Waiting for data processing */
/* While this wait operation, the following action would be done
 * First: Second matrix will be sent.
 * After: DoA will be compute.
 * Then: Output matrix will be sent from the accelerator to DDR and
 * it will be stored at the base address that you set in the first SimpleTransfer
 */
	while ((XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)) || (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)))
	{
//		xil_printf("infinite\n");
	};

	return 0;

}
