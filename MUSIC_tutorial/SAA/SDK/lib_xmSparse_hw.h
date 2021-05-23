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
#define L         6
#define OUT_DIM   M
#define SIZE1 	 (DIMR*DIMC)
#define OUT_SIZE M
#define SIZE2    (L*181)
#define DIMC_VEC 4
#define ind_rem 11
#define N DIMC

#define ind_rem 11
#define N_new 6

#define ss_ind ((int) (ind_rem/2 + ind_rem%2))
#define row_matr ((int) (ind_rem-ss_ind + 1))

#define RowsA 6
#define ColsA 6

#define ROWS  	  6
#define COLS      6

struct cmplx
{
	float real;
	float imag;
};

/*Defination of complex data type in C*/
typedef struct cmplx DATA_TYPE;

int Setup_HW_Accelerator(float X_real[DIMR][DIMC],float X_imag[DIMR][DIMC],float A_real[L][181],float A_imag[L][181],float indices[ind_rem], int res_hw[OUT_DIM],int dma_size1,int dma_size2,int dma_size3,int dma_size_out);
int Run_HW_Accelerator(float X_real[DIMR][DIMC],float X_imag[DIMR][DIMC],float A_real[L][181],float A_imag[L][181],float indices[ind_rem], int res_hw[OUT_DIM],int dma_size1,int dma_size2,int dma_size3,int dma_size_out);
void Start_HW_Accelerator(void);

void top_software (DATA_TYPE X[DIMR][DIMC], DATA_TYPE A[L][181], int indices[9], int ind_out[M]);
void matrix_mult (int DIMRa, int DIMCa, int DIMCb, DATA_TYPE A[DIMRa][DIMCa], DATA_TYPE B[DIMCa][DIMCb], DATA_TYPE OUT[DIMRa][DIMCb]);
void qrf(DATA_TYPE A_in[row_matr][row_matr], DATA_TYPE noise_eigVec[row_matr][DIMC_VEC]);
void auto_correlation (DATA_TYPE A[DIMR][DIMC], DATA_TYPE OUT[DIMR][DIMR]);
void auto_correlation1 (DATA_TYPE A[row_matr][N_new], DATA_TYPE OUT[row_matr][row_matr]);
void column_extract(DATA_TYPE mat[L][181], int ind, DATA_TYPE col[L][1]);
void MSG(DATA_TYPE A[L][181], DATA_TYPE V[row_matr][DIMC_VEC], int doa_out[M]);
void redundancy_removal(DATA_TYPE out[DIMR][DIMR], DATA_TYPE out_final[ind_rem], int indices [ind_rem]);
void matrix_form(DATA_TYPE out_final[ind_rem], DATA_TYPE matrix[N_new][N_new]);

float absolute_self(float inp);
void qrf_sft_mm(DATA_TYPE G[2][2], DATA_TYPE *a, DATA_TYPE *b);
void qrf_sft_mm_or_mag(DATA_TYPE G[2][2], DATA_TYPE *a, DATA_TYPE *b, DATA_TYPE mag, int use_mag, int extra_pass);
void qrf_sft_givens(int extra_pass, DATA_TYPE a, DATA_TYPE b, DATA_TYPE* c, DATA_TYPE* s, DATA_TYPE* ss, DATA_TYPE* cc, DATA_TYPE* r);
void qrf_sft_basic(DATA_TYPE  A[RowsA][ColsA], DATA_TYPE Q[RowsA][RowsA], DATA_TYPE R[RowsA][ColsA]);


#endif
