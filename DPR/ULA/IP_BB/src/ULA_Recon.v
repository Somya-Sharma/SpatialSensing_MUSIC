module ULA_Recon
(
    input ap_clk,
    input ap_rst_n,
    
    input in_stream_TVALID,
    output in_stream_TREADY,
    input [31:0] in_stream_TDATA,
    input in_stream_TLAST,
    
    output out_stream_TVALID,
    input out_stream_TREADY,
    output [31:0] out_stream_TDATA,
    output out_stream_TLAST
);

endmodule