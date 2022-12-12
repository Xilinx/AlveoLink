`timescale 1ns/1ps

module axi_address_remap
(
  input  [ 33:0] s_hbm_araddr,
  input  [ 33:0] s_hbm_awaddr,

  output [ 33:0] m_hbm_araddr,
  output [ 33:0] m_hbm_awaddr,

  input  [ 33:0] start_address
);

assign m_hbm_araddr   = s_hbm_araddr | start_address;
assign m_hbm_awaddr   = s_hbm_awaddr | start_address;

endmodule