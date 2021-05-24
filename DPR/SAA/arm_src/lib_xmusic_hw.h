
#ifndef H_LIB_EXAMPLE_HW_H
#define H_LIB_EXAMPLE_HW_H

/*
Data Size parameters
 */
#define L		4		//no. of antennae
#define K		100		//no. of samples
#define LL		6		//rows of sensing matrix
#define SIZE1	L*K		//input matrix size
#define SIZE2	LL*181	//sensing matrix size

//function definitions
int Setup_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[L][K], float X_imag[L][K], float A_real[LL][181], float A_imag[LL][181], int res_hw[dma_size_out]);
void Start_HW_Accelerator();
int Run_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[L][K], float X_imag[L][K], float A_real[LL][181], float A_imag[LL][181], int res_hw[dma_size_out]);

#endif
