/*Add Header Files*/

#include "hls/linear_algebra/utils/x_hls_complex.h" /* in-build HLS complex number data-type*/
#include "hls_linear_algebra.h" /*The inbuild library includes matrix multiplication, QR decomposition and other functions*/
#include <stdbool.h> /*for boolean data-type*/
#include <ap_axi_sdata.h> /*Axi stream*/
#include <hls_stream.h>
#include <stdio.h>
#include <stdlib.h>

/*These are the parameters for the MUSIC-ULA Algorithm.
The input is a LxK matrix and LLx181 sensing matrix.
where LL is the sum of no. of physical and virtual antennas.
and K is the no. of samples.
M is the number of DOAs to be computed.
The output of the algorithm is the M DOAs.
*/

#define L 4
#define K 100
#define M 2

#define N K

#define ind_rem 11
#define N_new 6

#define ss_ind (ind_rem/2 + ind_rem%2)
#define row_matr (ind_rem-ss_ind + 1)

#define DIMC_noise (row_matr-M)
#define LL N_new

typedef float type;
/*defination of HLS Complex Data-type*/
typedef hls::x_complex<type> HLS_COMPLEX;

/*in_str is the structure defined for input stream.
The input stream are float values. last is the last
signal for AXI stream transaction.*/
struct in_str
{
	float val;
	ap_uint<1> last;
};

/*out_str is the structure defined for input stream.
The input stream are float values. last is the last
signal for AXI stream transaction.*/
struct out_str
{
	int val;
	ap_uint<1> last;
};

/*Function declaration for top function --> MUSIC(Sparse)*/
void MUSIC_SPARSE(hls::stream<in_str> &in_stream, hls::stream<out_str> &out_stream);

/*The defination for the complex conjugate multiplication
* The function returns the multiplication of complex numbers A and conjugate of B.
A=a+ib;
B=c+id'
C=A*conjugate(B)=(a+ib)*(c-id) = ac+bd + i(bc-ad)
*/
template <typename DATA_TYPE,typename T>
DATA_TYPE complex_mult_conj(DATA_TYPE A,DATA_TYPE B)
{
	#pragma HLS INLINE

	DATA_TYPE C;
	T a,b,c,d,ac,ad,bd,bc;
	a=A.real();
	b=A.imag();
	c=B.real();
	d=B.imag();

	ac=a*c;
	ad=a*d;

	bd=b*d;
	bc=b*c;

	C.real(ac+bd);
	C.imag(bc-ad);
	return C;
}

/*The defination for the complex multiplication
* The function returns the multiplication of complex numbers A and B.
A=a+ib;
B=c+id'
C=A*conjugate(B)=(a+ib)*(c+id) = ac-bd + i(bc+ad)
*/
template <typename DATA_TYPE,typename T>
DATA_TYPE complex_mult(DATA_TYPE A,DATA_TYPE B)
{
#pragma HLS INLINE
	DATA_TYPE C;
	T a,b,c,d,ac,ad,bd,bc;

	a=A.real();
	b=A.imag();
	c=B.real();
	d=B.imag();

	ac=a*c;
	ad=a*d;

	bd=b*d;
	bc=b*c;

	C.real(ac-bd);
	C.imag(bc+ad);
	return C;
}


template <typename DATA_TYPE,typename T>
DATA_TYPE complex_add(DATA_TYPE A,DATA_TYPE B)
{
#pragma HLS INLINE
	DATA_TYPE C;
	C.real(A.real()+B.real());
	C.imag(A.imag()+B.imag());

	return C;
}

/*AutoCorrelation function performs autocorrelation of the input matrix.
It is the matrix multiplication of input and its hermatian.
OUT(DIMRaxDIMRa)= A(DIMRaxDIMCa) * A_herm(DIMCaxDIMRa)

@param A is the input complex matrix
@param OUT is the output of the autocorrelation of A

@template DATA_TYPE defines the type of input matrix(complex in our case)
@template T is the type of values(float).
@template DIMRa --> Rows of input matrix
@template DIMCa --> Columns of input matrix
*/
template <typename DATA_TYPE, typename T, int DIMRa, int DIMCa>
void auto_correlation_Opt1(DATA_TYPE A[DIMRa][DIMCa], DATA_TYPE OUT[DIMRa][DIMRa])
{
#pragma HLS INLINE
	/*Optimization Details: Only the innermost loop has been pipelined
	to perform the operations concurrently.The other loops have not
	been pipelined to avoid unrolling of inner loops(Unrolling leads
	to significant increase in resources for larger size matrix
	multiplication)*/

	DATA_TYPE num,out;
	DATA_TYPE sum[DIMRa][DIMRa]; //array to accumulate sum
	/*Array partitioning to be done while unrolling innermost loop*/
//#pragma HLS ARRAY_PARTITION variable=A block factor=DIMRa dim=1
//#pragma HLS ARRAY_PARTITION variable=sum block factor=DIMRa dim=2

	/*Iterates over the columns of A and rows of A_herm*/
	AC_11:for (int i = 0 ; i < DIMCa ; i++)
	{
		/*Iterates over the elements of A_herm(i,:)*/
		AC_12:for (int j = 0 ; j < DIMRa ; j++)
		{
			AC_13:for (int k = 0 ; k < DIMRa ; k++)
			{
#pragma HLS PIPELINE
				/*Performs A(j,i)*A(k,i)*/
				num=complex_mult_conj<DATA_TYPE,T>(A[j][i],A[k][i]);
				if(i==0) /*Initialization at the begining*/
					sum[j][k]=num;
				else if(i==DIMCa-1) /*Summation at the end of A11 loop*/
				{
					out=complex_add<DATA_TYPE,T>(sum[j][k],num);
					OUT[j][k].real(out.real());
					OUT[j][k].imag(out.imag());
				}

				else /*keep on accumulating the summation*/
					sum[j][k]=complex_add<DATA_TYPE,T>(sum[j][k],num);
			}
		}
	}
}

/*matrixmult function performs autocorrelation of the input matrix.
It is the matrix multiplication of input and its hermatian.
OUT(DIMRaxDIMRa)= A(DIMRaxDIMCa) * B(DIMCaxDIMCb)

@param A and B are the input complex matrix
@param OUT is the output of the autocorrelation of A

@template DATA_TYPE defines the type of input matrix(complex in our case)
@template T is the type of values(float).
@template DIMRa --> Rows of input matrix
@template DIMCa --> Columns of input matrix
*/

template<typename DATA_TYPE, typename T, int DIMRa, int DIMCa,int DIMCb>
void matrix_mult(DATA_TYPE A[DIMRa][DIMCa],DATA_TYPE B[DIMCa][DIMCb],DATA_TYPE OUT[DIMRa][DIMCb])
{
#pragma HLS INLINE
#pragma HLS ALLOCATION instances=mul limit=10 operation

	DATA_TYPE num,out;
	DATA_TYPE sum[DIMRa][DIMCb];

	/*Iterates over the columns of A and rows of B*/
	C11:for (int i = 0 ; i < DIMCa ; i++)
	{
		/*Iterates over the elements of A(:,i)*/
		C12:for (int j = 0 ; j < DIMRa ; j++)
		{
			/*Iterates over the elements of B(i,:)*/
			C13:for (int k = 0 ; k < DIMCb ; k++)
			{
#pragma HLS PIPELINE
				/*Performs A(j,i)*B(i,k)*/
				num=complex_mult<DATA_TYPE,T>(A[j][i],B[i][k]);
				if(i==0) /*Initialization at the begining*/
					sum[j][k]=num;
				else if(i==DIMCa-1) /*Summation at the end of C11 loop*/
				{
					out=complex_add<DATA_TYPE,T>(sum[j][k],num);//sum[j][k]+num;
					OUT[j][k].real(out.real());
					OUT[j][k].imag(out.imag());
				}

				else /*keep on accumulating the summation*/
					sum[j][k]=complex_add<DATA_TYPE,T>(sum[j][k],num);			}
		}
	}
}

/*Similar to auto_correlation_Opt1 only difference here is in optimization
 * that innermost loop has been unrolled.*/
template <typename DATA_TYPE, typename T, int DIMRa, int DIMCa>
void auto_correlation_Opt2(DATA_TYPE A[DIMRa][DIMCa], DATA_TYPE OUT[DIMRa][DIMRa])
{
#pragma HLS INLINE
	DATA_TYPE num,out;
	DATA_TYPE sum[DIMRa][DIMRa];
//#pragma HLS ARRAY_PARTITION variable=A block factor=DIMRa dim=1
//#pragma HLS ARRAY_PARTITION variable=sum block factor=DIMRa dim=2

	AC_21:for (int i = 0 ; i < DIMCa ; i++)
	{
		AC_22:for (int j = 0 ; j < DIMRa ; j++)
		{

#pragma HLS PIPELINE
			AC_23:for (int k = 0 ; k < DIMRa ; k++)
			{
				num=complex_mult_conj<DATA_TYPE,T>(A[j][i],A[k][i]);
				if(i==0)
					sum[j][k]=num;
				else if(i==DIMCa-1)
				{
					out=complex_add<DATA_TYPE,T>(sum[j][k],num);
					OUT[j][k].real(out.real());
					OUT[j][k].imag(out.imag());
				}

				else
					sum[j][k]=complex_add<DATA_TYPE,T>(sum[j][k],num);
			}
		}
	}
}

/*This function is part of sparse pre-processing and it removes the
redundancy and form the spatially smoothed matrix from the remaining
elements.

@param 	  out is the input complex matrix (the output of auto-correlation
		  block)
@param 	  indices is the array contains the indices of elements of vectorized
	   	  out which are to kept
@param	  matrix is the output matrix after redundnancy removal and matrix
	   	  formation of size L'xL'

@template DATA_TYPE is the inbuilt HLS complex data type
@template T is the float data type
@template row_new is the row dimension for the output matrix
@template col_new is the column dimension for the output matrix
		   (here row_new=col_new)

@return   void
*/
template<typename DATA_TYPE,typename T,int row_new, int col_new>
void redundancy_removal(DATA_TYPE out[L][L], int indices [ind_rem], DATA_TYPE matrix[col_new][col_new])
{
#pragma HLS INLINE

/*Optimization details: MF_label0 can be executed only when RR_label0
loop is completed. Hence they cannot be parallelized completely. Since
single iteration of both the loops take more than one clock cycle to
execute, they can be pipelined for higher throughput.*/

	int indi = 0, indj = 0;

	//vector after removing redundant elements from out
	DATA_TYPE out_final[ind_rem];

	//redundancy removal loop
	RR_label0:for (int i = 0 ; i < ind_rem ; i++)
	{
#pragma HLS PIPELINE
/*elements only at the index of indices[i] are to be kept for further
executions indi and indj are used to compute the address (row, column)
of the element in matrix out.
*/
		indi = (indices[i]-1)/L;
		indj = (indices[i]-1)%L;

		out_final[i].real(out[indi][indj].real());
		out_final[i].imag(-out[indi][indj].imag());
	}

//matrix formation loop
	MF_label0:for (int i = 0 ; i < col_new ; i++)
	{
#pragma HLS PIPELINE
/*the elements from out_final vector from RR_label0 are rearranged
to form an output matrix
*/
		MF_label1:for (int j = 0 ; j < row_new ; j++)
		{
			int ind=ss_ind+j-1-i;
			matrix[j][i].real(out_final[ind].real());
			matrix[j][i].imag(out_final[ind].imag());
		}
	}

	return;
}

/*EVD performs the eigen value decomposition by QR decomposition method and
sorting of noise eigenvectors from the computed eigenvectors. With two
iteration of QR decomposition, the A_in matrix converges to a diagonal matrix
with diagonal elements as eigenvalues and the product of consequent
Qs gives the eigenvectors for the corresponding eigenvalues.

@param A_in --> Input matrix
@param noise_eigVec --> Extracted noise eigenvectors

@template DATA_TYPE defines the type of input matrix(complex in our case)
@template T is the type of values(float).
@template DIMRa --> Rows=columns of input matrix, A_in
@template DIMC_VEC --> Columns of noise_eigVec
*/

template <typename DATA_TYPE, typename T,int DIMRa, int DIMC_VEC>
void EVD(DATA_TYPE A_in[DIMRa][DIMRa], DATA_TYPE noise_eigVec[DIMRa][DIMC_VEC])
{

#pragma HLS INLINE
	DATA_TYPE q_out[DIMRa][DIMRa];
	DATA_TYPE a_in_local[DIMRa][DIMRa];
	DATA_TYPE r_out[DIMRa][DIMRa];
	DATA_TYPE temp[DIMRa][DIMRa];
	DATA_TYPE eigen_vectors[DIMRa][DIMRa];

	/*Initialize the eigen_vectors as identity matrix*/
	QRF_IL0:for (int i = 0 ; i < DIMRa ; i++)
	{
#pragma HLS PIPELINE
		QRF_IL1:for (int j = 0 ; j < DIMRa ; j++)
		{
			if (i == j)
				eigen_vectors[i][j].real(1);
			else
				eigen_vectors[i][j].real(0);
			eigen_vectors[i][j].imag(0);

			a_in_local[i][j].real(A_in[i][j].real());
			a_in_local[i][j].imag(A_in[i][j].imag());
		}
	}


	const bool TRANSPOSED_Q = false;

	QRF_mainLoop:for (int i = 0 ; i < 2 ; i++)
	{
		/*Inbuild QRF function used to decompose the input A_in as a product
		of q_out(orthogonal matrix) and r_out(lower triangular matrix)*/
		hls::qrf<TRANSPOSED_Q, DIMRa, DIMRa, HLS_COMPLEX, HLS_COMPLEX>(a_in_local, q_out, r_out);

		/*Multiply consecutive Q to get the eigenvectors*/
		hls::matrix_multiply<hls::NoTranspose, hls::NoTranspose, DIMRa, DIMRa, DIMRa, DIMRa, DIMRa, DIMRa, DATA_TYPE, DATA_TYPE>(eigen_vectors, q_out, temp);
		hls::matrix_multiply<hls::NoTranspose, hls::NoTranspose, DIMRa, DIMRa, DIMRa, DIMRa, DIMRa, DIMRa, DATA_TYPE, DATA_TYPE>(r_out, q_out, a_in_local);

		/*Update eigen_vectors*/
		QRF_updateEV_0:for (int k = 0 ; k < DIMRa ; k++)
		{
#pragma HLS PIPELINE
			QRF_updateEV_1:for(int j = 0 ; j < DIMRa ; j++)
			{
				eigen_vectors[k][j].real(temp[k][j].real());
				eigen_vectors[k][j].imag(temp[k][j].imag());
			}
		}
	}

	/*The eigenvectors corresponding to the lowest (LL-M)
	eigenvalues are taken as noise eigen_vectors*/
	Noise_EV1:for (int i = 0 ; i < DIMRa ; i++)
	{
		Noise_EV2:for (int j = 0 ; j < DIMC_VEC ; j++)
		{
#pragma HLS PIPELINE
			int index=DIMRa-j-1;
			noise_eigVec[i][j].real(eigen_vectors[i][index].real());
			noise_eigVec[i][j].imag(eigen_vectors[i][index].imag());
		}
	}


	return;
}

/*
This file contains all the helper functions required to compute peaks spectra

/*This function extracts ind_th column of sensring matrix S_e and outputs the
hermitian of the column

@param	  mat is the input matrix S_e of size Lx181
@param	  ind is the column to be extracted
@param	  col is the hermitian of the ind_th column 1xL

@template DATA_TYPE is the data type of S_e which is complex in this case

@return   void
*/
template <typename DATA_TYPE>
void column_extract(DATA_TYPE mat[L][181], int ind, DATA_TYPE col[1][LL])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=mat block factor=4 dim=1

/*Optimization details: the input matrix S_e is partitioned along the row
with factor 4.
The loop col_ext is pipelined such that when the data read from mat is
being written to the col, another data will be loaded thereby improving
the throughput
*/
	col_ext:for (int i = 0 ; i < LL ; i++)
	{
#pragma HLS PIPELINE
		col[0][i].real(mat[i][ind].real());
		col[0][i].imag(-mat[i][ind].imag());
	}
}

template<typename DATA_TYPE,typename T>
T complex_mode(DATA_TYPE a)
{
//#pragma HLS ALLOCATION instances=add limit=2 operation
#pragma HLS INLINE

	T re,im,re_sq,im_sq;
	re=a.real();
	im=a.imag();
	re_sq=re*re;
	im_sq=im*im;
	return (re_sq+im_sq);
}

/*
Comparator
@param	  num1 is the first input
@param	  num1 is the second input

@template T is the data type of input values, here it is float

@return   boolean value. 1 if num1 is greater than num2, 0 otherwise
*/
template<typename T>
bool greaterthan(T num1, T num2)
{
#pragma HLS INLINE
	if(num1 > num2)
		return 1;
	else
		return 0;
}

/*
This function checks whether a value in MUSIC spectra is a peak or not.
If yes, then it is added to the array of peaks at a location such that
the peaks array is maintained in the descending order.

@param	  doa is the array maintained of the indices of the peaks (in
		  peaks_array). It is updated whenever new peak is added
@param	  peaks_array is the array maintained of the peaks (should
		  always be in descending order)
@param	  peak is the peak to be inserted in the peaks_array
@param	  index is the index of the peak

@template T is the data type of the PMUSIC spectra which is float here

@return   void
*/
template<typename T>
void get_doa(int doa[M], T peaks_array[M], T peak, int index)
{
#pragma HLS INLINE

	bool flag = 1;	//peak insertion to be done only once

	Pcal_sort1: for(int j = 0 ; j < M ; j++)
	{
/*
Optimization Details: inner if block is pipelined to enable partial
parallelization of the operations and hence better throughput.
*/
		if(flag & greaterthan(peak, peaks_array[j]))
		{
/*if the peak hasn't been inserted yet and is greater than any of the
existing peaks, it will be inserted at that particular position and
the rest of the peaks will be shifted one position to the right such
that peaks_array is in descending order (last peak in original peaks_array
will be removed now!)
For ex. peaks_array = {9.6, 8.13, 5.48, 3.22}
		peak = 7.46
		after this block,
		peaks_array = {9..6, 8.13, 7.46, 5.48}
*/
#pragma HLS PIPELINE
			flag = 0;
			Pcal_sort2: for(int k = M-1 ; k > j ; k--)
			{
				peaks_array[k] = peaks_array[k-1];
				doa[k] = doa[k-1];
			}
			peaks_array[j] = peak;
			doa[j] = index;
		}
	}
}

/*
Checks whether one of the inputs is greater than the other two
(used to check whether a PMUSIC entry is peak or not)

@param	  val is the first input (middle value in PMUSIC)
@param	  val_prev is the second input (left value in PMUSIC)
@param	  val_next is the third input (right value in PMUSIC)

@template T is the data type of input values, here it is float

@return   boolean value. 1 if val is peak or not
*/
template<typename T>
bool is_peak(T val, T val_prev, T val_next)
{
#pragma HLS INLINE
	if((val > val_prev) & (val > val_next))
		return 1;
	else
		return 0;
}

/*This function generated the PMUSIC Spectra and gives the final doa as output
The input to this block is sensing matrix and the noise eigen vectors.

@param A --> Input Sensing matrix
@param noise_eigVec --> Extracted noise eigenvectors

@template DATA_TYPE defines the type of input matrix(complex in our case)
@template T is the type of values(float).
@template L --> Rows input matrix, A
@template DIMC_VEC --> Columns of noise_eigVec */

template <typename DATA_TYPE, typename T,int DIMRa, int DIMC_VEC>
void MSG(DATA_TYPE A[L][181], DATA_TYPE V[DIMRa][DIMC_VEC],T P[181])
{

#pragma HLS INLINE

	DATA_TYPE col_herm [1][LL];  //hermatian of col
	DATA_TYPE out[1][DIMC_VEC];
	DATA_TYPE out_f[1][1];

	pcal_loop:for (int i = 0 ; i < 181 ; i++)
	{
	#pragma HLS PIPELINE
		column_extract<DATA_TYPE>(A, i, col_herm);

		/*Multiplication of Hermatian of A(:,i) and V(Noise eigenvectors)*/
		matrix_mult<DATA_TYPE,T,1,DIMRa,DIMC_VEC>(col_herm,V,out);

		/*AutoCorrelation of the output*/
		auto_correlation_Opt2<DATA_TYPE,T,1,DIMC_VEC>(out,out_f);

		/*Calculate the peak --> Complex Modulus of the number.*/
		float temp=N*complex_mode<DATA_TYPE,float>(out_f[0][0]);

		/*P[i] is the peak at index i.*/
		P[i]=(T)(1/temp);
	}
	return;
}

/*This function sorts the peaks and maintains peaks_max array containing M maximum peaks*/
//P is MUSIC Spectra
template <typename T>
void p_sort(T P[181],int doa_out[M])
{
#pragma HLS INLINE

	//array that stores the maximum M peaks
	T peaks_max[M];
	/*Initialize of peaks_max*/
	init_peak:for(int i=0;i<M;i++)
	{

		peaks_max[i]=(T)0;
	}
	bool Is_peak=0;
	pcal_sort:for(int i = 1 ; i < 181 ; i++)
	{
	#pragma HLS PIPELINE
		/*P[i] is a peak if it is greater than its neigbours P[i-1] and P[i+1]*/
		Is_peak=is_peak<T>(P[i-1],P[i-2],P[i]);
		if(Is_peak)
		{
			/*This function continues sorting the peaks as we calculate them.*/
			get_doa<T>(doa_out,peaks_max,P[i-1],i-1);
		}
	}
}

template <typename DATA_TYPE,typename T,int indices_num,int DIMRa,int DIMCa>
void input_stream(hls::stream<in_str> &in_stream,DATA_TYPE X[DIMRa][DIMCa],DATA_TYPE A[LL][181],int indices[indices_num])
{
#pragma HLS INLINE
	in_str valIn;
	int Instream=2*(DIMRa*DIMCa + LL*181) + indices_num;
	int i = 0, j = 0;

	stream_in:for (int count = 1 ; count <= Instream ; count++)
	{
#pragma HLS PIPELINE
		valIn = in_stream.read();
		/*Read X from stream*/
		if (count <= DIMRa*DIMCa)
		{
			if (j == DIMCa)
			{
				j=0;
				i++;
			}
			X[i][j].real(valIn.val);
			if (count == DIMRa*DIMCa)
			{
				i = 0;
				j = 0;
			}
			else
			{
				j++;
			}
		}
		else if ((count > DIMRa*DIMCa) & (count <= 2*DIMRa*DIMCa))
		{
			if (j == DIMCa)
			{
				j=0;
				i++;
			}
			X[i][j].imag(valIn.val);
			if (count == 2*DIMRa*DIMCa)
			{
				i = 0;
				j = 0;
			}
			else
			{
				j++;
			}
		}
		/*Read A from stream*/
		else if ((count > 2*DIMRa*DIMCa) & (count <= (2*DIMRa*DIMCa) + (LL*181)))
		{
			if (j == 181)
			{
				j=0;
				i++;
			}
			A[i][j].real(valIn.val);
			if (count == (2* DIMRa*DIMCa)+(LL*181))
			{
				i = 0;
				j = 0;
			}
			else
			{
				j++;
			}

		}
		else if ((count > 2*DIMRa*DIMCa + LL*181) & (count <= (2*(DIMRa*DIMCa + LL*181))))
		{
			if (j == 181)
			{
				j=0;
				i++;
			}
			A[i][j].imag(valIn.val);
			if (count == 2*(DIMRa*DIMCa+LL*181))
			{
				i = 0;
				j = 0;
			}
			else
			{
				j++;
			}
		}
		else
		{
			indices[i] = (int) valIn.val;
			i++;
		}
	}

}

template <int doas>
void output_stream(hls::stream<out_str> &out_stream,int ind_out[doas])
{
#pragma HLS INLINE
	out_str valOut;
	stream_out:for (int i = 0 ; i < doas ; i++)
	{
		#pragma HLS PIPELINE
		valOut.val = ind_out[i];
		valOut.last = (i == 1);
		out_stream.write(valOut);
	}
}
