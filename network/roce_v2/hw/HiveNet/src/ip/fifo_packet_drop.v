`timescale 1ns/1ps

module fifo_packet_drop#
(
  parameter                 DATA_BYTES = 64,
  parameter                 MAX_PACKET_SIZE = 9600,
  parameter                 SPACE_FOR_PACKETS = 2
)
(
  input                     s_aclk,
  input                     s_aresetn,
  input  [DATA_BYTES*8-1:0] s_axis_tdata,
  input  [  DATA_BYTES-1:0] s_axis_tkeep,
  input                     s_axis_tuser,
  input                     s_axis_tlast,
  output                    s_axis_tready,
  input                     s_axis_tvalid,
  output [DATA_BYTES*8-1:0] m_axis_tdata,
  output [  DATA_BYTES-1:0] m_axis_tkeep,
  output                    m_axis_tuser,
  output                    m_axis_tlast,
  input                     m_axis_tready,
  output                    m_axis_tvalid,
  output                    axis_prog_full
);

localparam X_FIFO_DEPTH      = 512;
localparam X_FIFO_DWIDTH     = 64;
localparam X_FIFO_PWIDTH     = X_FIFO_DWIDTH / 8;
localparam X_FIFO_WIDTH      = X_FIFO_DWIDTH + X_FIFO_PWIDTH;
localparam FIFO_DEPTH        = ((SPACE_FOR_PACKETS * (MAX_PACKET_SIZE / DATA_BYTES)) % X_FIFO_DEPTH) ?
                                  ((((SPACE_FOR_PACKETS * (MAX_PACKET_SIZE / DATA_BYTES)) / X_FIFO_DEPTH) * X_FIFO_DEPTH) + X_FIFO_DEPTH) :
                                  (SPACE_FOR_PACKETS * (MAX_PACKET_SIZE / DATA_BYTES));

localparam THRESHOLD_TOTAL   = FIFO_DEPTH - (MAX_PACKET_SIZE / DATA_BYTES) - 16;
localparam THRESHOLD_F_VAL   = ((THRESHOLD_TOTAL % X_FIFO_DEPTH) == 0) ? X_FIFO_DEPTH : (THRESHOLD_TOTAL % X_FIFO_DEPTH);
localparam THRESHOLD_F_NUM   = ((THRESHOLD_TOTAL % X_FIFO_DEPTH) == 0) ? (THRESHOLD_TOTAL / X_FIFO_DEPTH - 1) : (THRESHOLD_TOTAL / X_FIFO_DEPTH);
localparam THRESHOLD_TLAST   = THRESHOLD_TOTAL;
localparam FIFO_ROW_NUM      = (FIFO_DEPTH % X_FIFO_DEPTH) ? ((FIFO_DEPTH / X_FIFO_DEPTH) + 1) : (FIFO_DEPTH / X_FIFO_DEPTH);
localparam FIFO_COLUMN_NUM   = ((DATA_BYTES * 9) % X_FIFO_WIDTH) ? (((DATA_BYTES * 9) / X_FIFO_WIDTH) + 1) : ((DATA_BYTES * 9) / X_FIFO_WIDTH);

wire [X_FIFO_DWIDTH-1:0] CASDOUT[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_PWIDTH-1:0] CASDOUTP[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASNXTEMPTY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASPRVRDEN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     DBITERR[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_DWIDTH-1:0] DOUT[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_PWIDTH-1:0] DOUTP[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_PWIDTH-1:0] ECCPARITY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     EMPTY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     FULL[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     PROGEMPTY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     PROGFULL[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [             13:0] RDCOUNT[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     RDERR[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     RDRSTBUSY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     SBITERR[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [             13:0] WRCOUNT[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     WRERR[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     WRRSTBUSY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];

wire [X_FIFO_DWIDTH-1:0] CASDIN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_PWIDTH-1:0] CASDINP[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASDOMUX[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASDOMUXEN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASNXTRDEN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASOREGIMUX[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASOREGIMUXEN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     CASPRVEMPTY[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_DWIDTH-1:0] DIN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire [X_FIFO_PWIDTH-1:0] DINP[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     INJECTDBITERR[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     INJECTSBITERR[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     RDCLK[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     RDEN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     REGCE[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     RST[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     RSTREG[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     SLEEP[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     WRCLK[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];
wire                     WREN[FIFO_ROW_NUM-1:0][FIFO_COLUMN_NUM-1:0];

wire [           32-1:0] TLAST_CASDOUT;
wire [            4-1:0] TLAST_CASDOUTP;
wire                     TLAST_CASNXTEMPTY;
wire                     TLAST_CASPRVRDEN;
wire                     TLAST_DBITERR;
wire [           32-1:0] TLAST_DOUT;
wire [            4-1:0] TLAST_DOUTP;
wire [            4-1:0] TLAST_ECCPARITY;
wire                     TLAST_EMPTY;
wire                     TLAST_FULL;
wire                     TLAST_PROGEMPTY;
wire                     TLAST_PROGFULL;
wire [             12:0] TLAST_RDCOUNT;
wire                     TLAST_RDERR;
wire                     TLAST_RDRSTBUSY;
wire                     TLAST_SBITERR;
wire [             12:0] TLAST_WRCOUNT;
wire                     TLAST_WRERR;
wire                     TLAST_WRRSTBUSY;

wire [           32-1:0] TLAST_CASDIN;
wire [            4-1:0] TLAST_CASDINP;
wire                     TLAST_CASDOMUX;
wire                     TLAST_CASDOMUXEN;
wire                     TLAST_CASNXTRDEN;
wire                     TLAST_CASOREGIMUX;
wire                     TLAST_CASOREGIMUXEN;
wire                     TLAST_CASPRVEMPTY;
wire [           32-1:0] TLAST_DIN;
wire [            4-1:0] TLAST_DINP;
wire                     TLAST_INJECTDBITERR;
wire                     TLAST_INJECTSBITERR;
wire                     TLAST_RDCLK;
wire                     TLAST_RDEN;
wire                     TLAST_REGCE;
wire                     TLAST_RST;
wire                     TLAST_RSTREG;
wire                     TLAST_SLEEP;
wire                     TLAST_WRCLK;
wire                     TLAST_WREN;

wire [FIFO_COLUMN_NUM-1:0] empty_w;
wire                       empty_g_w;
wire [FIFO_COLUMN_NUM-1:0] full_w;
wire                       full_g_w;
wire [FIFO_COLUMN_NUM-1:0] pfull_w;
wire                       pfull_g_w;

assign m_axis_tvalid  = (~empty_g_w);
assign m_axis_tlast   = TLAST_DOUT[0];
assign m_axis_tuser   = TLAST_DOUT[1];

assign s_axis_tready  = (~full_g_w);

assign axis_prog_full = pfull_g_w;

assign empty_g_w = (|empty_w) | TLAST_EMPTY;
assign full_g_w  = (|full_w ) | TLAST_FULL;
assign pfull_g_w = (|pfull_w) | TLAST_PROGFULL;

generate genvar i3;
  for(i3 = 0 ; i3 < FIFO_COLUMN_NUM ; i3 = i3 + 1) begin
    assign empty_w[i3] = EMPTY[FIFO_ROW_NUM - 1][i3];
    assign full_w[i3]  = FULL[0][i3];
    assign pfull_w[i3] = PROGFULL[FIFO_ROW_NUM - THRESHOLD_F_NUM - 1][i3];
    assign m_axis_tdata[i3*X_FIFO_DWIDTH+:X_FIFO_DWIDTH] = DOUT[FIFO_ROW_NUM - 1][i3];
    assign m_axis_tkeep[i3*X_FIFO_PWIDTH+:X_FIFO_PWIDTH] = DOUTP[FIFO_ROW_NUM - 1][i3];
  end
endgenerate

generate genvar i0,i1;
  for(i0 = 0 ; i0 < FIFO_ROW_NUM ; i0 = i0 + 1) begin
    for(i1 = 0 ; i1 < FIFO_COLUMN_NUM ; i1 = i1 + 1) begin

      assign CASDIN[i0][i1]        = (i0 == 0) ? 'b0 : CASDOUT[i0 - 1][i1];
      assign CASDINP[i0][i1]       = (i0 == 0) ? 'b0 : CASDOUTP[i0 - 1][i1];
      assign CASDOMUX[i0][i1]      = 1'b0;
      assign CASDOMUXEN[i0][i1]    = 1'b0;
      assign CASNXTRDEN[i0][i1]    = (i0 == FIFO_ROW_NUM - 1) ? 'b0 : CASPRVRDEN[i0 + 1][i1];
      assign CASOREGIMUX[i0][i1]   = 1'b0;
      assign CASOREGIMUXEN[i0][i1] = 1'b0;
      assign CASPRVEMPTY[i0][i1]   = (i0 == 0) ? 'b0 : CASNXTEMPTY[i0 - 1][i1];
      assign DIN[i0][i1]           = (i0 == 0) ? s_axis_tdata[i1*X_FIFO_DWIDTH+:X_FIFO_DWIDTH] : 'b0;
      assign DINP[i0][i1]          = (i0 == 0) ? s_axis_tkeep[i1*X_FIFO_PWIDTH+:X_FIFO_PWIDTH] : 'b0;
      assign INJECTDBITERR[i0][i1] = 1'b0;
      assign INJECTSBITERR[i0][i1] = 1'b0;
      assign RDCLK[i0][i1]         = s_aclk;
      assign RDEN[i0][i1]          = (i0 == FIFO_ROW_NUM - 1) ? m_axis_tready & (~empty_g_w) : 1'b0;
      assign REGCE[i0][i1]         = 1'b0;
      assign RST[i0][i1]           = s_aresetn;
      assign RSTREG[i0][i1]        = 1'b0;
      assign SLEEP[i0][i1]         = 1'b0;
      assign WRCLK[i0][i1]         = s_aclk;
      assign WREN[i0][i1]          = (i0 == 0) ? s_axis_tvalid & (~full_g_w) : 1'b0;

      FIFO36E2 #(
        .CASCADE_ORDER(((FIFO_ROW_NUM == 1) ? "NONE" : ((i0 == 0) ? "FIRST" : ((i0 == (FIFO_ROW_NUM - 1)) ? "LAST" : "MIDDLE")))),            // FIRST, LAST, MIDDLE, NONE, PARALLEL
        .CLOCK_DOMAINS("COMMON"),          // COMMON, INDEPENDENT
        .EN_ECC_PIPE("FALSE"),             // ECC pipeline register, (FALSE, TRUE)
        .EN_ECC_READ("FALSE"),             // Enable ECC decoder, (FALSE, TRUE)
        .EN_ECC_WRITE("FALSE"),            // Enable ECC encoder, (FALSE, TRUE)
        .FIRST_WORD_FALL_THROUGH("TRUE"),  // FALSE, TRUE
        .INIT(72'h000000000000000000),     // Initial values on output port

        .PROG_EMPTY_THRESH(1),             // Programmable Empty Threshold
        .PROG_FULL_THRESH(THRESHOLD_F_VAL),      // Programmable Full Threshold
        // Programmable Inversion Attributes: Specifies the use of the built-in programmable inversion
        .IS_RDCLK_INVERTED(1'b0),          // Optional inversion for RDCLK
        .IS_RDEN_INVERTED(1'b0),           // Optional inversion for RDEN
        .IS_RSTREG_INVERTED(1'b1),         // Optional inversion for RSTREG
        .IS_RST_INVERTED(1'b1),            // Optional inversion for RST
        .IS_WRCLK_INVERTED(1'b0),          // Optional inversion for WRCLK
        .IS_WREN_INVERTED(1'b0),           // Optional inversion for WREN
        .RDCOUNT_TYPE("RAW_PNTR"),         // EXTENDED_DATACOUNT, RAW_PNTR, SIMPLE_DATACOUNT, SYNC_PNTR
        .READ_WIDTH(X_FIFO_WIDTH),         // 18-9
        .REGISTER_MODE("UNREGISTERED"),    // DO_PIPELINED, REGISTERED, UNREGISTERED
        .RSTREG_PRIORITY("RSTREG"),        // REGCE, RSTREG
        .SLEEP_ASYNC("FALSE"),             // FALSE, TRUE
        .SRVAL(72'h000000000000000000),    // SET/reset value of the FIFO outputs
        .WRCOUNT_TYPE("RAW_PNTR"),         // EXTENDED_DATACOUNT, RAW_PNTR, SIMPLE_DATACOUNT, SYNC_PNTR
        .WRITE_WIDTH(X_FIFO_WIDTH)         // 18-9
      )
      FIFO36E2_inst (
        // Cascade Signals outputs: Multi-FIFO cascade signals
        .CASDOUT(CASDOUT[i0][i1]),             // 64-bit output: Data cascade output bus
        .CASDOUTP(CASDOUTP[i0][i1]),           // 8-bit output: Parity data cascade output bus
        .CASNXTEMPTY(CASNXTEMPTY[i0][i1]),     // 1-bit output: Cascade next empty
        .CASPRVRDEN(CASPRVRDEN[i0][i1]),       // 1-bit output: Cascade previous read enable
        // ECC Signals outputs: Error Correction Circuitry ports
        .DBITERR(DBITERR[i0][i1]),             // 1-bit output: Double bit error status
        .ECCPARITY(ECCPARITY[i0][i1]),         // 8-bit output: Generated error correction parity
        .SBITERR(SBITERR[i0][i1]),             // 1-bit output: Single bit error status
        // Read Data outputs: Read output data
        .DOUT(DOUT[i0][i1]),                   // 64-bit output: FIFO data output bus
        .DOUTP(DOUTP[i0][i1]),                 // 8-bit output: FIFO parity output bus.
        // Status outputs: Flags and other FIFO status outputs
        .EMPTY(EMPTY[i0][i1]),                 // 1-bit output: Empty
        .FULL(FULL[i0][i1]),                   // 1-bit output: Full
        .PROGEMPTY(PROGEMPTY[i0][i1]),         // 1-bit output: Programmable empty
        .PROGFULL(PROGFULL[i0][i1]),           // 1-bit output: Programmable full
        .RDCOUNT(RDCOUNT[i0][i1]),             // 14-bit output: Read count
        .RDERR(RDERR[i0][i1]),                 // 1-bit output: Read error
        .RDRSTBUSY(RDRSTBUSY[i0][i1]),         // 1-bit output: Reset busy (sync to RDCLK)
        .WRCOUNT(WRCOUNT[i0][i1]),             // 14-bit output: Write count
        .WRERR(WRERR[i0][i1]),                 // 1-bit output: Write Error
        .WRRSTBUSY(WRRSTBUSY[i0][i1]),         // 1-bit output: Reset busy (sync to WRCLK)
        // Cascade Signals inputs: Multi-FIFO cascade signals
        .CASDIN(CASDIN[i0][i1]),               // 64-bit input: Data cascade input bus
        .CASDINP(CASDINP[i0][i1]),             // 8-bit input: Parity data cascade input bus
        .CASDOMUX(CASDOMUX[i0][i1]),           // 1-bit input: Cascade MUX select input
        .CASDOMUXEN(CASDOMUXEN[i0][i1]),       // 1-bit input: Enable for cascade MUX select
        .CASNXTRDEN(CASNXTRDEN[i0][i1]),       // 1-bit input: Cascade next read enable
        .CASOREGIMUX(CASOREGIMUX[i0][i1]),     // 1-bit input: Cascade output MUX select
        .CASOREGIMUXEN(CASOREGIMUXEN[i0][i1]), // 1-bit input: Cascade output MUX select enable
        .CASPRVEMPTY(CASPRVEMPTY[i0][i1]),     // 1-bit input: Cascade previous empty
        // ECC Signals inputs: Error Correction Circuitry ports
        .INJECTDBITERR(INJECTDBITERR[i0][i1]), // 1-bit input: Inject a double-bit error
        .INJECTSBITERR(INJECTSBITERR[i0][i1]), // 1-bit input: Inject a single bit error
        // Read Control Signals inputs: Read clock, enable and reset input signals
        .RDCLK(RDCLK[i0][i1]),                 // 1-bit input: Read clock
        .RDEN(RDEN[i0][i1]),                   // 1-bit input: Read enable
        .REGCE(REGCE[i0][i1]),                 // 1-bit input: Output register clock enable
        .RSTREG(RSTREG[i0][i1]),               // 1-bit input: Output register reset
        .SLEEP(SLEEP[i0][i1]),                 // 1-bit input: Sleep Mode
        // Write Control Signals inputs: Write clock and enable input signals
        .RST(RST[i0][i1]),                     // 1-bit input: Reset
        .WRCLK(WRCLK[i0][i1]),                 // 1-bit input: Write clock
        .WREN(WREN[i0][i1]),                   // 1-bit input: Write enable
        // Write Data inputs: Write input data
        .DIN(DIN[i0][i1]),                     // 64-bit input: FIFO data input bus
        .DINP(DINP[i0][i1])                    // 8-bit input: FIFO parity input bus
      );
    end
  end
endgenerate

assign TLAST_CASDIN        = 'b0;
assign TLAST_CASDINP       = 'b0;
assign TLAST_CASDOMUX      = 1'b0;
assign TLAST_CASDOMUXEN    = 1'b0;
assign TLAST_CASNXTRDEN    = 'b0;
assign TLAST_CASOREGIMUX   = 1'b0;
assign TLAST_CASOREGIMUXEN = 1'b0;
assign TLAST_CASPRVEMPTY   = 'b0;
assign TLAST_DIN           = {30'h0, s_axis_tuser, s_axis_tlast};
assign TLAST_DINP          = 'b0;
assign TLAST_INJECTDBITERR = 1'b0;
assign TLAST_INJECTSBITERR = 1'b0;
assign TLAST_RDCLK         = s_aclk;
assign TLAST_RDEN          = m_axis_tready & (~empty_g_w);
assign TLAST_REGCE         = 1'b0;
assign TLAST_RST           = s_aresetn;
assign TLAST_RSTREG        = 1'b0;
assign TLAST_SLEEP         = 1'b0;
assign TLAST_WRCLK         = s_aclk;
assign TLAST_WREN          = s_axis_tvalid & (~full_g_w);

FIFO18E2 #(
  .CASCADE_ORDER("NONE"),            // FIRST, LAST, MIDDLE, NONE, PARALLEL
  .CLOCK_DOMAINS("COMMON"),          // COMMON, INDEPENDENT
  .FIRST_WORD_FALL_THROUGH("TRUE"),  // FALSE, TRUE
  .INIT(36'h000000000),     // Initial values on output port

  .PROG_EMPTY_THRESH(1),             // Programmable Empty Threshold
  .PROG_FULL_THRESH(THRESHOLD_TLAST),// Programmable Full Threshold
  // Programmable Inversion Attributes: Specifies the use of the built-in programmable inversion
  .IS_RDCLK_INVERTED(1'b0),          // Optional inversion for RDCLK
  .IS_RDEN_INVERTED(1'b0),           // Optional inversion for RDEN
  .IS_RSTREG_INVERTED(1'b1),         // Optional inversion for RSTREG
  .IS_RST_INVERTED(1'b1),            // Optional inversion for RST
  .IS_WRCLK_INVERTED(1'b0),          // Optional inversion for WRCLK
  .IS_WREN_INVERTED(1'b0),           // Optional inversion for WREN
  .RDCOUNT_TYPE("RAW_PNTR"),         // EXTENDED_DATACOUNT, RAW_PNTR, SIMPLE_DATACOUNT, SYNC_PNTR
  .READ_WIDTH(4),         // 18-9
  .REGISTER_MODE("UNREGISTERED"),    // DO_PIPELINED, REGISTERED, UNREGISTERED
  .RSTREG_PRIORITY("RSTREG"),        // REGCE, RSTREG
  .SLEEP_ASYNC("FALSE"),             // FALSE, TRUE
  .SRVAL(72'h000000000000000000),    // SET/reset value of the FIFO outputs
  .WRCOUNT_TYPE("RAW_PNTR"),         // EXTENDED_DATACOUNT, RAW_PNTR, SIMPLE_DATACOUNT, SYNC_PNTR
  .WRITE_WIDTH(4)         // 18-9
)
FIFO18E2_tlast_inst (
  // Cascade Signals outputs: Multi-FIFO cascade signals
  .CASDOUT(TLAST_CASDOUT),             // 64-bit output: Data cascade output bus
  .CASDOUTP(TLAST_CASDOUTP),           // 8-bit output: Parity data cascade output bus
  .CASNXTEMPTY(TLAST_CASNXTEMPTY),     // 1-bit output: Cascade next empty
  .CASPRVRDEN(TLAST_CASPRVRDEN),       // 1-bit output: Cascade previous read enable
  // Read Data outputs: Read output data
  .DOUT(TLAST_DOUT),                   // 64-bit output: FIFO data output bus
  .DOUTP(TLAST_DOUTP),                 // 8-bit output: FIFO parity output bus.
  // Status outputs: Flags and other FIFO status outputs
  .EMPTY(TLAST_EMPTY),                 // 1-bit output: Empty
  .FULL(TLAST_FULL),                   // 1-bit output: Full
  .PROGEMPTY(TLAST_PROGEMPTY),         // 1-bit output: Programmable empty
  .PROGFULL(TLAST_PROGFULL),           // 1-bit output: Programmable full
  .RDCOUNT(TLAST_RDCOUNT),             // 14-bit output: Read count
  .RDERR(TLAST_RDERR),                 // 1-bit output: Read error
  .RDRSTBUSY(TLAST_RDRSTBUSY),         // 1-bit output: Reset busy (TLAST_sync to RDCLK)
  .WRCOUNT(TLAST_WRCOUNT),             // 14-bit output: Write count
  .WRERR(TLAST_WRERR),                 // 1-bit output: Write Error
  .WRRSTBUSY(TLAST_WRRSTBUSY),         // 1-bit output: Reset busy (TLAST_sync to WRCLK)
  // Cascade Signals inputs: Multi-FIFO cascade signals
  .CASDIN(TLAST_CASDIN),               // 64-bit input: Data cascade input bus
  .CASDINP(TLAST_CASDINP),             // 8-bit input: Parity data cascade input bus
  .CASDOMUX(TLAST_CASDOMUX),           // 1-bit input: Cascade MUX select input
  .CASDOMUXEN(TLAST_CASDOMUXEN),       // 1-bit input: Enable for cascade MUX select
  .CASNXTRDEN(TLAST_CASNXTRDEN),       // 1-bit input: Cascade next read enable
  .CASOREGIMUX(TLAST_CASOREGIMUX),     // 1-bit input: Cascade output MUX select
  .CASOREGIMUXEN(TLAST_CASOREGIMUXEN), // 1-bit input: Cascade output MUX select enable
  .CASPRVEMPTY(TLAST_CASPRVEMPTY),     // 1-bit input: Cascade previous empty
  // Read Control Signals inputs: Read clock, enable and reset input signals
  .RDCLK(TLAST_RDCLK),                 // 1-bit input: Read clock
  .RDEN(TLAST_RDEN),                   // 1-bit input: Read enable
  .REGCE(TLAST_REGCE),                 // 1-bit input: Output register clock enable
  .RSTREG(TLAST_RSTREG),               // 1-bit input: Output register reset
  .SLEEP(TLAST_SLEEP),                 // 1-bit input: Sleep Mode
  // Write Control Signals inputs: Write clock and enable input signals
  .RST(TLAST_RST),                     // 1-bit input: Reset
  .WRCLK(TLAST_WRCLK),                 // 1-bit input: Write clock
  .WREN(TLAST_WREN),                   // 1-bit input: Write enable
  // Write Data inputs: Write input data
  .DIN(TLAST_DIN),                     // 64-bit input: FIFO data input bus
  .DINP(TLAST_DINP)                    // 8-bit input: FIFO parity input bus
);

endmodule
