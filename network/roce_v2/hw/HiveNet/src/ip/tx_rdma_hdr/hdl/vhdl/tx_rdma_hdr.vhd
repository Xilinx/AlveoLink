-- ==============================================================
-- RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.2 (64-bit)
-- Version: 2020.2
-- Copyright (C) Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- 
-- ===========================================================

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity tx_rdma_hdr is
port (
    axis_frame_ctx_tx_V_TDATA : IN STD_LOGIC_VECTOR (319 downto 0);
    axis_frame_ctx_rx_V_TDATA : IN STD_LOGIC_VECTOR (319 downto 0);
    tx_in_TDATA : IN STD_LOGIC_VECTOR (511 downto 0);
    tx_in_TKEEP : IN STD_LOGIC_VECTOR (63 downto 0);
    tx_in_TSTRB : IN STD_LOGIC_VECTOR (63 downto 0);
    tx_in_TLAST : IN STD_LOGIC_VECTOR (0 downto 0);
    tx_in_TDEST : IN STD_LOGIC_VECTOR (0 downto 0);
    tx_out_TDATA : OUT STD_LOGIC_VECTOR (511 downto 0);
    tx_out_TKEEP : OUT STD_LOGIC_VECTOR (63 downto 0);
    tx_out_TSTRB : OUT STD_LOGIC_VECTOR (63 downto 0);
    tx_out_TLAST : OUT STD_LOGIC_VECTOR (0 downto 0);
    tx_out_TDEST : OUT STD_LOGIC_VECTOR (0 downto 0);
    rdma_udp_port_dst : IN STD_LOGIC_VECTOR (15 downto 0);
    my_ip : IN STD_LOGIC_VECTOR (31 downto 0);
    offst : OUT STD_LOGIC_VECTOR (31 downto 0);
    packet_len : OUT STD_LOGIC_VECTOR (31 downto 0);
    ecn_for_ack : IN STD_LOGIC_VECTOR (0 downto 0);
    ap_clk : IN STD_LOGIC;
    ap_rst_n : IN STD_LOGIC;
    axis_frame_ctx_tx_V_TVALID : IN STD_LOGIC;
    axis_frame_ctx_tx_V_TREADY : OUT STD_LOGIC;
    axis_frame_ctx_rx_V_TVALID : IN STD_LOGIC;
    axis_frame_ctx_rx_V_TREADY : OUT STD_LOGIC;
    offst_ap_vld : OUT STD_LOGIC;
    tx_in_TVALID : IN STD_LOGIC;
    tx_in_TREADY : OUT STD_LOGIC;
    packet_len_ap_vld : OUT STD_LOGIC;
    tx_out_TVALID : OUT STD_LOGIC;
    tx_out_TREADY : IN STD_LOGIC );
end;


architecture behav of tx_rdma_hdr is 
    attribute CORE_GENERATION_INFO : STRING;
    attribute CORE_GENERATION_INFO of behav : architecture is
    "tx_rdma_hdr_tx_rdma_hdr,hls_ip_2020_2,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcu55c-fsvh2892-2L-e,HLS_INPUT_CLOCK=3.000000,HLS_INPUT_ARCH=dataflow,HLS_SYN_CLOCK=6.387750,HLS_SYN_LAT=6,HLS_SYN_TPT=1,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=8706,HLS_SYN_LUT=70772,HLS_VERSION=2020_2}";
    constant ap_const_lv512_lc_1 : STD_LOGIC_VECTOR (511 downto 0) := "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    constant ap_const_lv64_0 : STD_LOGIC_VECTOR (63 downto 0) := "0000000000000000000000000000000000000000000000000000000000000000";
    constant ap_const_lv1_0 : STD_LOGIC_VECTOR (0 downto 0) := "0";
    constant ap_const_lv32_0 : STD_LOGIC_VECTOR (31 downto 0) := "00000000000000000000000000000000";
    constant ap_const_logic_1 : STD_LOGIC := '1';
    constant ap_const_logic_0 : STD_LOGIC := '0';

    signal ap_rst_n_inv : STD_LOGIC;
    signal tx_rdma_hdr_entry3_U0_ap_start : STD_LOGIC;
    signal tx_rdma_hdr_entry3_U0_ap_done : STD_LOGIC;
    signal tx_rdma_hdr_entry3_U0_ap_continue : STD_LOGIC;
    signal tx_rdma_hdr_entry3_U0_ap_idle : STD_LOGIC;
    signal tx_rdma_hdr_entry3_U0_ap_ready : STD_LOGIC;
    signal tx_rdma_hdr_entry3_U0_ecn_for_ack_out_din : STD_LOGIC_VECTOR (0 downto 0);
    signal tx_rdma_hdr_entry3_U0_ecn_for_ack_out_write : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ap_start : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ap_done : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ap_continue : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ap_idle : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ap_ready : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ecn_for_ack_read : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_ecn_for_ack_out_din : STD_LOGIC_VECTOR (0 downto 0);
    signal lshiftWordByOctet_512_0_4_U0_ecn_for_ack_out_write : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_tx_to_hdr_gen_din : STD_LOGIC_VECTOR (1023 downto 0);
    signal lshiftWordByOctet_512_0_4_U0_tx_to_hdr_gen_write : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_to_hdr_gen_din : STD_LOGIC_VECTOR (280 downto 0);
    signal lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_to_hdr_gen_write : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_tx_V_TREADY : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_rx_V_TREADY : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_offst : STD_LOGIC_VECTOR (31 downto 0);
    signal lshiftWordByOctet_512_0_4_U0_offst_ap_vld : STD_LOGIC;
    signal lshiftWordByOctet_512_0_4_U0_tx_in_TREADY : STD_LOGIC;
    signal ap_sync_continue : STD_LOGIC;
    signal put_hdr_U0_ap_start : STD_LOGIC;
    signal put_hdr_U0_ap_done : STD_LOGIC;
    signal put_hdr_U0_ap_continue : STD_LOGIC;
    signal put_hdr_U0_ap_idle : STD_LOGIC;
    signal put_hdr_U0_ap_ready : STD_LOGIC;
    signal put_hdr_U0_ecn_for_ack_read : STD_LOGIC;
    signal put_hdr_U0_tx_to_hdr_gen_read : STD_LOGIC;
    signal put_hdr_U0_axis_frame_ctx_to_hdr_gen_read : STD_LOGIC;
    signal put_hdr_U0_payload_remainder_din : STD_LOGIC_VECTOR (1 downto 0);
    signal put_hdr_U0_payload_remainder_write : STD_LOGIC;
    signal put_hdr_U0_tx_to_crc_and_remainder_din : STD_LOGIC_VECTOR (1023 downto 0);
    signal put_hdr_U0_tx_to_crc_and_remainder_write : STD_LOGIC;
    signal put_hdr_U0_packet_len : STD_LOGIC_VECTOR (31 downto 0);
    signal put_hdr_U0_packet_len_ap_vld : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_ap_start : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_ap_done : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_ap_continue : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_ap_idle : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_ap_ready : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_to_crc_and_remainder_read : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_payload_remainder_read : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TDATA : STD_LOGIC_VECTOR (511 downto 0);
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TVALID : STD_LOGIC;
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TKEEP : STD_LOGIC_VECTOR (63 downto 0);
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TSTRB : STD_LOGIC_VECTOR (63 downto 0);
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TLAST : STD_LOGIC_VECTOR (0 downto 0);
    signal calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TDEST : STD_LOGIC_VECTOR (0 downto 0);
    signal ecn_for_ack_c1_full_n : STD_LOGIC;
    signal ecn_for_ack_c1_dout : STD_LOGIC_VECTOR (0 downto 0);
    signal ecn_for_ack_c1_empty_n : STD_LOGIC;
    signal ecn_for_ack_c_full_n : STD_LOGIC;
    signal ecn_for_ack_c_dout : STD_LOGIC_VECTOR (0 downto 0);
    signal ecn_for_ack_c_empty_n : STD_LOGIC;
    signal axis_frame_ctx_to_hdr_gen_full_n : STD_LOGIC;
    signal axis_frame_ctx_to_hdr_gen_dout : STD_LOGIC_VECTOR (280 downto 0);
    signal axis_frame_ctx_to_hdr_gen_empty_n : STD_LOGIC;
    signal tx_to_hdr_gen_full_n : STD_LOGIC;
    signal tx_to_hdr_gen_dout : STD_LOGIC_VECTOR (1023 downto 0);
    signal tx_to_hdr_gen_empty_n : STD_LOGIC;
    signal payload_remainder_full_n : STD_LOGIC;
    signal payload_remainder_dout : STD_LOGIC_VECTOR (1 downto 0);
    signal payload_remainder_empty_n : STD_LOGIC;
    signal tx_to_crc_and_remainder_full_n : STD_LOGIC;
    signal tx_to_crc_and_remainder_dout : STD_LOGIC_VECTOR (1023 downto 0);
    signal tx_to_crc_and_remainder_empty_n : STD_LOGIC;

    component tx_rdma_hdr_tx_rdma_hdr_entry3 IS
    port (
        ap_clk : IN STD_LOGIC;
        ap_rst : IN STD_LOGIC;
        ap_start : IN STD_LOGIC;
        ap_done : OUT STD_LOGIC;
        ap_continue : IN STD_LOGIC;
        ap_idle : OUT STD_LOGIC;
        ap_ready : OUT STD_LOGIC;
        ecn_for_ack : IN STD_LOGIC_VECTOR (0 downto 0);
        ecn_for_ack_out_din : OUT STD_LOGIC_VECTOR (0 downto 0);
        ecn_for_ack_out_full_n : IN STD_LOGIC;
        ecn_for_ack_out_write : OUT STD_LOGIC );
    end component;


    component tx_rdma_hdr_lshiftWordByOctet_512_0_4 IS
    port (
        ap_clk : IN STD_LOGIC;
        ap_rst : IN STD_LOGIC;
        ap_start : IN STD_LOGIC;
        ap_done : OUT STD_LOGIC;
        ap_continue : IN STD_LOGIC;
        ap_idle : OUT STD_LOGIC;
        ap_ready : OUT STD_LOGIC;
        ecn_for_ack_dout : IN STD_LOGIC_VECTOR (0 downto 0);
        ecn_for_ack_empty_n : IN STD_LOGIC;
        ecn_for_ack_read : OUT STD_LOGIC;
        ecn_for_ack_out_din : OUT STD_LOGIC_VECTOR (0 downto 0);
        ecn_for_ack_out_full_n : IN STD_LOGIC;
        ecn_for_ack_out_write : OUT STD_LOGIC;
        tx_in_TVALID : IN STD_LOGIC;
        axis_frame_ctx_tx_V_TVALID : IN STD_LOGIC;
        axis_frame_ctx_rx_V_TVALID : IN STD_LOGIC;
        tx_to_hdr_gen_din : OUT STD_LOGIC_VECTOR (1023 downto 0);
        tx_to_hdr_gen_full_n : IN STD_LOGIC;
        tx_to_hdr_gen_write : OUT STD_LOGIC;
        axis_frame_ctx_to_hdr_gen_din : OUT STD_LOGIC_VECTOR (280 downto 0);
        axis_frame_ctx_to_hdr_gen_full_n : IN STD_LOGIC;
        axis_frame_ctx_to_hdr_gen_write : OUT STD_LOGIC;
        axis_frame_ctx_tx_V_TDATA : IN STD_LOGIC_VECTOR (319 downto 0);
        axis_frame_ctx_tx_V_TREADY : OUT STD_LOGIC;
        axis_frame_ctx_rx_V_TDATA : IN STD_LOGIC_VECTOR (319 downto 0);
        axis_frame_ctx_rx_V_TREADY : OUT STD_LOGIC;
        offst : OUT STD_LOGIC_VECTOR (31 downto 0);
        offst_ap_vld : OUT STD_LOGIC;
        tx_in_TDATA : IN STD_LOGIC_VECTOR (511 downto 0);
        tx_in_TREADY : OUT STD_LOGIC;
        tx_in_TKEEP : IN STD_LOGIC_VECTOR (63 downto 0);
        tx_in_TSTRB : IN STD_LOGIC_VECTOR (63 downto 0);
        tx_in_TLAST : IN STD_LOGIC_VECTOR (0 downto 0);
        tx_in_TDEST : IN STD_LOGIC_VECTOR (0 downto 0) );
    end component;


    component tx_rdma_hdr_put_hdr IS
    port (
        ap_clk : IN STD_LOGIC;
        ap_rst : IN STD_LOGIC;
        ap_start : IN STD_LOGIC;
        ap_done : OUT STD_LOGIC;
        ap_continue : IN STD_LOGIC;
        ap_idle : OUT STD_LOGIC;
        ap_ready : OUT STD_LOGIC;
        ecn_for_ack_dout : IN STD_LOGIC_VECTOR (0 downto 0);
        ecn_for_ack_empty_n : IN STD_LOGIC;
        ecn_for_ack_read : OUT STD_LOGIC;
        tx_to_hdr_gen_dout : IN STD_LOGIC_VECTOR (1023 downto 0);
        tx_to_hdr_gen_empty_n : IN STD_LOGIC;
        tx_to_hdr_gen_read : OUT STD_LOGIC;
        axis_frame_ctx_to_hdr_gen_dout : IN STD_LOGIC_VECTOR (280 downto 0);
        axis_frame_ctx_to_hdr_gen_empty_n : IN STD_LOGIC;
        axis_frame_ctx_to_hdr_gen_read : OUT STD_LOGIC;
        payload_remainder_din : OUT STD_LOGIC_VECTOR (1 downto 0);
        payload_remainder_full_n : IN STD_LOGIC;
        payload_remainder_write : OUT STD_LOGIC;
        tx_to_crc_and_remainder_din : OUT STD_LOGIC_VECTOR (1023 downto 0);
        tx_to_crc_and_remainder_full_n : IN STD_LOGIC;
        tx_to_crc_and_remainder_write : OUT STD_LOGIC;
        packet_len : OUT STD_LOGIC_VECTOR (31 downto 0);
        packet_len_ap_vld : OUT STD_LOGIC;
        rdma_udp_port_dst : IN STD_LOGIC_VECTOR (15 downto 0);
        my_ip : IN STD_LOGIC_VECTOR (31 downto 0) );
    end component;


    component tx_rdma_hdr_calc_ip_crc_and_reserve_place_for_icrc IS
    port (
        ap_clk : IN STD_LOGIC;
        ap_rst : IN STD_LOGIC;
        ap_start : IN STD_LOGIC;
        ap_done : OUT STD_LOGIC;
        ap_continue : IN STD_LOGIC;
        ap_idle : OUT STD_LOGIC;
        ap_ready : OUT STD_LOGIC;
        tx_to_crc_and_remainder_dout : IN STD_LOGIC_VECTOR (1023 downto 0);
        tx_to_crc_and_remainder_empty_n : IN STD_LOGIC;
        tx_to_crc_and_remainder_read : OUT STD_LOGIC;
        payload_remainder_dout : IN STD_LOGIC_VECTOR (1 downto 0);
        payload_remainder_empty_n : IN STD_LOGIC;
        payload_remainder_read : OUT STD_LOGIC;
        tx_out_TREADY : IN STD_LOGIC;
        tx_out_TDATA : OUT STD_LOGIC_VECTOR (511 downto 0);
        tx_out_TVALID : OUT STD_LOGIC;
        tx_out_TKEEP : OUT STD_LOGIC_VECTOR (63 downto 0);
        tx_out_TSTRB : OUT STD_LOGIC_VECTOR (63 downto 0);
        tx_out_TLAST : OUT STD_LOGIC_VECTOR (0 downto 0);
        tx_out_TDEST : OUT STD_LOGIC_VECTOR (0 downto 0) );
    end component;


    component tx_rdma_hdr_fifo_w1_d2_S IS
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        if_read_ce : IN STD_LOGIC;
        if_write_ce : IN STD_LOGIC;
        if_din : IN STD_LOGIC_VECTOR (0 downto 0);
        if_full_n : OUT STD_LOGIC;
        if_write : IN STD_LOGIC;
        if_dout : OUT STD_LOGIC_VECTOR (0 downto 0);
        if_empty_n : OUT STD_LOGIC;
        if_read : IN STD_LOGIC );
    end component;


    component tx_rdma_hdr_fifo_w281_d2_S IS
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        if_read_ce : IN STD_LOGIC;
        if_write_ce : IN STD_LOGIC;
        if_din : IN STD_LOGIC_VECTOR (280 downto 0);
        if_full_n : OUT STD_LOGIC;
        if_write : IN STD_LOGIC;
        if_dout : OUT STD_LOGIC_VECTOR (280 downto 0);
        if_empty_n : OUT STD_LOGIC;
        if_read : IN STD_LOGIC );
    end component;


    component tx_rdma_hdr_fifo_w1024_d2_S IS
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        if_read_ce : IN STD_LOGIC;
        if_write_ce : IN STD_LOGIC;
        if_din : IN STD_LOGIC_VECTOR (1023 downto 0);
        if_full_n : OUT STD_LOGIC;
        if_write : IN STD_LOGIC;
        if_dout : OUT STD_LOGIC_VECTOR (1023 downto 0);
        if_empty_n : OUT STD_LOGIC;
        if_read : IN STD_LOGIC );
    end component;


    component tx_rdma_hdr_fifo_w2_d2_S IS
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        if_read_ce : IN STD_LOGIC;
        if_write_ce : IN STD_LOGIC;
        if_din : IN STD_LOGIC_VECTOR (1 downto 0);
        if_full_n : OUT STD_LOGIC;
        if_write : IN STD_LOGIC;
        if_dout : OUT STD_LOGIC_VECTOR (1 downto 0);
        if_empty_n : OUT STD_LOGIC;
        if_read : IN STD_LOGIC );
    end component;



begin
    tx_rdma_hdr_entry3_U0 : component tx_rdma_hdr_tx_rdma_hdr_entry3
    port map (
        ap_clk => ap_clk,
        ap_rst => ap_rst_n_inv,
        ap_start => tx_rdma_hdr_entry3_U0_ap_start,
        ap_done => tx_rdma_hdr_entry3_U0_ap_done,
        ap_continue => tx_rdma_hdr_entry3_U0_ap_continue,
        ap_idle => tx_rdma_hdr_entry3_U0_ap_idle,
        ap_ready => tx_rdma_hdr_entry3_U0_ap_ready,
        ecn_for_ack => ecn_for_ack,
        ecn_for_ack_out_din => tx_rdma_hdr_entry3_U0_ecn_for_ack_out_din,
        ecn_for_ack_out_full_n => ecn_for_ack_c1_full_n,
        ecn_for_ack_out_write => tx_rdma_hdr_entry3_U0_ecn_for_ack_out_write);

    lshiftWordByOctet_512_0_4_U0 : component tx_rdma_hdr_lshiftWordByOctet_512_0_4
    port map (
        ap_clk => ap_clk,
        ap_rst => ap_rst_n_inv,
        ap_start => lshiftWordByOctet_512_0_4_U0_ap_start,
        ap_done => lshiftWordByOctet_512_0_4_U0_ap_done,
        ap_continue => lshiftWordByOctet_512_0_4_U0_ap_continue,
        ap_idle => lshiftWordByOctet_512_0_4_U0_ap_idle,
        ap_ready => lshiftWordByOctet_512_0_4_U0_ap_ready,
        ecn_for_ack_dout => ecn_for_ack_c1_dout,
        ecn_for_ack_empty_n => ecn_for_ack_c1_empty_n,
        ecn_for_ack_read => lshiftWordByOctet_512_0_4_U0_ecn_for_ack_read,
        ecn_for_ack_out_din => lshiftWordByOctet_512_0_4_U0_ecn_for_ack_out_din,
        ecn_for_ack_out_full_n => ecn_for_ack_c_full_n,
        ecn_for_ack_out_write => lshiftWordByOctet_512_0_4_U0_ecn_for_ack_out_write,
        tx_in_TVALID => tx_in_TVALID,
        axis_frame_ctx_tx_V_TVALID => axis_frame_ctx_tx_V_TVALID,
        axis_frame_ctx_rx_V_TVALID => axis_frame_ctx_rx_V_TVALID,
        tx_to_hdr_gen_din => lshiftWordByOctet_512_0_4_U0_tx_to_hdr_gen_din,
        tx_to_hdr_gen_full_n => tx_to_hdr_gen_full_n,
        tx_to_hdr_gen_write => lshiftWordByOctet_512_0_4_U0_tx_to_hdr_gen_write,
        axis_frame_ctx_to_hdr_gen_din => lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_to_hdr_gen_din,
        axis_frame_ctx_to_hdr_gen_full_n => axis_frame_ctx_to_hdr_gen_full_n,
        axis_frame_ctx_to_hdr_gen_write => lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_to_hdr_gen_write,
        axis_frame_ctx_tx_V_TDATA => axis_frame_ctx_tx_V_TDATA,
        axis_frame_ctx_tx_V_TREADY => lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_tx_V_TREADY,
        axis_frame_ctx_rx_V_TDATA => axis_frame_ctx_rx_V_TDATA,
        axis_frame_ctx_rx_V_TREADY => lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_rx_V_TREADY,
        offst => lshiftWordByOctet_512_0_4_U0_offst,
        offst_ap_vld => lshiftWordByOctet_512_0_4_U0_offst_ap_vld,
        tx_in_TDATA => tx_in_TDATA,
        tx_in_TREADY => lshiftWordByOctet_512_0_4_U0_tx_in_TREADY,
        tx_in_TKEEP => tx_in_TKEEP,
        tx_in_TSTRB => tx_in_TSTRB,
        tx_in_TLAST => tx_in_TLAST,
        tx_in_TDEST => tx_in_TDEST);

    put_hdr_U0 : component tx_rdma_hdr_put_hdr
    port map (
        ap_clk => ap_clk,
        ap_rst => ap_rst_n_inv,
        ap_start => put_hdr_U0_ap_start,
        ap_done => put_hdr_U0_ap_done,
        ap_continue => put_hdr_U0_ap_continue,
        ap_idle => put_hdr_U0_ap_idle,
        ap_ready => put_hdr_U0_ap_ready,
        ecn_for_ack_dout => ecn_for_ack_c_dout,
        ecn_for_ack_empty_n => ecn_for_ack_c_empty_n,
        ecn_for_ack_read => put_hdr_U0_ecn_for_ack_read,
        tx_to_hdr_gen_dout => tx_to_hdr_gen_dout,
        tx_to_hdr_gen_empty_n => tx_to_hdr_gen_empty_n,
        tx_to_hdr_gen_read => put_hdr_U0_tx_to_hdr_gen_read,
        axis_frame_ctx_to_hdr_gen_dout => axis_frame_ctx_to_hdr_gen_dout,
        axis_frame_ctx_to_hdr_gen_empty_n => axis_frame_ctx_to_hdr_gen_empty_n,
        axis_frame_ctx_to_hdr_gen_read => put_hdr_U0_axis_frame_ctx_to_hdr_gen_read,
        payload_remainder_din => put_hdr_U0_payload_remainder_din,
        payload_remainder_full_n => payload_remainder_full_n,
        payload_remainder_write => put_hdr_U0_payload_remainder_write,
        tx_to_crc_and_remainder_din => put_hdr_U0_tx_to_crc_and_remainder_din,
        tx_to_crc_and_remainder_full_n => tx_to_crc_and_remainder_full_n,
        tx_to_crc_and_remainder_write => put_hdr_U0_tx_to_crc_and_remainder_write,
        packet_len => put_hdr_U0_packet_len,
        packet_len_ap_vld => put_hdr_U0_packet_len_ap_vld,
        rdma_udp_port_dst => rdma_udp_port_dst,
        my_ip => my_ip);

    calc_ip_crc_and_reserve_place_for_icrc_U0 : component tx_rdma_hdr_calc_ip_crc_and_reserve_place_for_icrc
    port map (
        ap_clk => ap_clk,
        ap_rst => ap_rst_n_inv,
        ap_start => calc_ip_crc_and_reserve_place_for_icrc_U0_ap_start,
        ap_done => calc_ip_crc_and_reserve_place_for_icrc_U0_ap_done,
        ap_continue => calc_ip_crc_and_reserve_place_for_icrc_U0_ap_continue,
        ap_idle => calc_ip_crc_and_reserve_place_for_icrc_U0_ap_idle,
        ap_ready => calc_ip_crc_and_reserve_place_for_icrc_U0_ap_ready,
        tx_to_crc_and_remainder_dout => tx_to_crc_and_remainder_dout,
        tx_to_crc_and_remainder_empty_n => tx_to_crc_and_remainder_empty_n,
        tx_to_crc_and_remainder_read => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_to_crc_and_remainder_read,
        payload_remainder_dout => payload_remainder_dout,
        payload_remainder_empty_n => payload_remainder_empty_n,
        payload_remainder_read => calc_ip_crc_and_reserve_place_for_icrc_U0_payload_remainder_read,
        tx_out_TREADY => tx_out_TREADY,
        tx_out_TDATA => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TDATA,
        tx_out_TVALID => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TVALID,
        tx_out_TKEEP => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TKEEP,
        tx_out_TSTRB => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TSTRB,
        tx_out_TLAST => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TLAST,
        tx_out_TDEST => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TDEST);

    ecn_for_ack_c1_U : component tx_rdma_hdr_fifo_w1_d2_S
    port map (
        clk => ap_clk,
        reset => ap_rst_n_inv,
        if_read_ce => ap_const_logic_1,
        if_write_ce => ap_const_logic_1,
        if_din => tx_rdma_hdr_entry3_U0_ecn_for_ack_out_din,
        if_full_n => ecn_for_ack_c1_full_n,
        if_write => tx_rdma_hdr_entry3_U0_ecn_for_ack_out_write,
        if_dout => ecn_for_ack_c1_dout,
        if_empty_n => ecn_for_ack_c1_empty_n,
        if_read => lshiftWordByOctet_512_0_4_U0_ecn_for_ack_read);

    ecn_for_ack_c_U : component tx_rdma_hdr_fifo_w1_d2_S
    port map (
        clk => ap_clk,
        reset => ap_rst_n_inv,
        if_read_ce => ap_const_logic_1,
        if_write_ce => ap_const_logic_1,
        if_din => lshiftWordByOctet_512_0_4_U0_ecn_for_ack_out_din,
        if_full_n => ecn_for_ack_c_full_n,
        if_write => lshiftWordByOctet_512_0_4_U0_ecn_for_ack_out_write,
        if_dout => ecn_for_ack_c_dout,
        if_empty_n => ecn_for_ack_c_empty_n,
        if_read => put_hdr_U0_ecn_for_ack_read);

    axis_frame_ctx_to_hdr_gen_U : component tx_rdma_hdr_fifo_w281_d2_S
    port map (
        clk => ap_clk,
        reset => ap_rst_n_inv,
        if_read_ce => ap_const_logic_1,
        if_write_ce => ap_const_logic_1,
        if_din => lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_to_hdr_gen_din,
        if_full_n => axis_frame_ctx_to_hdr_gen_full_n,
        if_write => lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_to_hdr_gen_write,
        if_dout => axis_frame_ctx_to_hdr_gen_dout,
        if_empty_n => axis_frame_ctx_to_hdr_gen_empty_n,
        if_read => put_hdr_U0_axis_frame_ctx_to_hdr_gen_read);

    tx_to_hdr_gen_U : component tx_rdma_hdr_fifo_w1024_d2_S
    port map (
        clk => ap_clk,
        reset => ap_rst_n_inv,
        if_read_ce => ap_const_logic_1,
        if_write_ce => ap_const_logic_1,
        if_din => lshiftWordByOctet_512_0_4_U0_tx_to_hdr_gen_din,
        if_full_n => tx_to_hdr_gen_full_n,
        if_write => lshiftWordByOctet_512_0_4_U0_tx_to_hdr_gen_write,
        if_dout => tx_to_hdr_gen_dout,
        if_empty_n => tx_to_hdr_gen_empty_n,
        if_read => put_hdr_U0_tx_to_hdr_gen_read);

    payload_remainder_U : component tx_rdma_hdr_fifo_w2_d2_S
    port map (
        clk => ap_clk,
        reset => ap_rst_n_inv,
        if_read_ce => ap_const_logic_1,
        if_write_ce => ap_const_logic_1,
        if_din => put_hdr_U0_payload_remainder_din,
        if_full_n => payload_remainder_full_n,
        if_write => put_hdr_U0_payload_remainder_write,
        if_dout => payload_remainder_dout,
        if_empty_n => payload_remainder_empty_n,
        if_read => calc_ip_crc_and_reserve_place_for_icrc_U0_payload_remainder_read);

    tx_to_crc_and_remainder_U : component tx_rdma_hdr_fifo_w1024_d2_S
    port map (
        clk => ap_clk,
        reset => ap_rst_n_inv,
        if_read_ce => ap_const_logic_1,
        if_write_ce => ap_const_logic_1,
        if_din => put_hdr_U0_tx_to_crc_and_remainder_din,
        if_full_n => tx_to_crc_and_remainder_full_n,
        if_write => put_hdr_U0_tx_to_crc_and_remainder_write,
        if_dout => tx_to_crc_and_remainder_dout,
        if_empty_n => tx_to_crc_and_remainder_empty_n,
        if_read => calc_ip_crc_and_reserve_place_for_icrc_U0_tx_to_crc_and_remainder_read);





    ap_rst_n_inv_assign_proc : process(ap_rst_n)
    begin
                ap_rst_n_inv <= not(ap_rst_n);
    end process;

    ap_sync_continue <= ap_const_logic_0;
    axis_frame_ctx_rx_V_TREADY <= lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_rx_V_TREADY;
    axis_frame_ctx_tx_V_TREADY <= lshiftWordByOctet_512_0_4_U0_axis_frame_ctx_tx_V_TREADY;
    calc_ip_crc_and_reserve_place_for_icrc_U0_ap_continue <= ap_const_logic_1;
    calc_ip_crc_and_reserve_place_for_icrc_U0_ap_start <= ap_const_logic_1;
    lshiftWordByOctet_512_0_4_U0_ap_continue <= ap_const_logic_1;
    lshiftWordByOctet_512_0_4_U0_ap_start <= ap_const_logic_1;
    offst <= lshiftWordByOctet_512_0_4_U0_offst;
    offst_ap_vld <= lshiftWordByOctet_512_0_4_U0_offst_ap_vld;
    packet_len <= put_hdr_U0_packet_len;
    packet_len_ap_vld <= put_hdr_U0_packet_len_ap_vld;
    put_hdr_U0_ap_continue <= ap_const_logic_1;
    put_hdr_U0_ap_start <= ap_const_logic_1;
    tx_in_TREADY <= lshiftWordByOctet_512_0_4_U0_tx_in_TREADY;
    tx_out_TDATA <= calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TDATA;
    tx_out_TDEST <= calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TDEST;
    tx_out_TKEEP <= calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TKEEP;
    tx_out_TLAST <= calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TLAST;
    tx_out_TSTRB <= calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TSTRB;
    tx_out_TVALID <= calc_ip_crc_and_reserve_place_for_icrc_U0_tx_out_TVALID;
    tx_rdma_hdr_entry3_U0_ap_continue <= ap_const_logic_1;
    tx_rdma_hdr_entry3_U0_ap_start <= ap_const_logic_1;
end behav;
