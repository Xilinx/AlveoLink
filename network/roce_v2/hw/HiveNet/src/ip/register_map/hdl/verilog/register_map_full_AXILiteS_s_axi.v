// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1ns/1ps
module register_map_full_AXILiteS_s_axi
#(parameter
    C_S_AXI_ADDR_WIDTH = 7,
    C_S_AXI_DATA_WIDTH = 32
)(
    input  wire                          ACLK,
    input  wire                          ARESET,
    input  wire                          ACLK_EN,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] AWADDR,
    input  wire                          AWVALID,
    output wire                          AWREADY,
    input  wire [C_S_AXI_DATA_WIDTH-1:0] WDATA,
    input  wire [C_S_AXI_DATA_WIDTH/8-1:0] WSTRB,
    input  wire                          WVALID,
    output wire                          WREADY,
    output wire [1:0]                    BRESP,
    output wire                          BVALID,
    input  wire                          BREADY,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0] ARADDR,
    input  wire                          ARVALID,
    output wire                          ARREADY,
    output wire [C_S_AXI_DATA_WIDTH-1:0] RDATA,
    output wire [1:0]                    RRESP,
    output wire                          RVALID,
    input  wire                          RREADY,
    output wire [12:0]                   local_ID_V,
    output wire [63:0]                   timeoutInNs_V,
    output wire [31:0]                   ip_subnet_V,
    output wire [47:0]                   myMac_V,
    output wire [15:0]                   nRetransmissions_V,
    output wire [15:0]                   udp_port_V,
    output wire [0:0]                    arp_clean_n_V,
    input  wire [23:0]                   readPSN_V,
    input  wire [23:0]                   writePSN_V
);
//------------------------Address Info-------------------
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of local_ID_V
//        bit 12~0 - local_ID_V[12:0] (Read/Write)
//        others   - reserved
// 0x14 : reserved
// 0x18 : Data signal of timeoutInNs_V
//        bit 31~0 - timeoutInNs_V[31:0] (Read/Write)
// 0x1c : Data signal of timeoutInNs_V
//        bit 31~0 - timeoutInNs_V[63:32] (Read/Write)
// 0x20 : reserved
// 0x24 : Data signal of ip_subnet_V
//        bit 31~0 - ip_subnet_V[31:0] (Read/Write)
// 0x28 : reserved
// 0x2c : Data signal of myMac_V
//        bit 31~0 - myMac_V[31:0] (Read/Write)
// 0x30 : Data signal of myMac_V
//        bit 15~0 - myMac_V[47:32] (Read/Write)
//        others   - reserved
// 0x34 : reserved
// 0x38 : Data signal of nRetransmissions_V
//        bit 15~0 - nRetransmissions_V[15:0] (Read/Write)
//        others   - reserved
// 0x3c : reserved
// 0x40 : Data signal of udp_port_V
//        bit 15~0 - udp_port_V[15:0] (Read/Write)
//        others   - reserved
// 0x44 : reserved
// 0x48 : Data signal of arp_clean_n_V
//        bit 0  - arp_clean_n_V[0] (Read/Write)
//        others - reserved
// 0x4c : reserved
// 0x50 : Data signal of readPSN_V
//        bit 23~0 - readPSN_V[23:0] (Read)
//        others   - reserved
// 0x54 : reserved
// 0x58 : Data signal of writePSN_V
//        bit 23~0 - writePSN_V[23:0] (Read)
//        others   - reserved
// 0x5c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

//------------------------Parameter----------------------
localparam
    ADDR_LOCAL_ID_V_DATA_0         = 7'h10,
    ADDR_LOCAL_ID_V_CTRL           = 7'h14,
    ADDR_TIMEOUTINNS_V_DATA_0      = 7'h18,
    ADDR_TIMEOUTINNS_V_DATA_1      = 7'h1c,
    ADDR_TIMEOUTINNS_V_CTRL        = 7'h20,
    ADDR_IP_SUBNET_V_DATA_0        = 7'h24,
    ADDR_IP_SUBNET_V_CTRL          = 7'h28,
    ADDR_MYMAC_V_DATA_0            = 7'h2c,
    ADDR_MYMAC_V_DATA_1            = 7'h30,
    ADDR_MYMAC_V_CTRL              = 7'h34,
    ADDR_NRETRANSMISSIONS_V_DATA_0 = 7'h38,
    ADDR_NRETRANSMISSIONS_V_CTRL   = 7'h3c,
    ADDR_UDP_PORT_V_DATA_0         = 7'h40,
    ADDR_UDP_PORT_V_CTRL           = 7'h44,
    ADDR_ARP_CLEAN_N_V_DATA_0      = 7'h48,
    ADDR_ARP_CLEAN_N_V_CTRL        = 7'h4c,
    ADDR_READPSN_V_DATA_0          = 7'h50,
    ADDR_READPSN_V_CTRL            = 7'h54,
    ADDR_WRITEPSN_V_DATA_0         = 7'h58,
    ADDR_WRITEPSN_V_CTRL           = 7'h5c,
    WRIDLE                         = 2'd0,
    WRDATA                         = 2'd1,
    WRRESP                         = 2'd2,
    WRRESET                        = 2'd3,
    RDIDLE                         = 2'd0,
    RDDATA                         = 2'd1,
    RDRESET                        = 2'd2,
    ADDR_BITS         = 7;

//------------------------Local signal-------------------
    reg  [1:0]                    wstate = WRRESET;
    reg  [1:0]                    wnext;
    reg  [ADDR_BITS-1:0]          waddr;
    wire [31:0]                   wmask;
    wire                          aw_hs;
    wire                          w_hs;
    reg  [1:0]                    rstate = RDRESET;
    reg  [1:0]                    rnext;
    reg  [31:0]                   rdata;
    wire                          ar_hs;
    wire [ADDR_BITS-1:0]          raddr;
    // internal registers
    reg  [12:0]                   int_local_ID_V = 'b0;
    reg  [63:0]                   int_timeoutInNs_V = 'b0;
    reg  [31:0]                   int_ip_subnet_V = 'b0;
    reg  [47:0]                   int_myMac_V = 'b0;
    reg  [15:0]                   int_nRetransmissions_V = 'b0;
    reg  [15:0]                   int_udp_port_V = 'b0;
    reg  [0:0]                    int_arp_clean_n_V = 'b0;
    reg  [23:0]                   int_readPSN_V = 'b0;
    reg  [23:0]                   int_writePSN_V = 'b0;

//------------------------Instantiation------------------

//------------------------AXI write fsm------------------
assign AWREADY = (wstate == WRIDLE);
assign WREADY  = (wstate == WRDATA);
assign BRESP   = 2'b00;  // OKAY
assign BVALID  = (wstate == WRRESP);
assign wmask   = { {8{WSTRB[3]}}, {8{WSTRB[2]}}, {8{WSTRB[1]}}, {8{WSTRB[0]}} };
assign aw_hs   = AWVALID & AWREADY;
assign w_hs    = WVALID & WREADY;

// wstate
always @(posedge ACLK) begin
    if (ARESET)
        wstate <= WRRESET;
    else if (ACLK_EN)
        wstate <= wnext;
end

// wnext
always @(*) begin
    case (wstate)
        WRIDLE:
            if (AWVALID)
                wnext = WRDATA;
            else
                wnext = WRIDLE;
        WRDATA:
            if (WVALID)
                wnext = WRRESP;
            else
                wnext = WRDATA;
        WRRESP:
            if (BREADY)
                wnext = WRIDLE;
            else
                wnext = WRRESP;
        default:
            wnext = WRIDLE;
    endcase
end

// waddr
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (aw_hs)
            waddr <= AWADDR[ADDR_BITS-1:0];
    end
end

//------------------------AXI read fsm-------------------
assign ARREADY = (rstate == RDIDLE);
assign RDATA   = rdata;
assign RRESP   = 2'b00;  // OKAY
assign RVALID  = (rstate == RDDATA);
assign ar_hs   = ARVALID & ARREADY;
assign raddr   = ARADDR[ADDR_BITS-1:0];

// rstate
always @(posedge ACLK) begin
    if (ARESET)
        rstate <= RDRESET;
    else if (ACLK_EN)
        rstate <= rnext;
end

// rnext
always @(*) begin
    case (rstate)
        RDIDLE:
            if (ARVALID)
                rnext = RDDATA;
            else
                rnext = RDIDLE;
        RDDATA:
            if (RREADY & RVALID)
                rnext = RDIDLE;
            else
                rnext = RDDATA;
        default:
            rnext = RDIDLE;
    endcase
end

// rdata
always @(posedge ACLK) begin
    if (ACLK_EN) begin
        if (ar_hs) begin
            rdata <= 1'b0;
            case (raddr)
                ADDR_LOCAL_ID_V_DATA_0: begin
                    rdata <= int_local_ID_V[12:0];
                end
                ADDR_TIMEOUTINNS_V_DATA_0: begin
                    rdata <= int_timeoutInNs_V[31:0];
                end
                ADDR_TIMEOUTINNS_V_DATA_1: begin
                    rdata <= int_timeoutInNs_V[63:32];
                end
                ADDR_IP_SUBNET_V_DATA_0: begin
                    rdata <= int_ip_subnet_V[31:0];
                end
                ADDR_MYMAC_V_DATA_0: begin
                    rdata <= int_myMac_V[31:0];
                end
                ADDR_MYMAC_V_DATA_1: begin
                    rdata <= int_myMac_V[47:32];
                end
                ADDR_NRETRANSMISSIONS_V_DATA_0: begin
                    rdata <= int_nRetransmissions_V[15:0];
                end
                ADDR_UDP_PORT_V_DATA_0: begin
                    rdata <= int_udp_port_V[15:0];
                end
                ADDR_ARP_CLEAN_N_V_DATA_0: begin
                    rdata <= int_arp_clean_n_V[0:0];
                end
                ADDR_READPSN_V_DATA_0: begin
                    rdata <= int_readPSN_V[23:0];
                end
                ADDR_WRITEPSN_V_DATA_0: begin
                    rdata <= int_writePSN_V[23:0];
                end
            endcase
        end
    end
end


//------------------------Register logic-----------------
assign local_ID_V         = int_local_ID_V;
assign timeoutInNs_V      = int_timeoutInNs_V;
assign ip_subnet_V        = int_ip_subnet_V;
assign myMac_V            = int_myMac_V;
assign nRetransmissions_V = int_nRetransmissions_V;
assign udp_port_V         = int_udp_port_V;
assign arp_clean_n_V      = int_arp_clean_n_V;
// int_local_ID_V[12:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_local_ID_V[12:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_LOCAL_ID_V_DATA_0)
            int_local_ID_V[12:0] <= (WDATA[31:0] & wmask) | (int_local_ID_V[12:0] & ~wmask);
    end
end

// int_timeoutInNs_V[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_timeoutInNs_V[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_TIMEOUTINNS_V_DATA_0)
            int_timeoutInNs_V[31:0] <= (WDATA[31:0] & wmask) | (int_timeoutInNs_V[31:0] & ~wmask);
    end
end

// int_timeoutInNs_V[63:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_timeoutInNs_V[63:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_TIMEOUTINNS_V_DATA_1)
            int_timeoutInNs_V[63:32] <= (WDATA[31:0] & wmask) | (int_timeoutInNs_V[63:32] & ~wmask);
    end
end

// int_ip_subnet_V[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_ip_subnet_V[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_IP_SUBNET_V_DATA_0)
            int_ip_subnet_V[31:0] <= (WDATA[31:0] & wmask) | (int_ip_subnet_V[31:0] & ~wmask);
    end
end

// int_myMac_V[31:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_myMac_V[31:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_MYMAC_V_DATA_0)
            int_myMac_V[31:0] <= (WDATA[31:0] & wmask) | (int_myMac_V[31:0] & ~wmask);
    end
end

// int_myMac_V[47:32]
always @(posedge ACLK) begin
    if (ARESET)
        int_myMac_V[47:32] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_MYMAC_V_DATA_1)
            int_myMac_V[47:32] <= (WDATA[31:0] & wmask) | (int_myMac_V[47:32] & ~wmask);
    end
end

// int_nRetransmissions_V[15:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_nRetransmissions_V[15:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_NRETRANSMISSIONS_V_DATA_0)
            int_nRetransmissions_V[15:0] <= (WDATA[31:0] & wmask) | (int_nRetransmissions_V[15:0] & ~wmask);
    end
end

// int_udp_port_V[15:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_udp_port_V[15:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_UDP_PORT_V_DATA_0)
            int_udp_port_V[15:0] <= (WDATA[31:0] & wmask) | (int_udp_port_V[15:0] & ~wmask);
    end
end

// int_arp_clean_n_V[0:0]
always @(posedge ACLK) begin
    if (ARESET)
        int_arp_clean_n_V[0:0] <= 0;
    else if (ACLK_EN) begin
        if (w_hs && waddr == ADDR_ARP_CLEAN_N_V_DATA_0)
            int_arp_clean_n_V[0:0] <= (WDATA[31:0] & wmask) | (int_arp_clean_n_V[0:0] & ~wmask);
    end
end

// int_readPSN_V
always @(posedge ACLK) begin
    if (ARESET)
        int_readPSN_V <= 0;
    else if (ACLK_EN) begin
            int_readPSN_V <= readPSN_V;
    end
end

// int_writePSN_V
always @(posedge ACLK) begin
    if (ARESET)
        int_writePSN_V <= 0;
    else if (ACLK_EN) begin
            int_writePSN_V <= writePSN_V;
    end
end


//------------------------Memory logic-------------------

endmodule
