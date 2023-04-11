//Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2021.2.1 (lin64) Build 3414424 Sun Dec 19 10:57:14 MST 2021
//Date        : Thu Jul  7 23:58:00 2022
//Host        : grovfDEV running 64-bit Ubuntu 20.04.4 LTS
//Command     : generate_target HiveNet_wrapper.bd
//Design      : HiveNet_wrapper_Erik
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module HiveNet_kernel
   (HBM_read_araddr,
    HBM_read_arburst,
    HBM_read_arcache,
    HBM_read_arid,
    HBM_read_arlen,
    HBM_read_arlock,
    HBM_read_arprot,
    HBM_read_arqos,
    HBM_read_arready,
    HBM_read_arregion,
    HBM_read_arsize,
    HBM_read_arvalid,
    HBM_read_rdata,
    HBM_read_rid,
    HBM_read_rlast,
    HBM_read_rready,
    HBM_read_rresp,
    HBM_read_rvalid,
    HBM_write_awaddr,
    HBM_write_awburst,
    HBM_write_awcache,
    HBM_write_awlen,
    HBM_write_awlock,
    HBM_write_awprot,
    HBM_write_awqos,
    HBM_write_awready,
    HBM_write_awregion,
    HBM_write_awsize,
    HBM_write_awvalid,
    HBM_write_bready,
    HBM_write_bresp,
    HBM_write_bvalid,
    HBM_write_wdata,
    HBM_write_wlast,
    HBM_write_wready,
    HBM_write_wstrb,
    HBM_write_wvalid,
    S00_AXI_0_araddr,
    S00_AXI_0_arprot,
    S00_AXI_0_arready,
    S00_AXI_0_arvalid,
    S00_AXI_0_awaddr,
    S00_AXI_0_awprot,
    S00_AXI_0_awready,
    S00_AXI_0_awvalid,
    S00_AXI_0_bready,
    S00_AXI_0_bresp,
    S00_AXI_0_bvalid,
    S00_AXI_0_rdata,
    S00_AXI_0_rready,
    S00_AXI_0_rresp,
    S00_AXI_0_rvalid,
    S00_AXI_0_wdata,
    S00_AXI_0_wready,
    S00_AXI_0_wstrb,
    S00_AXI_0_wvalid,
    ap_clk,
    ref_clock,
    ap_clk_320,
    ap_clk_400,
    ap_rst_n,
    inputData_tdata,
    inputData_tdest,
    inputData_tkeep,
    inputData_tlast,
    inputData_tready,
    inputData_tstrb,
    inputData_tvalid,
    outData_tdata,
    outData_tdest,
    outData_tkeep,
    outData_tlast,
    outData_tready,
    outData_tstrb,
    outData_tvalid,
    resetn_320,
    resetn_400,
    rx_tdata,
    rx_tkeep,
    rx_tlast,
    rx_tready,
    rx_tuser,
    rx_tvalid,
    tx_tdata,
    tx_tkeep,
    tx_tlast,
    tx_tready,
    tx_tstrb,
    tx_tvalid);
  output [33:0]HBM_read_araddr;
  output [1:0]HBM_read_arburst;
  output [3:0]HBM_read_arcache;
  output [1:0]HBM_read_arid;
  output [7:0]HBM_read_arlen;
  output [0:0]HBM_read_arlock;
  output [2:0]HBM_read_arprot;
  output [3:0]HBM_read_arqos;
  input HBM_read_arready;
  output [3:0]HBM_read_arregion;
  output [2:0]HBM_read_arsize;
  output HBM_read_arvalid;
  input [255:0]HBM_read_rdata;
  input [1:0]HBM_read_rid;
  input HBM_read_rlast;
  output HBM_read_rready;
  input [1:0]HBM_read_rresp;
  input HBM_read_rvalid;
  output [33:0]HBM_write_awaddr;
  output [1:0]HBM_write_awburst;
  output [3:0]HBM_write_awcache;
  output [7:0]HBM_write_awlen;
  output [0:0]HBM_write_awlock;
  output [2:0]HBM_write_awprot;
  output [3:0]HBM_write_awqos;
  input HBM_write_awready;
  output [3:0]HBM_write_awregion;
  output [2:0]HBM_write_awsize;
  output HBM_write_awvalid;
  output HBM_write_bready;
  input [1:0]HBM_write_bresp;
  input HBM_write_bvalid;
  output [255:0]HBM_write_wdata;
  output HBM_write_wlast;
  input HBM_write_wready;
  output [31:0]HBM_write_wstrb;
  output HBM_write_wvalid;
  input [9:0]S00_AXI_0_araddr;
  input [2:0]S00_AXI_0_arprot;
  output S00_AXI_0_arready;
  input S00_AXI_0_arvalid;
  input [9:0]S00_AXI_0_awaddr;
  input [2:0]S00_AXI_0_awprot;
  output S00_AXI_0_awready;
  input S00_AXI_0_awvalid;
  input S00_AXI_0_bready;
  output [1:0]S00_AXI_0_bresp;
  output S00_AXI_0_bvalid;
  output [31:0]S00_AXI_0_rdata;
  input S00_AXI_0_rready;
  output [1:0]S00_AXI_0_rresp;
  output S00_AXI_0_rvalid;
  input [31:0]S00_AXI_0_wdata;
  output S00_AXI_0_wready;
  input [3:0]S00_AXI_0_wstrb;
  input S00_AXI_0_wvalid;
  input ap_clk;
  input ref_clock;
  input ap_clk_320;
  input ap_clk_400;
  input ap_rst_n;
  input [511:0]inputData_tdata;
  input [12:0]inputData_tdest;
  input [63:0]inputData_tkeep;
  input inputData_tlast;
  output inputData_tready;
  input [63:0]inputData_tstrb;
  input inputData_tvalid;
  output [511:0]outData_tdata;
  output [12:0]outData_tdest;
  output [63:0]outData_tkeep;
  output outData_tlast;
  input outData_tready;
  output [63:0]outData_tstrb;
  output outData_tvalid;
  input resetn_320;
  input resetn_400;
  input [511:0]rx_tdata;
  input [63:0]rx_tkeep;
  input rx_tlast;
  output rx_tready;
  input [0:0]rx_tuser;
  input rx_tvalid;
  output [511:0]tx_tdata;
  output [63:0]tx_tkeep;
  output tx_tlast;
  input tx_tready;
  output [63:0]tx_tstrb;
  output tx_tvalid;

  wire [33:0]HBM_read_araddr;
  wire [1:0]HBM_read_arburst;
  wire [3:0]HBM_read_arcache;
  wire [1:0]HBM_read_arid;
  wire [7:0]HBM_read_arlen;
  wire [0:0]HBM_read_arlock;
  wire [2:0]HBM_read_arprot;
  wire [3:0]HBM_read_arqos;
  wire HBM_read_arready;
  wire [3:0]HBM_read_arregion;
  wire [2:0]HBM_read_arsize;
  wire HBM_read_arvalid;
  wire [255:0]HBM_read_rdata;
  wire [1:0]HBM_read_rid;
  wire HBM_read_rlast;
  wire HBM_read_rready;
  wire [1:0]HBM_read_rresp;
  wire HBM_read_rvalid;
  wire [33:0]HBM_write_awaddr;
  wire [1:0]HBM_write_awburst;
  wire [3:0]HBM_write_awcache;
  wire [7:0]HBM_write_awlen;
  wire [0:0]HBM_write_awlock;
  wire [2:0]HBM_write_awprot;
  wire [3:0]HBM_write_awqos;
  wire HBM_write_awready;
  wire [3:0]HBM_write_awregion;
  wire [2:0]HBM_write_awsize;
  wire HBM_write_awvalid;
  wire HBM_write_bready;
  wire [1:0]HBM_write_bresp;
  wire HBM_write_bvalid;
  wire [255:0]HBM_write_wdata;
  wire HBM_write_wlast;
  wire HBM_write_wready;
  wire [31:0]HBM_write_wstrb;
  wire HBM_write_wvalid;
  wire [9:0]S00_AXI_0_araddr;
  wire [2:0]S00_AXI_0_arprot;
  wire S00_AXI_0_arready;
  wire S00_AXI_0_arvalid;
  wire [9:0]S00_AXI_0_awaddr;
  wire [2:0]S00_AXI_0_awprot;
  wire S00_AXI_0_awready;
  wire S00_AXI_0_awvalid;
  wire S00_AXI_0_bready;
  wire [1:0]S00_AXI_0_bresp;
  wire S00_AXI_0_bvalid;
  wire [31:0]S00_AXI_0_rdata;
  wire S00_AXI_0_rready;
  wire [1:0]S00_AXI_0_rresp;
  wire S00_AXI_0_rvalid;
  wire [31:0]S00_AXI_0_wdata;
  wire S00_AXI_0_wready;
  wire [3:0]S00_AXI_0_wstrb;
  wire S00_AXI_0_wvalid;
  wire ap_clk;
  wire ref_clock;
  wire ap_clk_320;
  wire ap_clk_400;
  wire ap_rst_n;
  wire [511:0]inputData_tdata;
  wire [12:0]inputData_tdest;
  wire [63:0]inputData_tkeep;
  wire inputData_tlast;
  wire inputData_tready;
  wire [63:0]inputData_tstrb;
  wire inputData_tvalid;
  wire [511:0]outData_tdata;
  wire [12:0]outData_tdest;
  wire [63:0]outData_tkeep;
  wire outData_tlast;
  wire outData_tready;
  wire [63:0]outData_tstrb;
  wire outData_tvalid;
  wire resetn_320;
  wire resetn_400;
  wire [511:0]rx_tdata;
  wire [63:0]rx_tkeep;
  wire rx_tlast;
  wire rx_tready;
  wire [0:0]rx_tuser;
  wire rx_tvalid;
  wire [511:0]tx_tdata;
  wire [63:0]tx_tkeep;
  wire tx_tlast;
  wire tx_tready;
  wire [63:0]tx_tstrb;
  wire tx_tvalid;

  HiveNet HiveNet_i
       (.HBM_read_araddr(HBM_read_araddr),
        .HBM_read_arburst(HBM_read_arburst),
        .HBM_read_arcache(HBM_read_arcache),
        .HBM_read_arid(HBM_read_arid),
        .HBM_read_arlen(HBM_read_arlen),
        .HBM_read_arlock(HBM_read_arlock),
        .HBM_read_arprot(HBM_read_arprot),
        .HBM_read_arqos(HBM_read_arqos),
        .HBM_read_arready(HBM_read_arready),
        .HBM_read_arregion(HBM_read_arregion),
        .HBM_read_arsize(HBM_read_arsize),
        .HBM_read_arvalid(HBM_read_arvalid),
        .HBM_read_rdata(HBM_read_rdata),
        .HBM_read_rid(HBM_read_rid),
        .HBM_read_rlast(HBM_read_rlast),
        .HBM_read_rready(HBM_read_rready),
        .HBM_read_rresp(HBM_read_rresp),
        .HBM_read_rvalid(HBM_read_rvalid),
        .HBM_write_awaddr(HBM_write_awaddr),
        .HBM_write_awburst(HBM_write_awburst),
        .HBM_write_awcache(HBM_write_awcache),
        .HBM_write_awlen(HBM_write_awlen),
        .HBM_write_awlock(HBM_write_awlock),
        .HBM_write_awprot(HBM_write_awprot),
        .HBM_write_awqos(HBM_write_awqos),
        .HBM_write_awready(HBM_write_awready),
        .HBM_write_awregion(HBM_write_awregion),
        .HBM_write_awsize(HBM_write_awsize),
        .HBM_write_awvalid(HBM_write_awvalid),
        .HBM_write_bready(HBM_write_bready),
        .HBM_write_bresp(HBM_write_bresp),
        .HBM_write_bvalid(HBM_write_bvalid),
        .HBM_write_wdata(HBM_write_wdata),
        .HBM_write_wlast(HBM_write_wlast),
        .HBM_write_wready(HBM_write_wready),
        .HBM_write_wstrb(HBM_write_wstrb),
        .HBM_write_wvalid(HBM_write_wvalid),
        .S00_AXI_0_araddr(S00_AXI_0_araddr),
        .S00_AXI_0_arprot(S00_AXI_0_arprot),
        .S00_AXI_0_arready(S00_AXI_0_arready),
        .S00_AXI_0_arvalid(S00_AXI_0_arvalid),
        .S00_AXI_0_awaddr(S00_AXI_0_awaddr),
        .S00_AXI_0_awprot(S00_AXI_0_awprot),
        .S00_AXI_0_awready(S00_AXI_0_awready),
        .S00_AXI_0_awvalid(S00_AXI_0_awvalid),
        .S00_AXI_0_bready(S00_AXI_0_bready),
        .S00_AXI_0_bresp(S00_AXI_0_bresp),
        .S00_AXI_0_bvalid(S00_AXI_0_bvalid),
        .S00_AXI_0_rdata(S00_AXI_0_rdata),
        .S00_AXI_0_rready(S00_AXI_0_rready),
        .S00_AXI_0_rresp(S00_AXI_0_rresp),
        .S00_AXI_0_rvalid(S00_AXI_0_rvalid),
        .S00_AXI_0_wdata(S00_AXI_0_wdata),
        .S00_AXI_0_wready(S00_AXI_0_wready),
        .S00_AXI_0_wstrb(S00_AXI_0_wstrb),
        .S00_AXI_0_wvalid(S00_AXI_0_wvalid),
        .ap_clk(ap_clk),
        .ref_clock(ref_clock),
        .ap_clk_320(ap_clk_320),
        .ap_clk_400(ap_clk_400),
        .ap_rst_n(ap_rst_n),
        .inputData_tdata(inputData_tdata),
        .inputData_tdest(inputData_tdest),
        .inputData_tkeep(inputData_tkeep),
        .inputData_tlast(inputData_tlast),
        .inputData_tready(inputData_tready),
        .inputData_tstrb(inputData_tstrb),
        .inputData_tvalid(inputData_tvalid),
        .outData_tdata(outData_tdata),
        .outData_tdest(outData_tdest),
        .outData_tkeep(outData_tkeep),
        .outData_tlast(outData_tlast),
        .outData_tready(outData_tready),
        .outData_tstrb(outData_tstrb),
        .outData_tvalid(outData_tvalid),
        .resetn_320(resetn_320),
        .resetn_400(resetn_400),
        .rx_tdata(rx_tdata),
        .rx_tkeep(rx_tkeep),
        .rx_tlast(rx_tlast),
        .rx_tready(rx_tready),
        .rx_tuser(rx_tuser),
        .rx_tvalid(rx_tvalid),
        .tx_tdata(tx_tdata),
        .tx_tkeep(tx_tkeep),
        .tx_tlast(tx_tlast),
        .tx_tready(tx_tready),
        .tx_tstrb(tx_tstrb),
        .tx_tvalid(tx_tvalid));
endmodule
