`timescale 1ns/1ps

module axis_switch #
(
  parameter DATA_WIDTH = 256,
  parameter DEST_WIDTH = 32
)
(
  input                      aclk,
  input                      aresetn,

  /*
   * AXIS Header input
   */
  input  [DATA_WIDTH  -1:0]  s_axis_header_tdata,
  input  [DATA_WIDTH/8-1:0]  s_axis_header_tkeep,
  input  [DEST_WIDTH  -1:0]  s_axis_header_tdest,
  input                      s_axis_header_tvalid,
  output                     s_axis_header_tready,

  /*
   * AXIS Data_0 input
   */
  input  [DATA_WIDTH  -1:0]  s_axis_data_0_tdata,
  input  [DATA_WIDTH/8-1:0]  s_axis_data_0_tkeep,
  input  [DEST_WIDTH  -1:0]  s_axis_data_0_tdest,
  input                      s_axis_data_0_tvalid,
  output                     s_axis_data_0_tready,

  /*
   * AXIS Data_1 input
   */
  input  [DATA_WIDTH  -1:0]  s_axis_data_1_tdata,
  input  [DATA_WIDTH/8-1:0]  s_axis_data_1_tkeep,
  input  [DEST_WIDTH  -1:0]  s_axis_data_1_tdest,
  input                      s_axis_data_1_tlast,
  input                      s_axis_data_1_tvalid,
  output                     s_axis_data_1_tready,

  /*
   * AXIS output
   */
  output [DATA_WIDTH  -1:0]  m_axis_out_tdata,
  output [DATA_WIDTH/8-1:0]  m_axis_out_tkeep,
  output [DEST_WIDTH  -1:0]  m_axis_out_tdest,
  output                     m_axis_out_tvalid,
  input                      m_axis_out_tready
);

wire [DATA_WIDTH  -1:0]  s_axis_header_tdata_int;
wire [DATA_WIDTH/8-1:0]  s_axis_header_tkeep_int;
wire [DEST_WIDTH  -1:0]  s_axis_header_tdest_int;
wire                     s_axis_header_tvalid_int;
wire                     s_axis_header_tready_int;

wire [DATA_WIDTH  -1:0]  s_axis_data_0_tdata_int;
wire [DATA_WIDTH/8-1:0]  s_axis_data_0_tkeep_int;
wire [DEST_WIDTH  -1:0]  s_axis_data_0_tdest_int;
wire                     s_axis_data_0_tvalid_int;
wire                     s_axis_data_0_tready_int;

wire [DATA_WIDTH  -1:0]  s_axis_data_1_tdata_int;
wire [DATA_WIDTH/8-1:0]  s_axis_data_1_tkeep_int;
wire [DEST_WIDTH  -1:0]  s_axis_data_1_tdest_int;
wire                     s_axis_data_1_tlast_int;
wire                     s_axis_data_1_tvalid_int;
wire                     s_axis_data_1_tready_int;

wire [DATA_WIDTH  -1:0]  m_axis_out_tdata_int;
wire [DATA_WIDTH/8-1:0]  m_axis_out_tkeep_int;
wire [DEST_WIDTH  -1:0]  m_axis_out_tdest_int;
wire                     m_axis_out_tvalid_int;
wire                     m_axis_out_tready_int;

reg  [1:0] state;

assign s_axis_header_tready_int = (state == 2'b00) ? m_axis_out_tready_int : 1'b0;
assign s_axis_data_0_tready_int = (state == 2'b01) ? m_axis_out_tready_int : 1'b0;
assign s_axis_data_1_tready_int = (state == 2'b10) ? m_axis_out_tready_int : 1'b0;

assign m_axis_out_tdata_int     = (state == 2'b00) ? s_axis_header_tdata_int  : ((state == 2'b01) ? s_axis_data_0_tdata_int  : ((state == 2'b10) ? s_axis_data_1_tdata_int  : 'b0));
assign m_axis_out_tkeep_int     = (state == 2'b00) ? s_axis_header_tkeep_int  : ((state == 2'b01) ? s_axis_data_0_tkeep_int  : ((state == 2'b10) ? s_axis_data_1_tkeep_int  : 'b0));
assign m_axis_out_tdest_int     = (state == 2'b00) ? s_axis_header_tdest_int  : ((state == 2'b01) ? s_axis_data_0_tdest_int  : ((state == 2'b10) ? s_axis_data_1_tdest_int  : 'b0));
assign m_axis_out_tvalid_int    = (state == 2'b00) ? s_axis_header_tvalid_int : ((state == 2'b01) ? s_axis_data_0_tvalid_int : ((state == 2'b10) ? s_axis_data_1_tvalid_int : 'b0));

always @(posedge aclk) begin
  if(aresetn == 1'b0) begin
    state <='b0;
  end else begin
    case(state)
      2'b00 : state <= ((s_axis_header_tready_int == 1'b1) && (s_axis_header_tvalid_int == 1'b1)) ? 2'b01 : 2'b00;
      2'b01 : state <= ((s_axis_data_0_tready_int == 1'b1) && (s_axis_data_0_tvalid_int == 1'b1)) ? 2'b10 : 2'b01;
      2'b10 : state <= ((s_axis_data_1_tready_int == 1'b1) && (s_axis_data_1_tvalid_int == 1'b1)) ? ((s_axis_data_1_tlast_int == 1'b1) ? 2'b00 : 2'b01) : 2'b10;
      2'b11 : state <= 2'b00;
    endcase
  end
end

axis_switch_reg_slice #
(
  .DATA_WIDTH (DATA_WIDTH),
  .DEST_WIDTH (DEST_WIDTH)
)
axis_switch_reg_slice_header
(
  .aclk          (aclk                    ),
  .aresetn       (aresetn                 ),
  .s_axis_tdata  (s_axis_header_tdata     ),
  .s_axis_tkeep  (s_axis_header_tkeep     ),
  .s_axis_tdest  (s_axis_header_tdest     ),
  .s_axis_tlast  (1'b0                    ),
  .s_axis_tvalid (s_axis_header_tvalid    ),
  .s_axis_tready (s_axis_header_tready    ),
  .m_axis_tdata  (s_axis_header_tdata_int ),
  .m_axis_tkeep  (s_axis_header_tkeep_int ),
  .m_axis_tdest  (s_axis_header_tdest_int ),
  .m_axis_tlast  (                        ),
  .m_axis_tvalid (s_axis_header_tvalid_int),
  .m_axis_tready (s_axis_header_tready_int)
);

axis_switch_reg_slice #
(
  .DATA_WIDTH (DATA_WIDTH),
  .DEST_WIDTH (DEST_WIDTH)
)
axis_switch_reg_slice_data_0
(
  .aclk          (aclk                    ),
  .aresetn       (aresetn                 ),
  .s_axis_tdata  (s_axis_data_0_tdata     ),
  .s_axis_tkeep  (s_axis_data_0_tkeep     ),
  .s_axis_tdest  (s_axis_data_0_tdest     ),
  .s_axis_tlast  (1'b0                    ),
  .s_axis_tvalid (s_axis_data_0_tvalid    ),
  .s_axis_tready (s_axis_data_0_tready    ),
  .m_axis_tdata  (s_axis_data_0_tdata_int ),
  .m_axis_tkeep  (s_axis_data_0_tkeep_int ),
  .m_axis_tdest  (s_axis_data_0_tdest_int ),
  .m_axis_tlast  (                        ),
  .m_axis_tvalid (s_axis_data_0_tvalid_int),
  .m_axis_tready (s_axis_data_0_tready_int)
);

axis_switch_reg_slice #
(
  .DATA_WIDTH (DATA_WIDTH),
  .DEST_WIDTH (DEST_WIDTH)
)
axis_switch_reg_slice_data_1
(
  .aclk          (aclk                    ),
  .aresetn       (aresetn                 ),
  .s_axis_tdata  (s_axis_data_1_tdata     ),
  .s_axis_tkeep  (s_axis_data_1_tkeep     ),
  .s_axis_tdest  (s_axis_data_1_tdest     ),
  .s_axis_tlast  (s_axis_data_1_tlast     ),
  .s_axis_tvalid (s_axis_data_1_tvalid    ),
  .s_axis_tready (s_axis_data_1_tready    ),
  .m_axis_tdata  (s_axis_data_1_tdata_int ),
  .m_axis_tkeep  (s_axis_data_1_tkeep_int ),
  .m_axis_tdest  (s_axis_data_1_tdest_int ),
  .m_axis_tlast  (s_axis_data_1_tlast_int ),
  .m_axis_tvalid (s_axis_data_1_tvalid_int),
  .m_axis_tready (s_axis_data_1_tready_int)
);

axis_switch_reg_slice #
(
  .DATA_WIDTH (DATA_WIDTH),
  .DEST_WIDTH (DEST_WIDTH)
)
axis_switch_reg_slice_out
(
  .aclk          (aclk                  ),
  .aresetn       (aresetn               ),
  .s_axis_tdata  (m_axis_out_tdata_int  ),
  .s_axis_tkeep  (m_axis_out_tkeep_int  ),
  .s_axis_tdest  (m_axis_out_tdest_int  ),
  .s_axis_tlast  (1'b0                  ),
  .s_axis_tvalid (m_axis_out_tvalid_int ),
  .s_axis_tready (m_axis_out_tready_int ),
  .m_axis_tdata  (m_axis_out_tdata      ),
  .m_axis_tkeep  (m_axis_out_tkeep      ),
  .m_axis_tdest  (m_axis_out_tdest      ),
  .m_axis_tlast  (                      ),
  .m_axis_tvalid (m_axis_out_tvalid     ),
  .m_axis_tready (m_axis_out_tready     )
);

endmodule

module axis_switch_reg_slice #
(
    parameter DATA_WIDTH = 256,
    parameter DEST_WIDTH = 32
)
(
  input                     aclk,
  input                     aresetn,

  input  [DATA_WIDTH  -1:0] s_axis_tdata,
  input  [DATA_WIDTH/8-1:0] s_axis_tkeep,
  input  [DEST_WIDTH  -1:0] s_axis_tdest,
  input                     s_axis_tlast,
  input                     s_axis_tvalid,
  output                    s_axis_tready,

  output [DATA_WIDTH  -1:0] m_axis_tdata,
  output [DATA_WIDTH/8-1:0] m_axis_tkeep,
  output [DEST_WIDTH  -1:0] m_axis_tdest,
  output                    m_axis_tlast,
  output                    m_axis_tvalid,
  input                     m_axis_tready
);

reg  [511:0] tdata_r[1:0];
reg  [ 63:0] tkeep_r[1:0];
reg  [511:0] tdest_r[1:0];
reg  [  1:0] tlast_r;
reg  [  1:0] status_r;
reg          write_tgl_r;
reg          read_tgl_r;
wire         wr_en_w;
wire         rd_en_w;

assign s_axis_tready = (status_r != 2'b11) ? 1'b1 : 1'b0;
assign wr_en_w  = ((status_r != 2'b11) && (s_axis_tvalid == 1'b1)) ? 1'b1 : 1'b0;
assign rd_en_w  = ((status_r[read_tgl_r] == 1'b1) && (m_axis_tready == 1'b1)) ? 1'b1 : 1'b0;

assign m_axis_tdata  = tdata_r[read_tgl_r];
assign m_axis_tkeep  = tkeep_r[read_tgl_r];
assign m_axis_tdest  = tdest_r[read_tgl_r];
assign m_axis_tlast  = tlast_r[read_tgl_r];
assign m_axis_tvalid = (status_r[read_tgl_r] == 1'b1) ? 1'b1 : 1'b0;

always @(posedge aclk) begin
  if(aresetn == 1'b0) begin
    tdata_r[0]  <= 'b0;
    tdata_r[1]  <= 'b0;
    tkeep_r[0]  <= 'b0;
    tkeep_r[1]  <= 'b0;
    tdest_r[0]  <= 'b0;
    tdest_r[1]  <= 'b0;
    tlast_r[0]  <= 'b0;
    tlast_r[1]  <= 'b0;
    status_r    <= 'b0;
    write_tgl_r <= 'b0;
    read_tgl_r  <= 'b0;
  end else begin
    if(wr_en_w == 1'b1) begin
      tdata_r[write_tgl_r]  <= s_axis_tdata;
      tkeep_r[write_tgl_r]  <= s_axis_tkeep;
      tdest_r[write_tgl_r]  <= s_axis_tdest;
      tlast_r[write_tgl_r]  <= s_axis_tlast;
      status_r[write_tgl_r] <= 1'b1;
      write_tgl_r <= ~write_tgl_r;
    end

    if(rd_en_w == 1'b1) begin
      status_r[read_tgl_r] <= 1'b0;
      read_tgl_r <= ~read_tgl_r;
    end
  end
end

endmodule
