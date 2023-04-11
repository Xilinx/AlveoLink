// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Version: 2022.1
// Copyright (C) Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="cmac_flowcontrol_tx_dev_cmac_flowcontrol_tx_dev,hls_ip_2022_1,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcu55c-fsvh2892-2L-e,HLS_INPUT_CLOCK=3.000000,HLS_INPUT_ARCH=pipeline,HLS_SYN_CLOCK=1.147000,HLS_SYN_LAT=1,HLS_SYN_TPT=1,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=199,HLS_SYN_LUT=197,HLS_VERSION=2022_1}" *)

module cmac_flowcontrol_tx_dev (
        ap_clk,
        ap_rst_n,
        tx_i_TDATA,
        tx_i_TVALID,
        tx_i_TREADY,
        tx_i_TKEEP,
        tx_i_TSTRB,
        tx_i_TLAST,
        tx_i_TDEST,
        tx_o_TDATA,
        tx_o_TVALID,
        tx_o_TREADY,
        tx_o_TKEEP,
        tx_o_TSTRB,
        tx_o_TLAST,
        tx_o_TDEST,
        cmac_rx_pause_req,
        stat_rx_pause,
        pause_frames_cnt_o,
        total_pause_frames_dur_o
);

parameter    ap_ST_fsm_pp0_stage0 = 1'd1;

input   ap_clk;
input   ap_rst_n;
input  [511:0] tx_i_TDATA;
input   tx_i_TVALID;
output   tx_i_TREADY;
input  [63:0] tx_i_TKEEP;
input  [63:0] tx_i_TSTRB;
input  [0:0] tx_i_TLAST;
input  [0:0] tx_i_TDEST;
output  [511:0] tx_o_TDATA;
output   tx_o_TVALID;
input   tx_o_TREADY;
output  [63:0] tx_o_TKEEP;
output  [63:0] tx_o_TSTRB;
output  [0:0] tx_o_TLAST;
output  [0:0] tx_o_TDEST;
input  [8:0] cmac_rx_pause_req;
input  [0:0] stat_rx_pause;
output  [31:0] pause_frames_cnt_o;
output  [63:0] total_pause_frames_dur_o;

 reg    ap_rst_n_inv;
reg   [31:0] pause_frames_cnt;
reg   [63:0] total_pause_frames_dur;
reg   [0:0] state;
reg    tx_i_TDATA_blk_n;
(* fsm_encoding = "none" *) reg   [0:0] ap_CS_fsm;
wire    ap_CS_fsm_pp0_stage0;
wire    ap_block_pp0_stage0;
wire   [0:0] icmp_ln1069_fu_239_p2;
wire   [0:0] grp_nbreadreq_fu_100_p7;
reg    tx_o_TDATA_blk_n;
reg    ap_enable_reg_pp0_iter1;
reg   [0:0] icmp_ln1069_reg_290;
reg   [0:0] state_load_reg_304;
reg   [0:0] tmp_1_reg_308;
reg   [0:0] tmp_reg_322;
reg    ap_predicate_op48_read_state1;
reg    ap_predicate_op54_write_state1;
reg    ap_predicate_op62_read_state1;
reg    ap_predicate_op68_write_state1;
reg    ap_block_state1_pp0_stage0_iter0;
reg    ap_block_state1_io;
reg    ap_predicate_op78_write_state2;
reg    ap_predicate_op79_write_state2;
wire    regslice_both_tx_o_V_data_V_U_apdone_blk;
reg    ap_block_state2_pp0_stage0_iter1;
reg    ap_block_state2_io;
reg    ap_block_pp0_stage0_11001;
wire   [31:0] add_ln34_fu_227_p2;
wire   [0:0] stat_rx_pause_read_read_fu_88_p2;
wire   [63:0] add_ln38_fu_249_p2;
wire   [0:0] grp_fu_198_p1;
reg    ap_block_pp0_stage0_subdone;
wire   [31:0] ap_phi_reg_pp0_iter0_pause_frames_cnt_loc_0_reg_163;
reg   [31:0] ap_phi_reg_pp0_iter1_pause_frames_cnt_loc_0_reg_163;
wire   [63:0] ap_phi_reg_pp0_iter0_total_pause_frames_dur_loc_0_reg_173;
reg   [63:0] ap_phi_reg_pp0_iter1_total_pause_frames_dur_loc_0_reg_173;
reg    ap_block_pp0_stage0_01001;
reg   [0:0] ap_NS_fsm;
wire    ap_reset_idle_pp0;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
wire    regslice_both_tx_i_V_data_V_U_apdone_blk;
wire   [511:0] tx_i_TDATA_int_regslice;
wire    tx_i_TVALID_int_regslice;
reg    tx_i_TREADY_int_regslice;
wire    regslice_both_tx_i_V_data_V_U_ack_in;
wire    regslice_both_tx_i_V_keep_V_U_apdone_blk;
wire   [63:0] tx_i_TKEEP_int_regslice;
wire    regslice_both_tx_i_V_keep_V_U_vld_out;
wire    regslice_both_tx_i_V_keep_V_U_ack_in;
wire    regslice_both_tx_i_V_strb_V_U_apdone_blk;
wire   [63:0] tx_i_TSTRB_int_regslice;
wire    regslice_both_tx_i_V_strb_V_U_vld_out;
wire    regslice_both_tx_i_V_strb_V_U_ack_in;
wire    regslice_both_tx_i_V_last_V_U_apdone_blk;
wire   [0:0] tx_i_TLAST_int_regslice;
wire    regslice_both_tx_i_V_last_V_U_vld_out;
wire    regslice_both_tx_i_V_last_V_U_ack_in;
wire    regslice_both_tx_i_V_dest_V_U_apdone_blk;
wire   [0:0] tx_i_TDEST_int_regslice;
wire    regslice_both_tx_i_V_dest_V_U_vld_out;
wire    regslice_both_tx_i_V_dest_V_U_ack_in;
reg    tx_o_TVALID_int_regslice;
wire    tx_o_TREADY_int_regslice;
wire    regslice_both_tx_o_V_data_V_U_vld_out;
wire    regslice_both_tx_o_V_keep_V_U_apdone_blk;
wire    regslice_both_tx_o_V_keep_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_keep_V_U_vld_out;
wire    regslice_both_tx_o_V_strb_V_U_apdone_blk;
wire    regslice_both_tx_o_V_strb_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_strb_V_U_vld_out;
wire    regslice_both_tx_o_V_last_V_U_apdone_blk;
wire    regslice_both_tx_o_V_last_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_last_V_U_vld_out;
wire    regslice_both_tx_o_V_dest_V_U_apdone_blk;
wire    regslice_both_tx_o_V_dest_V_U_ack_in_dummy;
wire    regslice_both_tx_o_V_dest_V_U_vld_out;
reg    ap_condition_133;
reg    ap_condition_385;
reg    ap_condition_384;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 pause_frames_cnt = 32'd0;
#0 total_pause_frames_dur = 64'd0;
#0 state = 1'd0;
#0 ap_CS_fsm = 1'd1;
#0 ap_enable_reg_pp0_iter1 = 1'b0;
end

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 512 ))
regslice_both_tx_i_V_data_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TDATA),
    .vld_in(tx_i_TVALID),
    .ack_in(regslice_both_tx_i_V_data_V_U_ack_in),
    .data_out(tx_i_TDATA_int_regslice),
    .vld_out(tx_i_TVALID_int_regslice),
    .ack_out(tx_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_i_V_data_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_i_V_keep_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TKEEP),
    .vld_in(tx_i_TVALID),
    .ack_in(regslice_both_tx_i_V_keep_V_U_ack_in),
    .data_out(tx_i_TKEEP_int_regslice),
    .vld_out(regslice_both_tx_i_V_keep_V_U_vld_out),
    .ack_out(tx_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_i_V_keep_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_i_V_strb_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TSTRB),
    .vld_in(tx_i_TVALID),
    .ack_in(regslice_both_tx_i_V_strb_V_U_ack_in),
    .data_out(tx_i_TSTRB_int_regslice),
    .vld_out(regslice_both_tx_i_V_strb_V_U_vld_out),
    .ack_out(tx_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_i_V_strb_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_i_V_last_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TLAST),
    .vld_in(tx_i_TVALID),
    .ack_in(regslice_both_tx_i_V_last_V_U_ack_in),
    .data_out(tx_i_TLAST_int_regslice),
    .vld_out(regslice_both_tx_i_V_last_V_U_vld_out),
    .ack_out(tx_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_i_V_last_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_i_V_dest_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TDEST),
    .vld_in(tx_i_TVALID),
    .ack_in(regslice_both_tx_i_V_dest_V_U_ack_in),
    .data_out(tx_i_TDEST_int_regslice),
    .vld_out(regslice_both_tx_i_V_dest_V_U_vld_out),
    .ack_out(tx_i_TREADY_int_regslice),
    .apdone_blk(regslice_both_tx_i_V_dest_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 512 ))
regslice_both_tx_o_V_data_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TDATA_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(tx_o_TREADY_int_regslice),
    .data_out(tx_o_TDATA),
    .vld_out(regslice_both_tx_o_V_data_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_data_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_o_V_keep_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TKEEP_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_keep_V_U_ack_in_dummy),
    .data_out(tx_o_TKEEP),
    .vld_out(regslice_both_tx_o_V_keep_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_keep_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 64 ))
regslice_both_tx_o_V_strb_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TSTRB_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_strb_V_U_ack_in_dummy),
    .data_out(tx_o_TSTRB),
    .vld_out(regslice_both_tx_o_V_strb_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_strb_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_o_V_last_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TLAST_int_regslice),
    .vld_in(tx_o_TVALID_int_regslice),
    .ack_in(regslice_both_tx_o_V_last_V_U_ack_in_dummy),
    .data_out(tx_o_TLAST),
    .vld_out(regslice_both_tx_o_V_last_V_U_vld_out),
    .ack_out(tx_o_TREADY),
    .apdone_blk(regslice_both_tx_o_V_last_V_U_apdone_blk)
);

cmac_flowcontrol_tx_dev_regslice_both #(
    .DataWidth( 1 ))
regslice_both_tx_o_V_dest_V_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .data_in(tx_i_TDEST_int_regslice),
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
        if (((1'b0 == ap_block_pp0_stage0_subdone) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
            ap_enable_reg_pp0_iter1 <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        pause_frames_cnt <= 32'd0;
    end else begin
        if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (stat_rx_pause_read_read_fu_88_p2 == 1'd1))) begin
            pause_frames_cnt <= add_ln34_fu_227_p2;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        state <= 1'd0;
    end else begin
        if ((1'b1 == ap_condition_384)) begin
            if (((state == 1'd1) & (grp_fu_198_p1 == 1'd1))) begin
                state <= 1'd0;
            end else if ((1'b1 == ap_condition_385)) begin
                state <= 1'd1;
            end
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst_n_inv == 1'b1) begin
        total_pause_frames_dur <= 64'd0;
    end else begin
        if (((icmp_ln1069_fu_239_p2 == 1'd0) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
            total_pause_frames_dur <= add_ln38_fu_249_p2;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_133)) begin
        if ((stat_rx_pause_read_read_fu_88_p2 == 1'd0)) begin
            ap_phi_reg_pp0_iter1_pause_frames_cnt_loc_0_reg_163 <= pause_frames_cnt;
        end else if ((stat_rx_pause_read_read_fu_88_p2 == 1'd1)) begin
            ap_phi_reg_pp0_iter1_pause_frames_cnt_loc_0_reg_163 <= add_ln34_fu_227_p2;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_pause_frames_cnt_loc_0_reg_163 <= ap_phi_reg_pp0_iter0_pause_frames_cnt_loc_0_reg_163;
        end
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_condition_133)) begin
        if ((icmp_ln1069_fu_239_p2 == 1'd0)) begin
            ap_phi_reg_pp0_iter1_total_pause_frames_dur_loc_0_reg_173 <= add_ln38_fu_249_p2;
        end else if ((icmp_ln1069_fu_239_p2 == 1'd1)) begin
            ap_phi_reg_pp0_iter1_total_pause_frames_dur_loc_0_reg_173 <= total_pause_frames_dur;
        end else if ((1'b1 == 1'b1)) begin
            ap_phi_reg_pp0_iter1_total_pause_frames_dur_loc_0_reg_173 <= ap_phi_reg_pp0_iter0_total_pause_frames_dur_loc_0_reg_173;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        icmp_ln1069_reg_290 <= icmp_ln1069_fu_239_p2;
        state_load_reg_304 <= state;
    end
end

always @ (posedge ap_clk) begin
    if (((state == 1'd0) & (icmp_ln1069_fu_239_p2 == 1'd1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tmp_1_reg_308 <= grp_nbreadreq_fu_100_p7;
    end
end

always @ (posedge ap_clk) begin
    if (((state == 1'd1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0))) begin
        tmp_reg_322 <= grp_nbreadreq_fu_100_p7;
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
    if ((((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd0) & (icmp_ln1069_fu_239_p2 == 1'd1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)))) begin
        tx_i_TDATA_blk_n = tx_i_TVALID_int_regslice;
    end else begin
        tx_i_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((((ap_predicate_op48_read_state1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_predicate_op62_read_state1 == 1'b1)))) begin
        tx_i_TREADY_int_regslice = 1'b1;
    end else begin
        tx_i_TREADY_int_regslice = 1'b0;
    end
end

always @ (*) begin
    if ((((tmp_reg_322 == 1'd1) & (state_load_reg_304 == 1'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((tmp_1_reg_308 == 1'd1) & (state_load_reg_304 == 1'd0) & (icmp_ln1069_reg_290 == 1'd1) & (ap_enable_reg_pp0_iter1 == 1'b1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd0) & (icmp_ln1069_fu_239_p2 == 1'd1) & (1'b0 == ap_block_pp0_stage0) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)))) begin
        tx_o_TDATA_blk_n = tx_o_TREADY_int_regslice;
    end else begin
        tx_o_TDATA_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((((ap_predicate_op54_write_state1 == 1'b1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0)) | ((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0) & (ap_predicate_op68_write_state1 == 1'b1)))) begin
        tx_o_TVALID_int_regslice = 1'b1;
    end else begin
        tx_o_TVALID_int_regslice = 1'b0;
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

assign add_ln34_fu_227_p2 = (pause_frames_cnt + 32'd1);

assign add_ln38_fu_249_p2 = (total_pause_frames_dur + 64'd1);

assign ap_CS_fsm_pp0_stage0 = ap_CS_fsm[32'd0];

assign ap_block_pp0_stage0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_pp0_stage0_01001 = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op79_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op78_write_state2 == 1'b1)))) | ((1'b1 == 1'b1) & (((ap_predicate_op54_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op48_read_state1 == 1'b1) & (tx_i_TVALID_int_regslice == 1'b0)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op68_write_state1 == 1'b1)) | ((tx_i_TVALID_int_regslice == 1'b0) & (ap_predicate_op62_read_state1 == 1'b1)))));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((1'b1 == ap_block_state2_io) | (regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op79_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op78_write_state2 == 1'b1)))) | ((1'b1 == 1'b1) & ((1'b1 == ap_block_state1_io) | ((ap_predicate_op54_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op48_read_state1 == 1'b1) & (tx_i_TVALID_int_regslice == 1'b0)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op68_write_state1 == 1'b1)) | ((tx_i_TVALID_int_regslice == 1'b0) & (ap_predicate_op62_read_state1 == 1'b1)))));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = (((ap_enable_reg_pp0_iter1 == 1'b1) & ((1'b1 == ap_block_state2_io) | (regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op79_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op78_write_state2 == 1'b1)))) | ((1'b1 == 1'b1) & ((1'b1 == ap_block_state1_io) | ((ap_predicate_op54_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op48_read_state1 == 1'b1) & (tx_i_TVALID_int_regslice == 1'b0)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op68_write_state1 == 1'b1)) | ((tx_i_TVALID_int_regslice == 1'b0) & (ap_predicate_op62_read_state1 == 1'b1)))));
end

always @ (*) begin
    ap_block_state1_io = (((ap_predicate_op54_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op68_write_state1 == 1'b1)));
end

always @ (*) begin
    ap_block_state1_pp0_stage0_iter0 = (((ap_predicate_op54_write_state1 == 1'b1) & (tx_o_TREADY_int_regslice == 1'b0)) | ((ap_predicate_op48_read_state1 == 1'b1) & (tx_i_TVALID_int_regslice == 1'b0)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op68_write_state1 == 1'b1)) | ((tx_i_TVALID_int_regslice == 1'b0) & (ap_predicate_op62_read_state1 == 1'b1)));
end

always @ (*) begin
    ap_block_state2_io = (((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op79_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op78_write_state2 == 1'b1)));
end

always @ (*) begin
    ap_block_state2_pp0_stage0_iter1 = ((regslice_both_tx_o_V_data_V_U_apdone_blk == 1'b1) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op79_write_state2 == 1'b1)) | ((tx_o_TREADY_int_regslice == 1'b0) & (ap_predicate_op78_write_state2 == 1'b1)));
end

always @ (*) begin
    ap_condition_133 = ((1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0));
end

always @ (*) begin
    ap_condition_384 = ((grp_nbreadreq_fu_100_p7 == 1'd1) & (1'b0 == ap_block_pp0_stage0_11001) & (1'b1 == 1'b1) & (1'b1 == ap_CS_fsm_pp0_stage0));
end

always @ (*) begin
    ap_condition_385 = ((state == 1'd0) & (icmp_ln1069_fu_239_p2 == 1'd1) & (grp_fu_198_p1 == 1'd0));
end

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

assign ap_phi_reg_pp0_iter0_pause_frames_cnt_loc_0_reg_163 = 'bx;

assign ap_phi_reg_pp0_iter0_total_pause_frames_dur_loc_0_reg_173 = 'bx;

always @ (*) begin
    ap_predicate_op48_read_state1 = ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd0) & (icmp_ln1069_fu_239_p2 == 1'd1));
end

always @ (*) begin
    ap_predicate_op54_write_state1 = ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd0) & (icmp_ln1069_fu_239_p2 == 1'd1));
end

always @ (*) begin
    ap_predicate_op62_read_state1 = ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd1));
end

always @ (*) begin
    ap_predicate_op68_write_state1 = ((grp_nbreadreq_fu_100_p7 == 1'd1) & (state == 1'd1));
end

always @ (*) begin
    ap_predicate_op78_write_state2 = ((tmp_1_reg_308 == 1'd1) & (state_load_reg_304 == 1'd0) & (icmp_ln1069_reg_290 == 1'd1));
end

always @ (*) begin
    ap_predicate_op79_write_state2 = ((tmp_reg_322 == 1'd1) & (state_load_reg_304 == 1'd1));
end

always @ (*) begin
    ap_rst_n_inv = ~ap_rst_n;
end

assign grp_fu_198_p1 = tx_i_TLAST_int_regslice;

assign grp_nbreadreq_fu_100_p7 = tx_i_TVALID_int_regslice;

assign icmp_ln1069_fu_239_p2 = ((cmac_rx_pause_req == 9'd0) ? 1'b1 : 1'b0);

assign pause_frames_cnt_o = ap_phi_reg_pp0_iter1_pause_frames_cnt_loc_0_reg_163;

assign stat_rx_pause_read_read_fu_88_p2 = stat_rx_pause;

assign total_pause_frames_dur_o = ap_phi_reg_pp0_iter1_total_pause_frames_dur_loc_0_reg_173;

assign tx_i_TREADY = regslice_both_tx_i_V_data_V_U_ack_in;

assign tx_o_TVALID = regslice_both_tx_o_V_data_V_U_vld_out;


reg find_kernel_block = 0;
// synthesis translate_off
`include "cmac_flowcontrol_tx_dev_hls_deadlock_kernel_monitor_top.vh"
// synthesis translate_on

endmodule //cmac_flowcontrol_tx_dev

