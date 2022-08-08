// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================

`timescale 1ns/1ps

module rx_qp_msn_V
#(parameter
    DataWidth    = 63,
    AddressWidth = 13,
    AddressRange = 8192
)(
    input  wire                    clk,
    input  wire                    reset,
    input  wire [AddressWidth-1:0] address0,
    input  wire                    ce0,
    output wire [DataWidth-1:0]    q0,
    input  wire [AddressWidth-1:0] address1,
    input  wire                    ce1,
    input  wire                    we1,
    input  wire [DataWidth-1:0]    d1
);
//------------------------Local signal-------------------
reg  [AddressRange-1:0] written = {AddressRange{1'b0}};
wire [DataWidth-1:0]    q0_ram;
wire [DataWidth-1:0]    q0_rom;
wire                    q0_sel;
reg  [0:0]              sel0_sr;
//------------------------Instantiation------------------
rx_qp_msn_V_ram rx_qp_msn_V_ram_u (
    .clk   ( clk ),
    .ce0   ( ce0 ),
    .addr0 ( address0 ),
    .q0    ( q0_ram ),
    .ce1   ( ce1 ),
    .addr1 ( address1 ),
    .we1   ( we1 ),
    .d1    ( d1 )
);
//------------------------Body---------------------------
assign q0     = q0_sel? q0_ram : q0_rom;
assign q0_sel = sel0_sr[0];
assign q0_rom = 63'b000000000000000000000000000000000000000000000000000000000000000;

always @(posedge clk) begin
    if (reset)
        written <= 1'b0;
    else begin
        if (ce1 & we1) begin
            written[address1] <= 1'b1;
        end
    end
end

always @(posedge clk) begin
    if (ce0) begin
        sel0_sr[0] <= written[address0];
    end
end

endmodule
