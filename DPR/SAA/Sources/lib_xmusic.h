
#ifndef H_LIB_EXAMPLE_HW_H
#define H_LIB_EXAMPLE_HW_H

#define DIMR      	 4
#define DIMC      	 100
#define L         	 6
#define SIZE1 	 	 (DIMR*DIMC)
#define SIZE2    	 L*181
//#define eigVec   	 4
//#define SIZE3    	 eigVec*eigVec
#define SIZE_indices 11

//#define RowsA DIMR
//#define ColsA DIMC
//#define N DIMC

struct complex
{
	float real;
	float imag;
};

typedef struct complex DATA_TYPE;


//extern float A_real_in[L][181];
//extern float A_imag_in[L][181];
//extern float X_real_in[10][1000];
//extern float X_imag_in[10][1000];

//

int Setup_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[DIMR][DIMC], float X_imag[DIMR][DIMC], float A_real[L][181], float A_imag[L][181], int res_hw[dma_size_out]);
void Start_HW_Accelerator();
int Run_HW_Accelerator(int dma_size1, int dma_size2, int dma_size_out, float X_real[DIMR][DIMC], float X_imag[DIMR][DIMC], float A_real[L][181], float A_imag[L][181],  int res_hw[dma_size_out]);
//void auto_correlation(struct complex A[DIMR][DIMC], struct complex OUT[DIMR][DIMR]);
//void matrix_mult (int DIMRa, int DIMCa, int DIMCb, struct complex A[DIMRa][DIMCa], struct complex B[DIMCa][DIMCb], struct complex OUT[DIMRa][DIMCb]);
//void column_extract(struct complex mat[L][181], int ind, struct complex col[L][1]);
//void qrf_top(struct complex X_in[DIMR][DIMC], struct complex noise_eigVec[DIMR][DIMC_VEC]);
//void p_calc(struct complex A[L][181], struct complex V[DIMR][DIMC_VEC], float P[181], int ind_out[M]);
//void top_software(DATA_TYPE X[DIMR][DIMC], DATA_TYPE A[L][181], int ind_out[M]);

#endif
