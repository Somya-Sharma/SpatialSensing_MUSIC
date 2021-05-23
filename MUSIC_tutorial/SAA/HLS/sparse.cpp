
#include "sparse.h"

void MUSIC_SPARSE (hls::stream<in_str> &in_stream, hls::stream<out_str> &out_stream)
{
	#pragma HLS INTERFACE s_axilite port=return     bundle=CONTROL_BUS
	#pragma HLS INTERFACE axis      port=out_stream
	#pragma HLS INTERFACE axis      port=in_stream

	HLS_COMPLEX Y_in[L][K]; //Input Matrix
	HLS_COMPLEX Se_in[LL][181]; //Input Sensing Matrix
	HLS_COMPLEX R_yy[L][L]; //Autocorrelation Output
	HLS_COMPLEX out_final[ind_rem]; //Output after vectorization and redundancy removal
	HLS_COMPLEX Y2[row_matr][N_new]; //Matrix Rearrangement
	HLS_COMPLEX R_yy2[LL][LL];
	HLS_COMPLEX noise_eigVec[LL][DIMC_noise];
	type P[181];
	int indices [ind_rem];
	int doa_out[M];

#pragma HLS DATAFLOW

	input_stream<HLS_COMPLEX,type,ind_rem,L,K>(in_stream,Y_in,Se_in,indices);
	auto_correlation_Opt1<HLS_COMPLEX,type,L,K>(Y_in,R_yy);
	redundancy_removal<HLS_COMPLEX,type, row_matr, N_new> (R_yy, indices, Y2);
	auto_correlation_Opt2<HLS_COMPLEX,type,row_matr, N_new>(Y2,R_yy2);
	EVD<HLS_COMPLEX,type,row_matr,DIMC_noise>(R_yy2,noise_eigVec);
	MSG<HLS_COMPLEX,type,row_matr, DIMC_noise>(Se_in, noise_eigVec,P);
	p_sort<type>(P,doa_out);
	output_stream<M>(out_stream,doa_out);

	return;
}
