`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/24/2021 06:03:01 PM
// Design Name: 
// Module Name: ULA_Reconfig
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ULA_Reconfig
(
    input ap_clk, input ap_rst_n,
    input [31:0] in_stream_TDATA,
    input in_stream_TVALID,
    input in_stream_TLAST,
    output in_stream_TREADY,
    
    output [31:0] out_stream_TDATA,
    output out_stream_TVALID,
    output out_stream_TLAST,
    input out_stream_TREADY
);

endmodule
