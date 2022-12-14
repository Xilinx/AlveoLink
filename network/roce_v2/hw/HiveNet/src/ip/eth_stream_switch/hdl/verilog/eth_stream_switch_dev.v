// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Version: 2020.2
// Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="eth_stream_switch_dev_eth_stream_switch_dev,hls_ip_2020_2,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcu200-fsgd2104-2-e,HLS_INPUT_CLOCK=3.300000,HLS_INPUT_ARCH=pipeline,HLS_SYN_CLOCK=0.656000,HLS_SYN_LAT=1,HLS_SYN_TPT=1,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=8,HLS_SYN_LUT=139,HLS_VERSION=2020_2}" *)

module eth_stream_switch_dev (
        ap_clk,
        ap_rst_n,
        tx_eth_i_TDATA,
        tx_eth_i_TVALID,
        tx_eth_i_TREADY,
        tx_eth_i_TKEEP,
        tx_eth_i_TSTRB,
        tx_eth_i_TLAST,
        tx_eth_i_TDEST,
        tx_rdma_i_TDATA,
        tx_rdma_i_TVALID,
        tx_rdma_i_TREADY,
        tx_rdma_i_TKEEP,
        tx_rdma_i_TSTRB,
        tx_rdma_i_TLAST,
        tx_rdma_i_TDEST,
        tx_o_TDATA,
        tx_o_TVALID,
        tx_o_TREADY,
        tx_o_TKEEP,
        tx_o_TSTRB,
        tx_o_TLAST,
        tx_o_TDEST
);

parameter    ap_ST_fsm_pp0_stage0 = 1'd1;

input   ap_clk;
input   ap_rst_n;
input  [511:0] tx_eth_i_TDATA;
input   tx_eth_i_TVALID;
output   tx_eth_i_TREADY;
input  [63:0] tx_eth_i_TKEEP;
input  [63:0] tx_eth_i_TSTRB;
input  [0:0] tx_eth_i_TLAST;
input  [0:0] tx_eth_i_TDEST;
input  [511:0] tx_rdma_i_TDATA;
input   tx_rdma_i_TVALID;
output   tx_rdma_i_TREADY;
input  [63:0] tx_rdma_i_TKEEP;
input  [63:0] tx_rdma_i_TSTRB;
input  [0:0] tx_rdma_i_TLAST;
input  [0:0] tx_rdma_i_TDEST;
output  [511:0] tx_o_TDATA;
output   tx_o_TVALID;
input   tx_o_TREADY;
output  [63:0] tx_o_TKEEP;
output  [63:0] tx_o_TSTRB;
output  [0:0] tx_o_TLAST;
output  [0:0] tx_o_TDEST;

 reg    ap_rst_n_inv;
reg   [1:0] state;
reg    tx_eth_i_TDATA_blk_n;
(* fsm_encoding = "none" *) reg   [0:0] ap_CS_fsm;
wire    ap_CS_fsm_pp0_stage0;
wire    ap_block_pp0_stage0;
wire   [1:0] state_load_load_fu_161_p1;
wire   [0:0] grp_nbreadreq_fu_125_p7;
reg    tx_rdma_i_TDATA_blk_n;
wire   [0:0] grp_nbreadreq_fu_76_p7;
reg    tx_o_TDATA_blk_n;
reg    ap_enable_reg_pp0_iter1;
reg   [1:0] state_load_reg_227;
reg   [0:0] tmp_1_reg_260;
reg   [0:0] tmp_2_reg_231;
reg    ap_predicate_op30_read_state1;
reg    ap_predicate_op36_write_state1;
reg    ap_predicate_op42_read_state1;
reg    ap_predicate_op48_write_state1;
reg    ap_block_state1_pp0_stage0_iter0;
reg    ap_block_state1_io;
reg    ap_predicate_op60_write_state2;
reg    ap_predicate_op61_write_state2;
wire    regslice_both_tx_o_V_data_V_U_apdone_blk;
reg    ap_block_state2_pp0_stage0_iter1;
reg    ap_block_state2_io;
reg    ap_block_pp0_stage0_11001;
wire   [0:0] tmp_i_last_fu_180_p1;
wire   [0:0] tmp_i27_last_fu_205_p1;
reg    ap_block_pp0_stage0_subdone;
reg    ap_block_pp0_stage0_01001;
reg   [0:0] ap_NS_fsm;
wire    ap_reset_idle_pp0;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
wire    regslice_both_tx_eth_i_V_data_V_U_apdone_blk;
wire   [511:0] tx_eth_i_TDATA_int_regslice;
wire    tx_eth_i_TVALID_int_regslice;
reg    tx_eth_i_TREADY_int_regslice;
wire    regslice_both_tx_eth_i_V_data_V_U_ack_in;
wire    regslice_both_tx_eth_i_V_keep_V_U_apdone_blk;
wire   [63:0] tx_eth_i_TKEEP_int_regslice;
wire    regslice_both_tx_eth_i_V_keep_V_U_vld_out;
wire    regslice_both_tx_eth_i_V_keep_V_U_ack_in;
wire    regslice_both_tx_eth_i_V_strb_V_U_apdone_blk;
wire   [63:0] tx_eth_i_TSTRB_int_regslice;
wire    regslice_both_tx_eth_i_V_strb_V_U_vld_out;
wire    regslice_both_tx_eth_i_V_strb_V_U_ack_in;
wire    regslice_both_tx_eth_i_V_last_V_U_apdone_blk;
wire   [0:0] tx_eth_i_TLAST_int_regslice;
wire    regslice_both_tx_eth_i_V_last_V_U_vld_out;
wire    regslice_both_tx_eth_i_V_last_V_U_ack_in;
wire    regslice_both_tx_eth_i_V_dest_V_U_apdone_blk;
wire   [0:0] tx_eth_i_TDEST_int_regslice;
wire    regslice_both_tx_eth_i_V_dest_V_U_vld_out;
wire    regslice_both_tx_eth_i_V_dest_V_U_ack_in;
wire    regslice_both_tx_rdma_i_V_data_V_U_apdone_blk;
wire   [511:0] tx_rdma_i_TDATA_int_regslice;
wire    tx_rdma_i_TVALID_int_regslice;
reg    tx_rdma_i_TREADY_int_regslice;
wire    regslice_both_tx_rdma_i_V_data_V_U_ack_in;
wire    regslice_both_tx_rdma_i_V_keep_V_U_apdone_blk;
wire   [63:0] tx_rdma_i_TKEEP_int_regslice;
wire    regslice_both_tx_rdma_i_V_keep_V_U_vld_out;
wire    regslice_both_tx_rdma_i_V_keep_V_U_ack_in;
wire    regslice_both_tx_rdma_i_V_strb_V_U_apdone_blk;
wire   [63:0] tx_rdma_i_TSTRB_int_regslice;
wire    regslice_both_tx_rdma_i_V_strb_V_U_vld_out;
wire    regslice_both_tx_rdma_i_V_strb_V_U_ack_in;
wire    regslice_both_tx_rdma_i_V_last_V_U_apdone_blk;
wire   [0:0] tx_rdma_i_TLAST_int_regslice;
wire    regslice_both_tx_rdma_i_V_last_V_U_vld_out;
wire    regslice_both_tx_rdma_i_V_last_V_U_ack_in;
wire    regslice_both_tx_rdma_i_V_dest_V_U_apdone_blk;
wire   [0:0] tx_rdma_i_TDEST_int_regslice;
wire    regslice_both_tx_rdma_i_V_dest_V_U_vld_out;
wire    regslice_both_tx_rdma_i_V_dest_V_U_ack_in;
reg   [511:0] tx_o_TDATA_int_regslice;
reg    tx_o_TVALID_int_regslice;
wire    tx_o_TREADY_int_regslice;
wire    regslice_both_tx_o_V_data_V_U_vld_out;
wire    regslice_both_tx_o_V_keep_V_U_apdone_blk;
reg   [63:0] tx_o_TKEEP_int_regslice;
wire    regslice_both_tx_o_V_keep_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_keep_V_U_vld_out;
wire    regslice_both_tx_o_V_strb_V_U_apdone_blk;
reg   [63:0] tx_o_TSTRB_int_regslice;
wire    regslice_both_tx_o_V_strb_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_strb_V_U_vld_out;
wire    regslice_both_tx_o_V_last_V_U_apdone_blk;
reg   [0:0] tx_o_TLAST_int_regslice;
wire    regslice_both_tx_o_V_last_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_last_V_U_vld_out;
wire    regslice_both_tx_o_V_dest_V_U_apdone_blk;
reg   [0:0] tx_o_TDEST_int_regslice;
wire    regslice_both_tx_o_V_dest_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_dest_V_U_vld_out;
reg    ap_condition_185;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 state = 2'd0;
#0 ap_CS_fsm = 1'd1;
#0 ap_enable_reg_pp0_iter1 = 1'b0;
end

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 512 ))
regslice_both_tx_eth_i_V_data_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_eth_i_TDATA),
    .vld_in(tx_eth_i_TVALID),
    .ack_in(regslice_both_tx_eth_i_V_data_V_U_ack_in),
    .data_out(tx_eth_i_TDATA_int_regslice),
    .vld_out(tx_eth_i_TVALID_int_regslice),
    .ack_out(tx_eth_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_eth_i_V_data_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_eth_i_V_keep_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_eth_i_TKEEP),
    .vld_in(tx_eth_i_TVALID),
    .ack_in(regslice_both_tx_eth_i_V_keep_V_U_ack_in),
    .data_out(tx_eth_i_TKEEP_int_regslice),
    .vld_out(regslice_both_tx_eth_i_V_keep_V_U_vld_out),
    .ack_out(tx_eth_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_eth_i_V_keep_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_eth_i_V_strb_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_eth_i_TSTRB),
    .vld_in(tx_eth_i_TVALID),
    .ack_in(regslice_both_tx_eth_i_V_strb_V_U_ack_in),
    .data_out(tx_eth_i_TSTRB_int_regslice),
    .vld_out(regslice_both_tx_eth_i_V_strb_V_U_vld_out),
    .ack_out(tx_eth_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_eth_i_V_strb_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_eth_i_V_last_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_eth_i_TLAST),
    .vld_in(tx_eth_i_TVALID),
    .ack_in(regslice_both_tx_eth_i_V_last_V_U_ack_in),
    .data_out(tx_eth_i_TLAST_int_regslice),
    .vld_out(regslice_both_tx_eth_i_V_last_V_U_vld_out),
    .ack_out(tx_eth_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_eth_i_V_last_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_eth_i_V_dest_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_eth_i_TDEST),
    .vld_in(tx_eth_i_TVALID),
    .ack_in(regslice_both_tx_eth_i_V_dest_V_U_ack_in),
    .data_out(tx_eth_i_TDEST_int_regslice),
    .vld_out(regslice_both_tx_eth_i_V_dest_V_U_vld_out),
    .ack_out(tx_eth_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_eth_i_V_dest_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 512 ))
regslice_both_tx_rdma_i_V_data_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_rdma_i_TDATA),
    .vld_in(tx_rdma_i_TVALID),
    .ack_in(regslice_both_tx_rdma_i_V_data_V_U_ack_in),
    .data_out(tx_rdma_i_TDATA_int_regslice),
    .vld_out(tx_rdma_i_TVALID_int_regslice),
    .ack_out(tx_rdma_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_rdma_i_V_data_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_rdma_i_V_keep_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_rdma_i_TKEEP),
    .vld_in(tx_rdma_i_TVALID),
    .ack_in(regslice_both_tx_rdma_i_V_keep_V_U_ack_in),
    .data_out(tx_rdma_i_TKEEP_int_regslice),
    .vld_out(regslice_both_tx_rdma_i_V_keep_V_U_vld_out),
    .ack_out(tx_rdma_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_rdma_i_V_keep_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_rdma_i_V_strb_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_rdma_i_TSTRB),
    .vld_in(tx_rdma_i_TVALID),
    .ack_in(regslice_both_tx_rdma_i_V_strb_V_U_ack_in),
    .data_out(tx_rdma_i_TSTRB_int_regslice),
    .vld_out(regslice_both_tx_rdma_i_V_strb_V_U_vld_out),
    .ack_out(tx_rdma_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_rdma_i_V_strb_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_rdma_i_V_last_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_rdma_i_TLAST),
    .vld_in(tx_rdma_i_TVALID),
    .ack_in(regslice_both_tx_rdma_i_V_last_V_U_ack_in),
    .data_out(tx_rdma_i_TLAST_int_regslice),
    .vld_out(regslice_both_tx_rdma_i_V_last_V_U_vld_out),
    .ack_out(tx_rdma_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_rdma_i_V_last_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_rdma_i_V_dest_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_rdma_i_TDEST),
    .vld_in(tx_rdma_i_TVALID),
    .ack_in(regslice_both_tx_rdma_i_V_dest_V_U_ack_in),
    .data_out(tx_rdma_i_TDEST_int_regslice),
    .vld_out(regslice_both_tx_rdma_i_V_dest_V_U_vld_out),
    .ack_out(tx_rdma_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_rdma_i_V_dest_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 512 ))
regslice_both_tx_o_V_data_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_o_TDATA_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(tx_o_TREADY_int_regslice),
    .data_out(tx_o_TDATA),
    .vld_out(regslice_both_tx_o_V_data_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_data_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_o_V_keep_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_o_TKEEP_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_keep_V_U_ack_in_dummy),
    .data_out(tx_o_TKEEP),
    .vld_out(regslice_both_tx_o_V_keep_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_keep_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_o_V_strb_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_o_TSTRB_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_strb_V_U_ack_in_dummy),
    .data_out(tx_o_TSTRB),
    .vld_out(regslice_both_tx_o_V_strb_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_strb_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_o_V_last_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_o_TLAST_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_last_V_U_ack_in_dummy),
    .data_out(tx_o_TLAST),
    .vld_out(regslice_both_tx_o_V_last_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_last_V_U_apdone_blk)
);

eth_stream_switch_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_o_V_dest_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_o_TDEST_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_dest_V_U_ack_in_dummy),
    .data_out(tx_o_TDEST),
    .vld_out(regslice_both_tx_o_V_dest_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_dest_V_U_apdone_blk)
);

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_pp0_stage0;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        ap_enable_reg_pp0_iter1 <= 1'b0;
    end else begin
        if ((1'b0 == ap_block_pp0_stage0_subdone)) begin
            ap_enable_reg_pp0_iter1 <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        state <= 2'd0;
    end else begin
        if (((grp_nbreadreq_fu_125_p7 == 1'd1) & (state_load_load_fu_161_p1 == 2'd0) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
            state <= 2'd1;
        end else if (((grp_nbreadreq_fu_76_p7 == 1'd1) & (grp_nbreadreq_fu_125_p7 == 1'd0) & (state_load_load_fu_161_p1 == 2'd0) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
            state <= 2'd2;
        end else if ((((grp_nbreadreq_fu_76_p7 == 1'd1) & (state == 2'd2) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (tmp_i_last_fu_180_p1 == 1'd1)) | ((grp_nbreadreq_fu_125_p7 == 1'd1) & (state == 2'd1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (tmp_i27_last_fu_205_p1 == 1'd1)))) begin
            state <= 2'd0;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        state_load_reg_227 <= state;
    end
end

always @ (posedge ap_clk) begin
    if (((state == 2'd1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tmp_1_reg_260 <= grp_nbreadreq_fu_125_p7;
    end
end

always @ (posedge ap_clk) begin
    if (((state == 2'd2) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tmp_2_reg_231 <= grp_nbreadreq_fu_76_p7;
    end
end

always @ (*) begin
    if (((ap_enable_reg_pp0_iter1 == 1'b0) & (1'b1 == 1'b0))) begin
        ap_idle_pp0 = 1'b1;
    end else begin
        ap_idle_pp0 = 1'b0;
    end
end

assign ap_reset_idle_pp0 = 1'b0;

always @ (*) begin
    if (((grp_nbreadreq_fu_125_p7 == 1'd1) & (state == 2'd1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tx_eth_i_TDATA_blk_n = tx_eth_i_TVALID_int_regslice;
    end else begin
        tx_eth_i_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((ap_predicate_op42_read_state1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tx_eth_i_TREADY_int_regslice = 1'b1;
    end else begin
        tx_eth_i_TREADY_int_regslice = 1'b0;
    end
end

always @ (*) begin
    if ((((tmp_2_reg_231 == 1'd1) & (state_load_reg_227 == 2'd2) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((tmp_1_reg_260 == 1'd1) & (state_load_reg_227 == 2'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((grp_nbreadreq_fu_76_p7 == 1'd1) & (state == 2'd2) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((grp_nbreadreq_fu_125_p7 == 1'd1) & (state == 2'd1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)))) begin
        tx_o_TDATA_blk_n = tx_o_TREADY_int_regslice;
    end else begin
        tx_o_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((1'b1 == ap_condition_185)) begin
        if ((ap_predicate_op48_write_state1 == 1'b1)) begin
            tx_o_TDATA_int_regslice = tx_eth_i_TDATA_int_regslice;
        end else if ((ap_predicate_op36_write_state1 == 1'b1)) begin
            tx_o_TDATA_int_regslice = tx_rdma_i_TDATA_int_regslice;
        end else begin
            tx_o_TDATA_int_regslice = 'bx;
        end
    end else begin
        tx_o_TDATA_int_regslice = 'bx;
    end
end

always @ (*) begin
    if ((1'b1 == ap_condition_185)) begin
        if ((ap_predicate_op48_write_state1 == 1'b1)) begin
            tx_o_TDEST_int_regslice = tx_eth_i_TDEST_int_regslice;
        end else if ((ap_predicate_op36_write_state1 == 1'b1)) begin
            tx_o_TDEST_int_regslice = tx_rdma_i_TDEST_int_regslice;
        end else begin
            tx_o_TDEST_int_regslice = 'bx;
        end
    end else begin
        tx_o_TDEST_int_regslice = 'bx;
    end
end

always @ (*) begin
    if ((1'b1 == ap_condition_185)) begin
        if ((ap_predicate_op48_write_state1 == 1'b1)) begin
            tx_o_TKEEP_int_regslice = tx_eth_i_TKEEP_int_regslice;
        end else if ((ap_predicate_op36_write_state1 == 1'b1)) begin
            tx_o_TKEEP_int_regslice = tx_rdma_i_TKEEP_int_regslice;
        end else begin
            tx_o_TKEEP_int_regslice = 'bx;
        end
    end else begin
        tx_o_TKEEP_int_regslice = 'bx;
    end
end

always @ (*) begin
    if ((1'b1 == ap_condition_185)) begin
        if ((ap_predicate_op48_write_state1 == 1'b1)) begin
            tx_o_TLAST_int_regslice = tx_eth_i_TLAST_int_regslice;
        end else if ((ap_predicate_op36_write_state1 == 1'b1)) begin
            tx_o_TLAST_int_regslice = tx_rdma_i_TLAST_int_regslice;
        end else begin
            tx_o_TLAST_int_regslice = 'bx;
        end
    end else begin
        tx_o_TLAST_int_regslice = 'bx;
    end
end

always @ (*) begin
    if ((1'b1 == ap_condition_185)) begin
        if ((ap_predicate_op48_write_state1 == 1'b1)) begin
            tx_o_TSTRB_int_regslice = tx_eth_i_TSTRB_int_regslice;
        end else if ((ap_predicate_op36_write_state1 == 1'b1)) begin
            tx_o_TSTRB_int_regslice = tx_rdma_i_TSTRB_int_regslice;
        end else begin
            tx_o_TSTRB_int_regslice = 'bx;
        end
    end else begin
        tx_o_TSTRB_int_regslice = 'bx;
    end
end

always @ (*) begin
    if ((((ap_predicate_op48_write_state1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((ap_predicate_op36_write_state1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)))) begin
        tx_o_TVALID_int_regslice = 1'b1;
    end else begin
        tx_o_TVALID_int_regslice = 1'b0;
    end
end

always @ (*) begin
    if (((grp_nbreadreq_fu_76_p7 == 1'd1) & (state == 2'd2) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tx_rdma_i_TDATA_blk_n = tx_rdma_i_TVALID_int_regslice;
    end else begin
        tx_rdma_i_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if (((ap_predicate_op30_read_state1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tx_rdma_i_TREADY_int_regslice = 1'b1;
    end else begin
        tx_rdma_i_TREADY_int_regslice = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_pp0_stage0 : begin
            ap_NS_fsm = ap_ST_fsm_pp0_stage0;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign ap_CS_fsm_pp0_stage0 = ap_CS_fsm[32'd0];

assign ap_block_pp0_stage0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_pp0_stage0_01001 = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op61_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op60_write_state2 == 1'b1)))) | ((1'b1 == 1'b1) & (((ap_predicate_op48_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op42_read_state1 == 1'b1) & (tx_eth_i_TVALID_int_regslice == 1'b0)) | ((ap_predicate_op36_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op30_read_state1 == 1'b1) & (tx_rdma_i_TVALID_int_regslice == 1'b0)))));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((1'b1 == ap_block_state2_io) | (regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op61_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op60_write_state2 == 1'b1)))) | ((1'b1 == 1'b1) & ((1'b1 == ap_block_state1_io) | ((ap_predicate_op48_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op42_read_state1 == 1'b1) & (tx_eth_i_TVALID_int_regslice == 1'b0)) | ((ap_predicate_op36_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op30_read_state1 == 1'b1) & (tx_rdma_i_TVALID_int_regslice == 1'b0)))));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((1'b1 == ap_block_state2_io) | (regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op61_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op60_write_state2 == 1'b1)))) | ((1'b1 == 1'b1) & ((1'b1 == ap_block_state1_io) | ((ap_predicate_op48_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op42_read_state1 == 1'b1) & (tx_eth_i_TVALID_int_regslice == 1'b0)) | ((ap_predicate_op36_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op30_read_state1 == 1'b1) & (tx_rdma_i_TVALID_int_regslice == 1'b0)))));
end

always @ (*) begin
    ap_block_state1_io = (((ap_predicate_op48_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op36_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)));
end

always @ (*) begin
    ap_block_state1_pp0_stage0_iter0 = (((ap_predicate_op48_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op42_read_state1 == 1'b1) & (tx_eth_i_TVALID_int_regslice == 1'b0)) | ((ap_predicate_op36_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op30_read_state1 == 1'b1) & (tx_rdma_i_TVALID_int_regslice == 1'b0)));
end

always @ (*) begin
    ap_block_state2_io = (((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op61_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op60_write_state2 == 1'b1)));
end

always @ (*) begin
    ap_block_state2_pp0_stage0_iter1 = ((regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op61_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op60_write_state2 == 1'b1)));
end

always @ (*) begin
    ap_condition_185 = ((1'b0 == ap_block_pp0_stage0_01001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0));
end

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

always @ (*) begin
    ap_predicate_op30_read_state1 = ((grp_nbreadreq_fu_76_p7 == 1'd1) & (state == 2'd2));
end

always @ (*) begin
    ap_predicate_op36_write_state1 = ((grp_nbreadreq_fu_76_p7 == 1'd1) & (state == 2'd2));
end

always @ (*) begin
    ap_predicate_op42_read_state1 = ((grp_nbreadreq_fu_125_p7 == 1'd1) & (state == 2'd1));
end

always @ (*) begin
    ap_predicate_op48_write_state1 = ((grp_nbreadreq_fu_125_p7 == 1'd1) & (state == 2'd1));
end

always @ (*) begin
    ap_predicate_op60_write_state2 = ((tmp_2_reg_231 == 1'd1) & (state_load_reg_227 == 2'd2));
end

always @ (*) begin
    ap_predicate_op61_write_state2 = ((tmp_1_reg_260 == 1'd1) & (state_load_reg_227 == 2'd1));
end

always @ (*) begin
    ap_rst_n_inv = ~ap_rst_n;
end

assign grp_nbreadreq_fu_125_p7 = tx_eth_i_TVALID_int_regslice;

assign grp_nbreadreq_fu_76_p7 = tx_rdma_i_TVALID_int_regslice;

assign state_load_load_fu_161_p1 = state;

assign tmp_i27_last_fu_205_p1 = tx_eth_i_TLAST_int_regslice;

assign tmp_i_last_fu_180_p1 = tx_rdma_i_TLAST_int_regslice;

assign tx_eth_i_TREADY = regslice_both_tx_eth_i_V_data_V_U_ack_in;

assign tx_o_TVALID = regslice_both_tx_o_V_data_V_U_vld_out;

assign tx_rdma_i_TREADY = regslice_both_tx_rdma_i_V_data_V_U_ack_in;

endmodule //eth_stream_switch_dev
