
#ifndef H_LIB_EXAMPLE_HW_H
#define H_LIB_EXAMPLE_HW_H

/*
DIMR x DIMC is the input size
M is the output size or the number of doas to be computed
L is the number of rows in sensing matrix
SIZE1 and SIZE2 are the size of input X and A respectively.
*/
#define DIMR      4
#define DIMC      100
#define M         2
#define L         4
#define OUT_DIM   M
#define SIZE1 	 (DIMR*DIMC)
#define OUT_SIZE M
#define SIZE2    L*181
#define DIMC_VEC DIMR-M

#define RowsA DIMR
#define ColsA DIMC
#define N DIMC


/*Defination of complex data type in C*/
struct complex
{
	float real;
	float imag;
};

typedef struct complex DATA_TYPE;

int Setup_HW_Accelerator(float X_real[DIMR][DIMC],float X_imag[DIMR][DIMC],float A_real[L][181],float A_imag[L][181], int doa_out[M], int dma_size1,int dma_size2,int dma_size_out);
int Run_HW_Accelerator(float X_real[DIMR][DIMC],float X_imag[DIMR][DIMC],float A_real[L][181],float A_imag[L][181], int doa_out[M], int dma_size1,int dma_size2,int dma_size_out);
void auto_correlation(struct complex A[DIMR][DIMC], struct complex OUT[DIMR][DIMR]);
void matrix_mult (int DIMRa,int DIMCa,int DIMCb,struct complex A[DIMRa][DIMCa],struct complex B[DIMCa][DIMCb],struct complex OUT[DIMRa][DIMCb]);
void column_extract(struct complex mat[L][181], int ind, struct complex col[L][1]);
void qrf(DATA_TYPE A_in[DIMR][DIMR], DATA_TYPE noise_eigVec[DIMR][DIMC_VEC]);
void MSG(struct complex A[L][181], struct complex V[DIMR][DIMC_VEC], float P[181], int doa_out[M]);
void top_software(DATA_TYPE X[DIMR][DIMC], DATA_TYPE A[L][181], int ind_out[M]);

#endif
