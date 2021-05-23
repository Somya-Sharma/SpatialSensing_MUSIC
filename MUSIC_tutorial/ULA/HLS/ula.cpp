
#include "ula.h"

void ULA_STREAM (hls::stream<in_str> &in_stream, hls::stream<out_str> &out_stream)
{
	#pragma HLS INTERFACE s_axilite port=return     bundle=CONTROL_BUS
	#pragma HLS INTERFACE axis      port=out_stream
	#pragma HLS INTERFACE axis      port=in_stream

	HLS_COMPLEX Y_in[L][K]; //Input Matrix
	HLS_COMPLEX Se_in[L][181]; //Input Sensing Matrix
	HLS_COMPLEX R_yy[L][L]; //Autocorrelation Output
	HLS_COMPLEX noise_eigVec[L][DIMC_noise]; //noise eigenvectors
	int doa_out[M];
	out_str valOut;
	in_str valIn;

	// take Y_in as input
	L11_inY:for  (int i = 0 ; i < L ; i++)
	{
		L12_inY:for (int j = 0 ; j < K ; j++)
		{
#pragma HLS PIPELINE
			valIn = in_stream.read();
			Y_in[i][j].real(valIn.val);
		}
	}

	//take Y_in as input
	L21_inY:for  (int i = 0 ; i < L ; i++)
	{
		L22_inY:for (int j = 0 ; j < K ; j++)
		{
#pragma HLS PIPELINE
			valIn = in_stream.read();
			Y_in[i][j].imag(valIn.val);
		}
	}

	//take S_e as input
	L31_inSe:for  (int i = 0 ; i < L ; i++)
	{

		L32_inSe:for (int j = 0 ; j < 181 ; j++)
		{
#pragma HLS PIPELINE
			valIn = in_stream.read();
			Se_in[i][j].real(valIn.val);
		}
	}

	//take S_e as input
	L41_inSe:for  (int i = 0 ; i < L ; i++)
	{
		L42_inSe:for (int j = 0 ; j < 181 ; j++)
		{
#pragma HLS PIPELINE
			valIn = in_stream.read();
			Se_in[i][j].imag(valIn.val);
		}
	}

	auto_correlation_Opt1<HLS_COMPLEX,type,L,K>(Y_in, R_yy);
	EVD<HLS_COMPLEX,float,L,DIMC_noise>(R_yy, noise_eigVec);
	MSG<HLS_COMPLEX,type,L,DIMC_noise>(Se_in,noise_eigVec,doa_out);

	//write output to stream
	L_Out2Stream:for (int j = 0 ; j < M ; j++)
	{
		valOut.val=doa_out[j];
		valOut.last = (j==(M-1));
		out_stream.write(valOut);
	}

	return;
}
