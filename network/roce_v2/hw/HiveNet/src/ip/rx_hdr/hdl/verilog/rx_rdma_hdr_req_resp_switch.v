// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
// Version: 2020.2
// Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

module rx_rdma_hdr_req_resp_switch (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
        ap_continue,
        ap_idle,
        ap_ready,
        rx_headers_internal_dout,
        rx_headers_internal_empty_n,
        rx_headers_internal_read,
        rx_payload_dout,
        rx_payload_empty_n,
        rx_payload_read,
        rx_req_o_V_TREADY,
        tx_resp_o_V_TREADY,
        rx_req_payload_o_TREADY,
        tx_resp_payload_o_TREADY,
        rx_req_o_V_TDATA,
        rx_req_o_V_TVALID,
        tx_resp_o_V_TDATA,
        tx_resp_o_V_TVALID,
        state_req_resp_switch,
        state_req_resp_switch_ap_vld,
        rx_req_payload_o_TDATA,
        rx_req_payload_o_TVALID,
        rx_req_payload_o_TKEEP,
        rx_req_payload_o_TSTRB,
        rx_req_payload_o_TLAST,
        rx_req_payload_o_TDEST,
        tx_resp_payload_o_TDATA,
        tx_resp_payload_o_TVALID,
        tx_resp_payload_o_TKEEP,
        tx_resp_payload_o_TSTRB,
        tx_resp_payload_o_TLAST,
        tx_resp_payload_o_TDEST
);

parameter    ap_ST_iter0_fsm_state1 = 1'd1;
parameter    ap_ST_iter1_fsm_state2 = 2'd2;
parameter    ap_ST_iter2_fsm_state3 = 2'd2;
parameter    ap_ST_iter1_fsm_state0 = 2'd1;
parameter    ap_ST_iter2_fsm_state0 = 2'd1;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
input   ap_continue;
output   ap_idle;
output   ap_ready;
input  [415:0] rx_headers_internal_dout;
input   rx_headers_internal_empty_n;
output   rx_headers_internal_read;
input  [1023:0] rx_payload_dout;
input   rx_payload_empty_n;
output   rx_payload_read;
input   rx_req_o_V_TREADY;
input   tx_resp_o_V_TREADY;
input   rx_req_payload_o_TREADY;
input   tx_resp_payload_o_TREADY;
output  [351:0] rx_req_o_V_TDATA;
output   rx_req_o_V_TVALID;
output  [255:0] tx_resp_o_V_TDATA;
output   tx_resp_o_V_TVALID;
output  [31:0] state_req_resp_switch;
output   state_req_resp_switch_ap_vld;
output  [511:0] rx_req_payload_o_TDATA;
output   rx_req_payload_o_TVALID;
output  [63:0] rx_req_payload_o_TKEEP;
output  [63:0] rx_req_payload_o_TSTRB;
output  [0:0] rx_req_payload_o_TLAST;
output  [0:0] rx_req_payload_o_TDEST;
output  [511:0] tx_resp_payload_o_TDATA;
output   tx_resp_payload_o_TVALID;
output  [63:0] tx_resp_payload_o_TKEEP;
output  [63:0] tx_resp_payload_o_TSTRB;
output  [0:0] tx_resp_payload_o_TLAST;
output  [0:0] tx_resp_payload_o_TDEST;

reg ap_done;
reg ap_idle;
reg ap_ready;
reg rx_headers_internal_read;
reg rx_payload_read;
reg rx_req_o_V_TVALID;
reg tx_resp_o_V_TVALID;
reg[31:0] state_req_resp_switch;
reg state_req_resp_switch_ap_vld;
reg rx_req_payload_o_TVALID;
reg tx_resp_payload_o_TVALID;

reg    ap_done_reg;
reg   [0:0] ap_CS_iter0_fsm;
wire    ap_CS_iter0_fsm_state1;
reg   [1:0] ap_CS_iter1_fsm;
wire    ap_CS_iter1_fsm_state0;
reg   [1:0] ap_CS_iter2_fsm;
wire    ap_CS_iter2_fsm_state0;
wire   [0:0] tmp_1_i_nbreadreq_fu_118_p3;
reg    ap_predicate_op8_read_state1;
wire   [0:0] tmp_i_nbreadreq_fu_132_p3;
reg    ap_predicate_op24_read_state1;
reg    ap_block_state1_pp0_stage0_iter0;
wire    ap_block_state2_pp0_stage0_iter1;
wire    ap_CS_iter1_fsm_state2;
reg   [0:0] state_load_reg_372;
reg   [0:0] state_load_reg_372_pp0_iter1_reg;
reg   [0:0] tmp_1_i_reg_377;
reg   [0:0] tmp_1_i_reg_377_pp0_iter1_reg;
reg   [0:0] icmp_ln11_reg_402;
reg   [0:0] icmp_ln11_reg_402_pp0_iter1_reg;
reg    ap_predicate_op56_write_state3;
reg    ap_predicate_op60_write_state3;
reg   [0:0] tmp_i_reg_409;
reg   [0:0] tmp_i_reg_409_pp0_iter1_reg;
reg   [0:0] drop_payload_load_reg_431;
reg   [0:0] payload_dir_load_reg_435;
reg    ap_predicate_op62_write_state3;
reg    ap_predicate_op64_write_state3;
reg    ap_block_state3_pp0_stage0_iter2;
reg    ap_block_state3_io;
wire    ap_CS_iter2_fsm_state3;
reg   [0:0] state;
reg   [0:0] payload_dir;
reg   [0:0] drop_payload;
reg    rx_req_o_V_TDATA_blk_n;
reg    tx_resp_o_V_TDATA_blk_n;
reg    rx_req_payload_o_TDATA_blk_n;
reg    tx_resp_payload_o_TDATA_blk_n;
reg    rx_headers_internal_blk_n;
reg    rx_payload_blk_n;
wire   [0:0] state_load_reg_372_pp0_iter0_reg;
wire   [0:0] tmp_1_i_reg_377_pp0_iter0_reg;
reg   [415:0] rx_headers_internal_read_reg_381;
reg   [415:0] rx_headers_internal_read_reg_381_pp0_iter1_reg;
wire   [159:0] trunc_ln145_1_fu_213_p1;
reg   [159:0] trunc_ln145_1_reg_387;
reg   [159:0] trunc_ln145_1_reg_387_pp0_iter1_reg;
reg   [31:0] trunc_ln145_s_reg_392;
reg   [31:0] trunc_ln145_s_reg_392_pp0_iter1_reg;
wire   [0:0] icmp_ln870_fu_251_p2;
reg   [0:0] icmp_ln870_reg_397;
wire   [0:0] icmp_ln11_fu_263_p2;
wire   [0:0] icmp_ln11_reg_402_pp0_iter0_reg;
wire   [0:0] tmp_i_reg_409_pp0_iter0_reg;
wire   [511:0] trunc_ln145_fu_281_p1;
reg   [511:0] trunc_ln145_reg_413;
reg   [511:0] trunc_ln145_reg_413_pp0_iter1_reg;
reg   [63:0] tmp_i4_keep_i_reg_419;
reg   [63:0] tmp_i4_keep_i_reg_419_pp0_iter1_reg;
wire   [0:0] tmp_last_V_fu_295_p3;
reg   [0:0] tmp_last_V_reg_425;
reg   [0:0] tmp_last_V_reg_425_pp0_iter1_reg;
wire   [0:0] drop_payload_load_load_fu_332_p1;
wire   [0:0] icmp_ln424_fu_269_p2;
wire   [31:0] state_load_cast_i_fu_340_p1;
reg   [31:0] state_req_resp_switch_preg;
wire   [7:0] hdr_bth_opcode_V_fu_217_p4;
wire   [8:0] zext_ln870_fu_247_p1;
wire   [8:0] add_ln11_fu_257_p2;
wire   [15:0] tmp_payload_length_fu_237_p4;
wire   [191:0] tmp_1_fu_344_p4;
wire   [223:0] trunc_ln174_fu_361_p1;
reg   [0:0] ap_NS_iter0_fsm;
reg   [1:0] ap_NS_iter1_fsm;
reg   [1:0] ap_NS_iter2_fsm;
reg    ap_condition_379;
reg    ap_condition_244;
reg    ap_condition_247;
reg    ap_condition_141;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 ap_done_reg = 1'b0;
#0 ap_CS_iter0_fsm = 1'd1;
#0 ap_CS_iter1_fsm = 2'd1;
#0 ap_CS_iter2_fsm = 2'd1;
#0 state = 1'd0;
#0 payload_dir = 1'd0;
#0 drop_payload = 1'd0;
#0 state_req_resp_switch_preg = 32'd0;
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_iter0_fsm <= ap_ST_iter0_fsm_state1;
    end else begin
        ap_CS_iter0_fsm <= ap_NS_iter0_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_iter1_fsm <= ap_ST_iter1_fsm_state0;
    end else begin
        ap_CS_iter1_fsm <= ap_NS_iter1_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_iter2_fsm <= ap_ST_iter2_fsm_state0;
    end else begin
        ap_CS_iter2_fsm <= ap_NS_iter2_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_done_reg <= 1'b0;
    end else begin
        if ((ap_continue == 1'b1)) begin
            ap_done_reg <= 1'b0;
        end else if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
            ap_done_reg <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
                state_req_resp_switch_preg[0] <= 1'b0;
    end else begin
        if ((~((ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
                        state_req_resp_switch_preg[0] <= state_load_cast_i_fu_340_p1[0];
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_379)) begin
        if ((icmp_ln11_reg_402_pp0_iter0_reg == 1'd1)) begin
            drop_payload <= icmp_ln870_reg_397;
        end else if ((icmp_ln11_reg_402_pp0_iter0_reg == 1'd0)) begin
            drop_payload <= 1'd0;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_379)) begin
        if ((icmp_ln11_reg_402_pp0_iter0_reg == 1'd1)) begin
            payload_dir <= 1'd1;
        end else if ((icmp_ln11_reg_402_pp0_iter0_reg == 1'd0)) begin
            payload_dir <= 1'd0;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_141)) begin
        if ((1'b1 == ap_condition_247)) begin
            state <= 1'd0;
        end else if ((1'b1 == ap_condition_244)) begin
            state <= 1'd1;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter1_fsm_state2) & (tmp_i_reg_409_pp0_iter0_reg == 1'd1) & (state_load_reg_372_pp0_iter0_reg == 1'd1))) begin
        drop_payload_load_reg_431 <= drop_payload;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (tmp_1_i_nbreadreq_fu_118_p3 == 1'd1) & (state == 1'd0) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        icmp_ln11_reg_402 <= icmp_ln11_fu_263_p2;
        icmp_ln870_reg_397 <= icmp_ln870_fu_251_p2;
        trunc_ln145_1_reg_387 <= trunc_ln145_1_fu_213_p1;
        trunc_ln145_s_reg_392 <= {{rx_headers_internal_dout[415:384]}};
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter1_fsm_state2))) begin
        icmp_ln11_reg_402_pp0_iter1_reg <= icmp_ln11_reg_402;
        rx_headers_internal_read_reg_381_pp0_iter1_reg <= rx_headers_internal_read_reg_381;
        state_load_reg_372_pp0_iter1_reg <= state_load_reg_372;
        tmp_1_i_reg_377_pp0_iter1_reg <= tmp_1_i_reg_377;
        tmp_i4_keep_i_reg_419_pp0_iter1_reg <= tmp_i4_keep_i_reg_419;
        tmp_i_reg_409_pp0_iter1_reg <= tmp_i_reg_409;
        tmp_last_V_reg_425_pp0_iter1_reg <= tmp_last_V_reg_425;
        trunc_ln145_1_reg_387_pp0_iter1_reg <= trunc_ln145_1_reg_387;
        trunc_ln145_reg_413_pp0_iter1_reg <= trunc_ln145_reg_413;
        trunc_ln145_s_reg_392_pp0_iter1_reg <= trunc_ln145_s_reg_392;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter1_fsm_state2) & (drop_payload_load_load_fu_332_p1 == 1'd0) & (tmp_i_reg_409_pp0_iter0_reg == 1'd1) & (state_load_reg_372_pp0_iter0_reg == 1'd1))) begin
        payload_dir_load_reg_435 <= payload_dir;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (ap_predicate_op8_read_state1 == 1'b1) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        rx_headers_internal_read_reg_381 <= rx_headers_internal_dout;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        state_load_reg_372 <= state;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (state == 1'd0) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        tmp_1_i_reg_377 <= tmp_1_i_nbreadreq_fu_118_p3;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (tmp_i_nbreadreq_fu_132_p3 == 1'd1) & (state == 1'd1) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        tmp_i4_keep_i_reg_419 <= {{rx_payload_dout[575:512]}};
        tmp_last_V_reg_425 <= rx_payload_dout[32'd576];
        trunc_ln145_reg_413 <= trunc_ln145_fu_281_p1;
    end
end

always @ (posedge ap_clk) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (state == 1'd1) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        tmp_i_reg_409 <= tmp_i_nbreadreq_fu_132_p3;
    end
end

always @ (*) begin
    if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        ap_done = 1'b1;
    end else begin
        ap_done = ap_done_reg;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_iter2_fsm_state0) & (1'b1 == ap_CS_iter1_fsm_state0) & (1'b1 == ap_CS_iter0_fsm_state1) & (ap_start == 1'b0))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ (*) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        ap_ready = 1'b1;
    end else begin
        ap_ready = 1'b0;
    end
end

always @ (*) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0)) & (ap_predicate_op8_read_state1 == 1'b1) & (1'b1 == ap_CS_iter0_fsm_state1) & (ap_start == 1'b1))) begin
        rx_headers_internal_blk_n = rx_headers_internal_empty_n;
    end else begin
        rx_headers_internal_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (ap_predicate_op8_read_state1 == 1'b1) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        rx_headers_internal_read = 1'b1;
    end else begin
        rx_headers_internal_read = 1'b0;
    end
end

always @ (*) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0)) & (ap_predicate_op24_read_state1 == 1'b1) & (1'b1 == ap_CS_iter0_fsm_state1) & (ap_start == 1'b1))) begin
        rx_payload_blk_n = rx_payload_empty_n;
    end else begin
        rx_payload_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (ap_predicate_op24_read_state1 == 1'b1) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
        rx_payload_read = 1'b1;
    end else begin
        rx_payload_read = 1'b0;
    end
end

always @ (*) begin
    if (((ap_predicate_op56_write_state3 == 1'b1) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        rx_req_o_V_TDATA_blk_n = rx_req_o_V_TREADY;
    end else begin
        rx_req_o_V_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (ap_predicate_op56_write_state3 == 1'b1) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        rx_req_o_V_TVALID = 1'b1;
    end else begin
        rx_req_o_V_TVALID = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_iter2_fsm_state3) & (ap_predicate_op62_write_state3 == 1'b1))) begin
        rx_req_payload_o_TDATA_blk_n = rx_req_payload_o_TREADY;
    end else begin
        rx_req_payload_o_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3) & (ap_predicate_op62_write_state3 == 1'b1))) begin
        rx_req_payload_o_TVALID = 1'b1;
    end else begin
        rx_req_payload_o_TVALID = 1'b0;
    end
end

always @ (*) begin
    if ((~((ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        state_req_resp_switch = state_load_cast_i_fu_340_p1;
    end else begin
        state_req_resp_switch = state_req_resp_switch_preg;
    end
end

always @ (*) begin
    if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        state_req_resp_switch_ap_vld = 1'b1;
    end else begin
        state_req_resp_switch_ap_vld = 1'b0;
    end
end

always @ (*) begin
    if (((ap_predicate_op60_write_state3 == 1'b1) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        tx_resp_o_V_TDATA_blk_n = tx_resp_o_V_TREADY;
    end else begin
        tx_resp_o_V_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (ap_predicate_op60_write_state3 == 1'b1) & (1'b1 == ap_CS_iter2_fsm_state3))) begin
        tx_resp_o_V_TVALID = 1'b1;
    end else begin
        tx_resp_o_V_TVALID = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_iter2_fsm_state3) & (ap_predicate_op64_write_state3 == 1'b1))) begin
        tx_resp_payload_o_TDATA_blk_n = tx_resp_payload_o_TREADY;
    end else begin
        tx_resp_payload_o_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter2_fsm_state3) & (ap_predicate_op64_write_state3 == 1'b1))) begin
        tx_resp_payload_o_TVALID = 1'b1;
    end else begin
        tx_resp_payload_o_TVALID = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_iter0_fsm)
        ap_ST_iter0_fsm_state1 : begin
            ap_NS_iter0_fsm = ap_ST_iter0_fsm_state1;
        end
        default : begin
            ap_NS_iter0_fsm = 'bx;
        end
    endcase
end

always @ (*) begin
    case (ap_CS_iter1_fsm)
        ap_ST_iter1_fsm_state2 : begin
            if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0))) & ~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
                ap_NS_iter1_fsm = ap_ST_iter1_fsm_state2;
            end else if ((~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & ((1'b0 == ap_CS_iter0_fsm_state1) | ((1'b1 == ap_CS_iter0_fsm_state1) & ((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0))))))) begin
                ap_NS_iter1_fsm = ap_ST_iter1_fsm_state0;
            end else begin
                ap_NS_iter1_fsm = ap_ST_iter1_fsm_state2;
            end
        end
        ap_ST_iter1_fsm_state0 : begin
            if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter0_fsm_state1))) begin
                ap_NS_iter1_fsm = ap_ST_iter1_fsm_state2;
            end else begin
                ap_NS_iter1_fsm = ap_ST_iter1_fsm_state0;
            end
        end
        default : begin
            ap_NS_iter1_fsm = 'bx;
        end
    endcase
end

always @ (*) begin
    case (ap_CS_iter2_fsm)
        ap_ST_iter2_fsm_state3 : begin
            if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b1 == ap_CS_iter1_fsm_state2))) begin
                ap_NS_iter2_fsm = ap_ST_iter2_fsm_state3;
            end else if ((~((1'b1 == ap_block_state3_io) | (ap_done_reg == 1'b1) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))) & (1'b0 == ap_CS_iter1_fsm_state2))) begin
                ap_NS_iter2_fsm = ap_ST_iter2_fsm_state0;
            end else begin
                ap_NS_iter2_fsm = ap_ST_iter2_fsm_state3;
            end
        end
        ap_ST_iter2_fsm_state0 : begin
            if ((~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter1_fsm_state2))) begin
                ap_NS_iter2_fsm = ap_ST_iter2_fsm_state3;
            end else begin
                ap_NS_iter2_fsm = ap_ST_iter2_fsm_state0;
            end
        end
        default : begin
            ap_NS_iter2_fsm = 'bx;
        end
    endcase
end

assign add_ln11_fu_257_p2 = ($signed(zext_ln870_fu_247_p1) + $signed(9'd499));

assign ap_CS_iter0_fsm_state1 = ap_CS_iter0_fsm[32'd0];

assign ap_CS_iter1_fsm_state0 = ap_CS_iter1_fsm[32'd0];

assign ap_CS_iter1_fsm_state2 = ap_CS_iter1_fsm[32'd1];

assign ap_CS_iter2_fsm_state0 = ap_CS_iter2_fsm[32'd0];

assign ap_CS_iter2_fsm_state3 = ap_CS_iter2_fsm[32'd1];

always @ (*) begin
    ap_block_state1_pp0_stage0_iter0 = ((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)));
end

assign ap_block_state2_pp0_stage0_iter1 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_state3_io = (((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0)));
end

always @ (*) begin
    ap_block_state3_pp0_stage0_iter2 = (((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0)));
end

always @ (*) begin
    ap_condition_141 = (~((ap_done_reg == 1'b1) | (ap_start == 1'b0) | ((ap_predicate_op24_read_state1 == 1'b1) & (rx_payload_empty_n == 1'b0)) | ((ap_predicate_op8_read_state1 == 1'b1) & (rx_headers_internal_empty_n == 1'b0)) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter0_fsm_state1));
end

always @ (*) begin
    ap_condition_244 = ((tmp_1_i_nbreadreq_fu_118_p3 == 1'd1) & (state == 1'd0) & (icmp_ln424_fu_269_p2 == 1'd0));
end

always @ (*) begin
    ap_condition_247 = ((tmp_i_nbreadreq_fu_132_p3 == 1'd1) & (state == 1'd1) & (tmp_last_V_fu_295_p3 == 1'd1));
end

always @ (*) begin
    ap_condition_379 = (~((ap_done_reg == 1'b1) | ((1'b1 == ap_CS_iter2_fsm_state3) & ((1'b1 == ap_block_state3_io) | ((ap_predicate_op56_write_state3 == 1'b1) & (rx_req_o_V_TREADY == 1'b0)) | ((ap_predicate_op64_write_state3 == 1'b1) & (tx_resp_payload_o_TREADY == 1'b0)) | ((ap_predicate_op62_write_state3 == 1'b1) & (rx_req_payload_o_TREADY == 1'b0)) | ((ap_predicate_op60_write_state3 == 1'b1) & (tx_resp_o_V_TREADY == 1'b0))))) & (1'b1 == ap_CS_iter1_fsm_state2) & (tmp_1_i_reg_377_pp0_iter0_reg == 1'd1) & (state_load_reg_372_pp0_iter0_reg == 1'd0));
end

always @ (*) begin
    ap_predicate_op24_read_state1 = ((tmp_i_nbreadreq_fu_132_p3 == 1'd1) & (state == 1'd1));
end

always @ (*) begin
    ap_predicate_op56_write_state3 = ((icmp_ln11_reg_402_pp0_iter1_reg == 1'd0) & (tmp_1_i_reg_377_pp0_iter1_reg == 1'd1) & (state_load_reg_372_pp0_iter1_reg == 1'd0));
end

always @ (*) begin
    ap_predicate_op60_write_state3 = ((icmp_ln11_reg_402_pp0_iter1_reg == 1'd1) & (tmp_1_i_reg_377_pp0_iter1_reg == 1'd1) & (state_load_reg_372_pp0_iter1_reg == 1'd0));
end

always @ (*) begin
    ap_predicate_op62_write_state3 = ((state_load_reg_372_pp0_iter1_reg == 1'd1) & (payload_dir_load_reg_435 == 1'd0) & (drop_payload_load_reg_431 == 1'd0) & (tmp_i_reg_409_pp0_iter1_reg == 1'd1));
end

always @ (*) begin
    ap_predicate_op64_write_state3 = ((state_load_reg_372_pp0_iter1_reg == 1'd1) & (payload_dir_load_reg_435 == 1'd1) & (drop_payload_load_reg_431 == 1'd0) & (tmp_i_reg_409_pp0_iter1_reg == 1'd1));
end

always @ (*) begin
    ap_predicate_op8_read_state1 = ((tmp_1_i_nbreadreq_fu_118_p3 == 1'd1) & (state == 1'd0));
end

assign drop_payload_load_load_fu_332_p1 = drop_payload;

assign hdr_bth_opcode_V_fu_217_p4 = {{rx_headers_internal_dout[39:32]}};

assign icmp_ln11_fu_263_p2 = ((add_ln11_fu_257_p2 < 9'd5) ? 1'b1 : 1'b0);

assign icmp_ln11_reg_402_pp0_iter0_reg = icmp_ln11_reg_402;

assign icmp_ln424_fu_269_p2 = ((tmp_payload_length_fu_237_p4 == 16'd0) ? 1'b1 : 1'b0);

assign icmp_ln870_fu_251_p2 = ((hdr_bth_opcode_V_fu_217_p4 == 8'd17) ? 1'b1 : 1'b0);

assign rx_req_o_V_TDATA = {{tmp_1_fu_344_p4}, {trunc_ln145_1_reg_387_pp0_iter1_reg}};

assign rx_req_payload_o_TDATA = trunc_ln145_reg_413_pp0_iter1_reg;

assign rx_req_payload_o_TDEST = 1'd0;

assign rx_req_payload_o_TKEEP = tmp_i4_keep_i_reg_419_pp0_iter1_reg;

assign rx_req_payload_o_TLAST = tmp_last_V_reg_425_pp0_iter1_reg;

assign rx_req_payload_o_TSTRB = 64'd0;

assign state_load_cast_i_fu_340_p1 = state_load_reg_372_pp0_iter1_reg;

assign state_load_reg_372_pp0_iter0_reg = state_load_reg_372;

assign tmp_1_fu_344_p4 = {{rx_headers_internal_read_reg_381_pp0_iter1_reg[415:224]}};

assign tmp_1_i_nbreadreq_fu_118_p3 = rx_headers_internal_empty_n;

assign tmp_1_i_reg_377_pp0_iter0_reg = tmp_1_i_reg_377;

assign tmp_i_nbreadreq_fu_132_p3 = rx_payload_empty_n;

assign tmp_i_reg_409_pp0_iter0_reg = tmp_i_reg_409;

assign tmp_last_V_fu_295_p3 = rx_payload_dout[32'd576];

assign tmp_payload_length_fu_237_p4 = {{rx_headers_internal_dout[399:384]}};

assign trunc_ln145_1_fu_213_p1 = rx_headers_internal_dout[159:0];

assign trunc_ln145_fu_281_p1 = rx_payload_dout[511:0];

assign trunc_ln174_fu_361_p1 = rx_headers_internal_read_reg_381_pp0_iter1_reg[223:0];

assign tx_resp_o_V_TDATA = {{trunc_ln145_s_reg_392_pp0_iter1_reg}, {trunc_ln174_fu_361_p1}};

assign tx_resp_payload_o_TDATA = trunc_ln145_reg_413_pp0_iter1_reg;

assign tx_resp_payload_o_TDEST = 1'd0;

assign tx_resp_payload_o_TKEEP = tmp_i4_keep_i_reg_419_pp0_iter1_reg;

assign tx_resp_payload_o_TLAST = tmp_last_V_reg_425_pp0_iter1_reg;

assign tx_resp_payload_o_TSTRB = 64'd0;

assign zext_ln870_fu_247_p1 = hdr_bth_opcode_V_fu_217_p4;

always @ (posedge ap_clk) begin
    state_req_resp_switch_preg[31:1] <= 31'b0000000000000000000000000000000;
end

endmodule //rx_rdma_hdr_req_resp_switch