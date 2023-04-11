`timescale 1ns/1ps

module drop_tusrer#
(
  parameter                      DATA_BYTES      = 64,
  parameter                      MAX_PACKET_SIZE = 9600
)
(
  // System Signals
  input                          aclk,
  input                          aresetn,
  // Slave Stream Signals
  input       [DATA_BYTES*8-1:0] s_axis_tdata,
  input       [  DATA_BYTES-1:0] s_axis_tkeep,
  input                          s_axis_tlast,
  input                          s_axis_tuser,
  input                          s_axis_tvalid,
  output wire                    s_axis_tready,
  // Master Stream Signals
  output wire [DATA_BYTES*8-1:0] m_axis_tdata,
  output wire [  DATA_BYTES-1:0] m_axis_tkeep,
  output wire                    m_axis_tlast,
  output wire                    m_axis_tvalid,
  input                          m_axis_tready,

  output                         drop_tusrer
);

wire                    packet_good;


wire [             1:0] stream_flag_din;
wire                    stream_flag_wr_en;
wire                    stream_flag_full;
wire [             1:0] stream_flag_dout;
wire                    stream_flag_rd_en;
wire                    stream_flag_empty;

wire                    s_axis_tlast_w;
wire                    s_axis_tuser_w;

wire                    f_m_axis_tvalid_w;
wire                    f_m_axis_tready_w;
wire [DATA_BYTES*8-1:0] f_m_axis_tdata_w;
wire [  DATA_BYTES-1:0] f_m_axis_tkeep_w;
wire                    f_m_axis_tlast_w;

assign packet_good       = ~stream_flag_empty & f_m_axis_tvalid_w & ~s_axis_tuser_w & s_axis_tlast_w;

assign stream_flag_din   = {s_axis_tuser,s_axis_tlast};
assign stream_flag_wr_en = s_axis_tvalid & s_axis_tready & s_axis_tlast;
assign stream_flag_rd_en = (packet_good == 1'b1) ? (~stream_flag_empty & f_m_axis_tvalid_w & f_m_axis_tlast_w & f_m_axis_tready_w) :
                                                   (~stream_flag_empty & f_m_axis_tvalid_w & f_m_axis_tlast_w);
assign s_axis_tlast_w    = stream_flag_dout[0];
assign s_axis_tuser_w    = stream_flag_dout[1];

assign m_axis_tdata      = f_m_axis_tdata_w;
assign m_axis_tkeep      = f_m_axis_tkeep_w;
assign m_axis_tlast      = f_m_axis_tlast_w;
assign m_axis_tvalid     = f_m_axis_tvalid_w & packet_good;

assign f_m_axis_tready_w = (packet_good == 1'b1) ? (~stream_flag_empty & f_m_axis_tvalid_w & m_axis_tready) : (~stream_flag_empty & f_m_axis_tvalid_w);

assign drop_tusrer       = ((packet_good == 1'b0) && (f_m_axis_tready_w == 1'b1) && (f_m_axis_tvalid_w == 1'b1) && (f_m_axis_tlast_w == 1'b1)) ? 1'b1 : 1'b0;

stream_flag_reg #
(
  .DATA_WIDTH (2),
  .DEPTH      (1 << 9)
)
stream_flag_reg_inst
(
  .clk   (aclk              ),
  .rst_n (aresetn           ),
  .din   (stream_flag_din   ),
  .wr_en (stream_flag_wr_en ),
  .full  (stream_flag_full  ),
  .dout  (stream_flag_dout  ),
  .rd_en (stream_flag_rd_en ),
  .empty (stream_flag_empty )
);

fifo_packet_drop#
(
  .DATA_BYTES        (DATA_BYTES),
  .MAX_PACKET_SIZE   (MAX_PACKET_SIZE),
  .SPACE_FOR_PACKETS (2)
)
fifo_packet_drop_inst
(
  .s_aclk         (aclk             ),
  .s_aresetn      (aresetn          ),
  .s_axis_tvalid  (s_axis_tvalid    ),
  .s_axis_tready  (s_axis_tready    ),
  .s_axis_tdata   (s_axis_tdata     ),
  .s_axis_tkeep   (s_axis_tkeep     ),
  .s_axis_tuser   (1'b0             ),
  .s_axis_tlast   (s_axis_tlast     ),
  .m_axis_tvalid  (f_m_axis_tvalid_w),
  .m_axis_tready  (f_m_axis_tready_w),
  .m_axis_tdata   (f_m_axis_tdata_w ),
  .m_axis_tkeep   (f_m_axis_tkeep_w ),
  .m_axis_tuser   (                 ),
  .m_axis_tlast   (f_m_axis_tlast_w ),
  .axis_prog_full (                 )
);

endmodule
