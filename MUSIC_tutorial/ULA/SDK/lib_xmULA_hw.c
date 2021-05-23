#include "platform.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xaxidma.h"
#include "xula_stream.h"
#include "lib_xmULA_hw.h"
#include "xil_printf.h"
#include <stdbool.h>
#include <stdio.h>
#include "math.h"

volatile static int RunExample = 0;
volatile static int ResultExample = 0;

XUla_stream xula_dev;

XUla_stream_Config xula_config = {
	0,
	XPAR_ULA_STREAM_0_S_AXI_CONTROL_BUS_BASEADDR
};

//Interrupt Controller Instance
XScuGic ScuGic;

// AXI DMA Instance
extern XAxiDma AxiDma;

//Configure ULA_Stream IP
int XUlaSetup(){
	return XUla_stream_CfgInitialize(&xula_dev,&xula_config);
}

void XUlaStart(void *InstancePtr){
	XUla_stream *pExample = (XUla_stream *)InstancePtr;
	XUla_stream_InterruptEnable(pExample,1);
	XUla_stream_InterruptGlobalEnable(pExample);
	XUla_stream_Start(pExample);
}

void XUlaIsr(void *InstancePtr){
	XUla_stream *pExample = (XUla_stream *)InstancePtr;

	//Disable the global interrupt
	XUla_stream_InterruptGlobalDisable(pExample);
	
	//Disable the local interrupt
	XUla_stream_InterruptDisable(pExample,0xffffffff);

	// clear the local interrupt
	XUla_stream_InterruptClear(pExample,1);

	ResultExample = 1;
	// restart the core if it should run again
	if(RunExample){
		XUlaStart(pExample);
	}
}

int XUlaSetupInterrupt()
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
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&ScuGic);
	//Enable the exception handler
	Xil_ExceptionEnable();
	// Connect the Adder ISR to the exception table
	result = XScuGic_Connect(&ScuGic,XPAR_FABRIC_ULA_STREAM_0_INTERRUPT_INTR,(Xil_InterruptHandler)XUlaIsr,&xula_dev);
	if(result != XST_SUCCESS){
		return result;
	}
	//Enable the Adder ISR
	XScuGic_Enable(&ScuGic,XPAR_FABRIC_ULA_STREAM_0_INTERRUPT_INTR);
	return XST_SUCCESS;
}

int Setup_HW_Accelerator(float X_real[DIMR][DIMC],float X_imag[DIMR][DIMC],float A_real[L][181],float A_imag[L][181], int doa_out[M],int dma_size1,int dma_size2,int dma_size_out)
{
	//SetUp the HLS Vivado IP
	int status = XUlaSetup();
	if(status != XST_SUCCESS){
		print("Error: example setup failed\n");
		return XST_FAILURE;
	}
	status =  XUlaSetupInterrupt();
	if(status != XST_SUCCESS){
		print("Error: interrupt setup failed\n");
		return XST_FAILURE;
	}
	
	XUlaStart(&xula_dev);

	//flush the cache
	Xil_DCacheFlushRange((unsigned int)X_real,dma_size1);
	Xil_DCacheFlushRange((unsigned int)X_imag,dma_size1);
	Xil_DCacheFlushRange((unsigned int)A_real,dma_size2);
	Xil_DCacheFlushRange((unsigned int)A_imag,dma_size2);
	Xil_DCacheFlushRange((unsigned int)doa_out,dma_size_out);
	print("\rCache cleared\n\r");

	return 0;
}

void Start_HW_Accelerator(void)
{
	int status = XUlaSetup();
	if(status != XST_SUCCESS){
		print("Error: example setup failed\n");
		return XST_FAILURE;
	}
	status =  XUlaSetupInterrupt();
	if(status != XST_SUCCESS){
		print("Error: interrupt setup failed\n");
		return XST_FAILURE;
	}
	XUlaStart(&xula_dev);
}

int Run_HW_Accelerator(float X_real[DIMR][DIMC],float X_imag[DIMR][DIMC],float A_real[L][181],float A_imag[L][181], int doa_out[M],int dma_size1,int dma_size2,int dma_size_out)
{

	//transfer X_in real to the Vivado HLS block
	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) X_real, dma_size1, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)) ;

	//transfer X_in imag to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) X_imag, dma_size1, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)) ;

	//transfer A_in real to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) A_real, dma_size2, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)) ;

	//transfer A_in imag to the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) A_imag, dma_size2, XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)) ;


	//get results from the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) doa_out, dma_size_out, XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
		print("Error: DMA transfer from Vivado HLS block failed\n");
		return XST_FAILURE;
	}

	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));

	//poll the DMA engine to verify transfers are complete
	/* Waiting for data processing */
	/* While this wait operation, the following action would be done
	 * First: Second Input will be sent.
	 * After: Output will be compute.
	 * Then: Output matrix will be sent from the accelerator to DDR and
	 * it will be stored at the base address that you set in the first SimpleTransfer
	 */
	while ((XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)) || (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)))
	{

	};

	return 0;
}

/******************SOFTWARE FUNCTIONS DEFINED FOR ULA MUSIC******************/
/******************QR FUNCTION START *****************/
float absolute_self(float inp)
{
	if (inp < 0)
		return (inp*(-1));
	else
		return inp;
}

void qrf_sft_mm(DATA_TYPE G[2][2], DATA_TYPE* a, DATA_TYPE* b)
{
	DATA_TYPE x, y;

	x.real = G[0][0].real*(*a).real - G[0][0].imag*(*a).imag  + G[0][1].real*(*b).real - G[0][1].imag*(*b).imag;
	x.imag = G[0][0].real*(*a).imag + G[0][0].imag*(*a).real  + G[0][1].real*(*b).imag + G[0][1].imag*(*b).real;

	y.real = G[1][0].real*(*a).real - G[1][0].imag*(*a).imag  + G[1][1].real*(*b).real - G[1][1].imag*(*b).imag;
	y.imag = G[1][0].real*(*a).imag + G[1][0].imag*(*a).real  + G[1][1].real*(*b).imag + G[1][1].imag*(*b).real;

	(*a).real = x.real;
	(*a).imag = x.imag;
	(*b).real = y.real;
	(*b).imag = y.imag;
}

void qrf_sft_mm_or_mag(DATA_TYPE G[2][2], DATA_TYPE *a, DATA_TYPE *b, DATA_TYPE mag, int use_mag, int extra_pass)
{
	DATA_TYPE x, y;

	x.real = G[0][0].real*(*a).real - G[0][0].imag*(*a).imag  + G[0][1].real*(*b).real - G[0][1].imag*(*b).imag;
	x.imag = G[0][0].real*(*a).imag + G[0][0].imag*(*a).real  + G[0][1].real*(*b).imag + G[0][1].imag*(*b).real;

	y.real = G[1][0].real*(*a).real - G[1][0].imag*(*a).imag  + G[1][1].real*(*b).real - G[1][1].imag*(*b).imag;
	y.imag = G[1][0].real*(*a).imag + G[1][0].imag*(*a).real  + G[1][1].real*(*b).imag + G[1][1].imag*(*b).real;

	if (use_mag == 0)
	{
		(*a).real = x.real;
		(*b).real = y.real;

		(*a).imag = x.imag;
		(*b).imag = y.imag;
	}
	else
	{
		if (extra_pass == 1)
		{
			(*b).real = mag.real;
			(*b).imag = 0;
		}
		else
		{
			(*a).real = mag.real;
			(*a).imag = 0;
			(*b).real = 0;
			(*b).imag = 0;
		}
	}
}

float qrf_sft_magnitude(DATA_TYPE a, DATA_TYPE b)
{
	const float ONE = 1.0;
	const float ZERO = 0.0;
	float ar, ai, br, bi, d1, d2, d3, x1, x2, x3;
	float largest;


	ar = absolute_self(a.real);
	ai = absolute_self(a.imag);
	br = absolute_self(b.real);
	bi = absolute_self(b.imag);

	if (ar >= ai && ar >= br && ar >= bi)
	{
		largest = ar;
		d1 = ai;
		d2 = br;
		d3 = bi;
	}
	else if (ai >= ar && ai >= br && ai >= bi)
	{
		largest = ai;
		d1 = ar;
		d2 = br;
		d3 = bi;
	}
	else if (br >= ar && br >= ai && br >= bi)
	{
		largest = br;
		d1 = ar;
		d2 = ai;
		d3 = bi;
	}
	else
	{
		largest = bi;
		d1 = ar;
		d2 = ai;
		d3 = br;
	}

	if (largest == ZERO)
	{ // Avoid divide-by-zero
		return ZERO;
	}
	else
	{
		x1 = d1/largest;
		x2 = d2/largest;
		x3 = d3/largest;
		float x1_sqd = x1*x1;
		float x2_sqd = x2*x2;
		float x3_sqd = x3*x3;

		float s1 = ONE + x1_sqd;
		float s2 = x2_sqd + x3_sqd;
		float s3 = s1 + s2;
		return largest*sqrtf(s3);
	}
}

void qrf_sft_givens(int extra_pass, DATA_TYPE a, DATA_TYPE b, DATA_TYPE* c, DATA_TYPE* s, DATA_TYPE* ss, DATA_TYPE* cc, DATA_TYPE* r)
{
	const float ONE = 1.0;
	const float ZERO = 0.0;
	const DATA_TYPE CZERO = {0, 0};
	float sqrt_mag_a_mag_b;
	DATA_TYPE c_tmp, s_tmp;

	if (extra_pass == 0)
	{
		// Standard modified Givens matrix, guarding against over-/underflow
		sqrt_mag_a_mag_b = qrf_sft_magnitude(a, b);

		if (absolute_self(a.real) == ZERO && absolute_self(a.imag) == ZERO && absolute_self(b.real) == ZERO && absolute_self(b.imag) == ZERO)
		{
			c_tmp.real = ONE*(a.real/absolute_self(a.real));
			c_tmp.imag = ZERO;
			s_tmp.real = ZERO;
			s_tmp.imag = ZERO;
		}
		else
		{
			c_tmp.real = a.real / sqrt_mag_a_mag_b;
			s_tmp.real = b.real / sqrt_mag_a_mag_b;

			c_tmp.imag = a.imag / sqrt_mag_a_mag_b;
			s_tmp.imag = b.imag / sqrt_mag_a_mag_b;
		}
		(*c).real = c_tmp.real;
		(*c).imag = -c_tmp.imag;

		(*cc).real = c_tmp.real;
		(*cc).imag = c_tmp.imag;

		(*s).real = s_tmp.real;
		(*s).imag = -s_tmp.imag;

		(*ss).real = -s_tmp.real;
		(*ss).imag = -s_tmp.imag;

		(*r).real = sqrt_mag_a_mag_b;
	}
	else
	{
		// floatransformation matrix to ensure real diagonal in R, guarding against over-/underflow
		sqrt_mag_a_mag_b = qrf_sft_magnitude(CZERO, b);

		c_tmp.real = ONE;
		c_tmp.imag = 0;

		if (absolute_self(b.real) == ZERO && absolute_self(b.imag) == ZERO)
		{

			s_tmp.real = ONE;
			s_tmp.imag = 0;
		}
		else
		{
			s_tmp.real = b.real / sqrt_mag_a_mag_b;
			s_tmp.imag = b.imag / sqrt_mag_a_mag_b;
		}


		(*c).real = c_tmp.real;
		(*c).imag = c_tmp.imag;
		(*cc).real = s_tmp.real;
		(*cc).imag = -s_tmp.imag;
		(*s).real = ZERO;
		(*s).imag = ZERO;
		(*ss).real = ZERO;
		(*ss).imag = ZERO;
		(*r).real = sqrt_mag_a_mag_b;
	}
}

void qrf_sft_basic(DATA_TYPE A[RowsA][RowsA], DATA_TYPE Q[RowsA][RowsA], DATA_TYPE R[RowsA][RowsA])
{
	// Buffers
	int r=0, j=0, k=0, c=0, i=0;
	DATA_TYPE Qi[RowsA][RowsA];
	DATA_TYPE Ri[RowsA][RowsA];
	DATA_TYPE G[2][2] = {{0, 0},{0, 0}};

	// Magnitude from Givens computation
	DATA_TYPE mag = {0, 0};

	// Flags for complex-valued case
	const int DO_UNITARY_TF = (RowsA == RowsA);
	int extra_pass = 0;

	// Initialize Qi and initialize/load Ri
	for(r = 0 ; r < RowsA ; r++)
	{
		for(c = 0 ; c < RowsA ; c++)
		{
			if (r == c)
				Qi[r][c].real = 1;
			else
				Qi[r][c].real = 0.0;
			Qi[r][c].imag = 0;
		}
		for(c = 0 ; c < RowsA ; c++)
		{
			Ri[r][c].real = A[r][c].real;
			Ri[r][c].imag = A[r][c].imag;
		}
	}

	for(j = 0 ; j < RowsA ; j++)
	{
	// For complex data and square matrices, we perform an additional pass to ensure that the diagonal of R is real
	// For non-square matrices, the modified Givens rotation ensures that the diagonal will be real-valued
		if (DO_UNITARY_TF == 1)
			if (j == RowsA-1)
				extra_pass = 1;
			else
				extra_pass = 0;
		else
		  extra_pass = 0;

		for(i = RowsA-1;  i > 0 ; i--)
		{
			if (i <= j-extra_pass)
			  continue;
			else
			{
				qrf_sft_givens(extra_pass, Ri[i-1][j], Ri[i][j], &G[0][0], &G[0][1], &G[1][0], &G[1][1], &mag);

				if (!extra_pass)
				{
					Ri[i-1][j].real = mag.real;
					Ri[i-1][j].imag = 0;
				}
				else
				{
					Ri[i][j].real = mag.real;
					Ri[i][j].imag = 0;
				}

				for(k = 0 ; k < RowsA ; k++)
				{
					if (k < j+1)
						continue;
					else
						qrf_sft_mm(G, &Ri[i-1][k], &Ri[i][k]);
				}

				for(k = 0 ; k < RowsA ; k++)
				{
					if (k < (i-(1+j)+extra_pass))
						continue;
					else
					{
//						printf("\nupdate q %d %d\n", i, k);
						qrf_sft_mm(G, &Qi[i-1][k], &Qi[i][k]);
					}
				}
			} // end if i<=j
		} // end qrf_sft_row_loop
	} // end qrf_sft_col_loop

	// Assign final outputs
	for(r = 0 ; r < RowsA; r++)
	{
		for(c = 0 ; c < RowsA ; c++)
		{
			Q[c][r].real = Qi[r][c].real;
			Q[c][r].imag = -Qi[r][c].imag;
		}
		for(c = 0 ; c < RowsA; c++)
		{
			if (r <= c)
			{
			// Values in the lower triangle are undefined
				R[r][c].real = Ri[r][c].real;
				R[r][c].imag = Ri[r][c].imag;
			}
			else
			{
				R[r][c].real = 0;
				R[r][c].imag = 0;
			}
		}
	}
}
//************************* QR FUNCTION END *************************//

//************************* ULA FUNCTIONS START *************************//
/*Function to multiply complex valued matrix A and B*/
void matrix_mult (int DIMRa, int DIMCa, int DIMCb, DATA_TYPE A[DIMRa][DIMCa], DATA_TYPE B[DIMCa][DIMCb], DATA_TYPE OUT[DIMRa][DIMCb])
{

	float sum_final_re = 0;
	float sum_final_im = 0;

	float sum_re = 0;
	float sum_im = 0;

	for (int i = 0 ; i < DIMRa ; i++)
	{
		for (int j = 0 ; j < DIMCb ; j++)
		{
			sum_final_re = 0.0f;
			sum_final_im = 0.0f;
			for (int k = 0 ; k < DIMCa ; k++)
			{
				sum_re = A[i][k].real*B[k][j].real;
				sum_re -= A[i][k].imag*B[k][j].imag;
				sum_final_re += sum_re;

				sum_im = A[i][k].real*B[k][j].imag;
				sum_im += A[i][k].imag*B[k][j].real;
				sum_final_im += sum_im;
			}
			OUT[i][j].real=(float)(sum_final_re);
			OUT[i][j].imag=(float)(sum_final_im);
		}
	}
}

/*QRF function for EVD. Here the eigenvalues and eigenvectors are computed for 
and then noise_eigenVectors are extracted from it.*/
void qrf(DATA_TYPE A_in[DIMR][DIMR], DATA_TYPE noise_eigVec[DIMR][DIMC_VEC])
{
	DATA_TYPE a_in[DIMR][DIMR];
	DATA_TYPE q_out[DIMR][DIMR];
	DATA_TYPE r_out[DIMR][DIMR];
	DATA_TYPE temp[DIMR][DIMR];
	DATA_TYPE eigen_vectors[DIMR][DIMR];

	int i=0, j=0;

	/*Initialize eigenvectors as identity matrix.*/
	for (i = 0 ; i < DIMR ; i++)
	{
		for (j = 0 ; j < DIMR ; j++)
		{
			a_in[i][j].real = A_in[i][j].real;
			a_in[i][j].imag = A_in[i][j].imag;
			if (i == j)
				eigen_vectors[i][j].real = 1;
			else
				eigen_vectors[i][j].real = 0;
			eigen_vectors[i][j].imag = 0;
		}
	}

	/*Iterative QRF*/
	for (i = 0 ; i < 2 ; i++)
	{
		qrf_sft_basic (a_in, q_out, r_out);
		matrix_mult (DIMR, DIMR, DIMR, eigen_vectors, q_out, temp);
		matrix_mult (DIMR, DIMR, DIMR, r_out, q_out, a_in);

		for (int k = 0 ; k < DIMR ; k++)
		{
			for (int j = 0 ; j < DIMR ; j++)
			{
				eigen_vectors[k][j].real = temp[k][j].real;
				eigen_vectors[k][j].imag = temp[k][j].imag;
			}
		}
	}

	/*Extract the noise eigenvectors*/
	for (i = 0 ; i < DIMR ; i++)
		for (j = 0 ; j < DIMC_VEC ; j++)
		{
			noise_eigVec[i][DIMC_VEC-1-j].real = eigen_vectors[i][j+M].real;
			noise_eigVec[i][DIMC_VEC-1-j].imag = eigen_vectors[i][j+M].imag;
		}

	return;
}

/*Performs autocorrelation of input A*/
void auto_correlation (DATA_TYPE A[DIMR][DIMC], DATA_TYPE OUT[DIMR][DIMR])
{
	float sum_final_re = 0, sum_final_im = 0;
	float sum_re = 0, sum_im = 0;

	for (int i = 0 ; i < DIMR ; i++)
	{
		for (int j = 0 ; j < DIMR ; j++)
		{
			sum_final_re = 0.0f;
			sum_final_im = 0.0f;
			for (int k = 0 ; k < DIMC ; k++)
			{
				sum_re = (A[i][k].real)*(A[j][k].real);
				sum_re -= (A[i][k].imag)*(-A[j][k].imag);
				sum_final_re += sum_re;

				sum_im = (A[i][k].real)*(-A[j][k].imag);
				sum_im += (A[i][k].imag)*(A[j][k].real);
				sum_final_im += sum_im;

			}
			OUT[i][j].real = (float)(sum_final_re)/N;
			OUT[i][j].imag = (float)(sum_final_im)/N;
		}
	}

}

/*Extract column--> It is used in MSG block*/
void column_extract(DATA_TYPE mat[L][181], int ind, DATA_TYPE col[L][1])
{
	for (int i = 0 ; i < L ; i++)
	{
		col[i][0].real = mat[i][ind].real;
		col[i][0].imag = mat[i][ind].imag;
	}
}

/*This block computes the peaks MUSIC Spectra and outputs the M doa value.*/
void MSG(DATA_TYPE A[L][181], DATA_TYPE V[DIMR][DIMC_VEC], float P[181], int doa_out[M])
{
	DATA_TYPE col [L][1];
	DATA_TYPE col_herm [1][L];
	DATA_TYPE out [1][DIMC_VEC];
	DATA_TYPE out_herm [DIMC_VEC][1];

	float out_max[M];

	DATA_TYPE out_f[1][1];

	float val;

	bool is_peak=0;
	bool flag=1;

	int i=0, j=0, k=0;

	for (i = 0 ; i < M ; i++)
	{
		out_max[i] = 0.0f;
		doa_out[i] = 0;
	}

	for (i = 0 ; i < 181 ; i++)
	{
		column_extract (A, i, col);

		for (k = 0 ; k < 1 ; k++)
			for (j = 0 ; j < L ; j++)
			{
				col_herm[k][j].real = col[j][k].real;
				col_herm[k][j].imag = -col[j][k].imag;
			}

		matrix_mult (1,DIMR,DIMC_VEC,col_herm, V, out);

		for (k = 0 ; k < DIMC_VEC ; k++)
			for (j = 0 ; j < 1 ; j++)
			{
				out_herm[k][j].real = out[j][k].real;
				out_herm[k][j].imag = -out[j][k].imag;
			}

		matrix_mult (1,DIMC_VEC,1,out, out_herm, out_f);
		
		val = 1/(sqrtf(out_f[0][0].real*out_f[0][0].real + out_f[0][0].imag*out_f[0][0].imag));
		P[i] = val;

		is_peak = 0;
		flag = 1;

		if((i > 1) & (P[i-1] > P[i]) & (P[i-1] > P[i-2]))
		{
			is_peak = 1;
		}
		if(is_peak)
		{
			for(j = 0 ; j < M ; j++)
			{
				if(flag & (P[i-1] > out_max[j]))
				{
					flag = 0;
					for(k = M - 1 ; k > j ; k--)
					{
						out_max[k] = out_max[k-1];
						doa_out[k] = doa_out[k-1];
					}
					out_max[j] = P[i-1];
					doa_out[j] = i-1;
				}
			}
		}
	}

	return;
}


//************************* SPARSE FUNCTIONS END *************************//

void top_software(DATA_TYPE X[DIMR][DIMC], DATA_TYPE A[L][181], int doa_out[M])
{
	DATA_TYPE auto_corr[DIMR][DIMR];
	DATA_TYPE eigVec[DIMR][DIMC_VEC];
	float P[181];

	auto_correlation (X, auto_corr);
	qrf(auto_corr, eigVec);
	MSG(A, eigVec, P, doa_out);
}


