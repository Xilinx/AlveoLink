`timescale 1ns/1ps

module ip_handler#
(
  parameter IN_RAW_REG_SLICE_BYPASS      = 1,
  parameter OUT_ARP_REG_SLICE_BYPASS     = 1,
  parameter OUT_ICMP_REG_SLICE_BYPASS    = 1,
  parameter OUT_HIVENET_REG_SLICE_BYPASS = 1,
  parameter OUT_OTHER_REG_SLICE_BYPASS   = 1
)
(
  // System Signals
  input          aclk,
  input          aresetn,
  // Slave Stream Signals
  input  [511:0] s_axis_raw_tdata,
  input  [ 63:0] s_axis_raw_tkeep,
  input          s_axis_raw_tlast,
  input          s_axis_raw_tvalid,
  output         s_axis_raw_tready,
  // ARP Master Stream Signals
  output [511:0] m_axis_arp_tdata,
  output [ 63:0] m_axis_arp_tkeep,
  output         m_axis_arp_tlast,
  output         m_axis_arp_tvalid,
  input          m_axis_arp_tready,
  // ICMP Master Stream Signals
  output [511:0] m_axis_icmp_tdata,
  output [ 63:0] m_axis_icmp_tkeep,
  output         m_axis_icmp_tlast,
  output         m_axis_icmp_tvalid,
  input          m_axis_icmp_tready,
  // HIVENET Master Stream Signals
  output [511:0] m_axis_hivenet_tdata,
  output [ 63:0] m_axis_hivenet_tkeep,
  output         m_axis_hivenet_tlast,
  output         m_axis_hivenet_tvalid,
  input          m_axis_hivenet_tready,
  // OTHER Master Stream Signals
  output [511:0] m_axis_other_tdata,
  output [ 63:0] m_axis_other_tkeep,
  output         m_axis_other_tlast,
  output         m_axis_other_tvalid,
  input          m_axis_other_tready,
  // Config inputs
  input  [ 31:0] my_ip_address,
  input  [ 15:0] roce_port
);

wire [511:0] axis_int_tdata;
wire [ 63:0] axis_int_tkeep;
wire         axis_int_tlast;
wire         axis_int_tvalid;
wire         axis_int_tready;

wire [511:0] axis_arp_tdata;
wire [ 63:0] axis_arp_tkeep;
wire         axis_arp_tlast;
wire         axis_arp_tvalid;
wire         axis_arp_tready;

wire [511:0] axis_icmp_tdata;
wire [ 63:0] axis_icmp_tkeep;
wire         axis_icmp_tlast;
wire         axis_icmp_tvalid;
wire         axis_icmp_tready;

wire [511:0] axis_hivenet_tdata;
wire [ 63:0] axis_hivenet_tkeep;
wire         axis_hivenet_tlast;
wire         axis_hivenet_tvalid;
wire         axis_hivenet_tready;

wire [511:0] axis_other_tdata;
wire [ 63:0] axis_other_tkeep;
wire         axis_other_tlast;
wire         axis_other_tvalid;
wire         axis_other_tready;

reg          first_int;

reg  [ 31:0] my_ip_address_reg;
reg  [ 15:0] roce_port_reg;

wire         is_arp_packet_w;
wire         is_icmp_packet_w;
wire         is_hivenet_packet_w;
wire         is_other_packet_w;

reg          is_arp_packet_r;
reg          is_icmp_packet_r;
reg          is_hivenet_packet_r;
reg          is_other_packet_r;

wire         connect_arp;
wire         connect_icmp;
wire         connect_hivenet;
wire         connect_other;

wire [ 15:0] ether_type;
wire [  7:0] ip_prot;
wire [  3:0] ip_header_length;
wire [ 31:0] ip_dest_ip;
wire [ 15:0] udp_dest_port;

assign ether_type          = ((first_int == 1'b1) && (axis_int_tkeep[12+:2] ==   2'b11)) ? axis_int_tdata[12*8+:16] :     16'h0000;
assign ip_prot             = ((first_int == 1'b1) && (axis_int_tkeep[23+:1] ==    1'b1)) ? axis_int_tdata[23*8+: 8] :        8'h00;
assign ip_header_length    = ((first_int == 1'b1) && (axis_int_tkeep[14+:1] ==    1'b1)) ? axis_int_tdata[14*8+: 4] :         4'h0;
assign ip_dest_ip          = ((first_int == 1'b1) && (axis_int_tkeep[30+:4] == 4'b1111)) ? axis_int_tdata[30*8+:32] : 32'h00000000;
assign udp_dest_port       = ((first_int == 1'b1) && (axis_int_tkeep[36+:2] ==   2'b11)) ? axis_int_tdata[36*8+:16] :     16'h0000;

assign is_arp_packet_w     = (ether_type == 16'h0608) ? 1'b1 : 1'b0;

assign is_icmp_packet_w    = ((ether_type == 16'h0008) &&
                              (ip_prot == 8'h01) &&
                              (ip_header_length == 4'h5) &&
                              ((ip_dest_ip == my_ip_address_reg) || (ip_dest_ip == 32'hFFFFFFFF))) ? 1'b1 : 1'b0;

assign is_hivenet_packet_w = ((ether_type == 16'h0008) &&
                              (ip_prot == 8'h11) &&
                              (ip_header_length == 4'h5) &&
                              ((ip_dest_ip == my_ip_address_reg) || (ip_dest_ip == 32'hFFFFFFFF)) &&
                              (udp_dest_port == roce_port_reg)) ? 1'b1 : 1'b0;

assign is_other_packet_w   = ((is_arp_packet_w == 1'b0) && (is_icmp_packet_w == 1'b0) && (is_hivenet_packet_w == 1'b0)) ? 1'b1 : 1'b0;

assign connect_arp         = ((~first_int) & is_arp_packet_r    ) | (first_int & is_arp_packet_w    );
assign connect_icmp        = ((~first_int) & is_icmp_packet_r   ) | (first_int & is_icmp_packet_w   );
assign connect_hivenet     = ((~first_int) & is_hivenet_packet_r) | (first_int & is_hivenet_packet_w);
assign connect_other       = ((~first_int) & is_other_packet_r  ) | (first_int & is_other_packet_w  );

assign axis_int_tready     = (connect_arp     == 1'b1) ? axis_arp_tready     :
                             (connect_icmp    == 1'b1) ? axis_icmp_tready    :
                             (connect_hivenet == 1'b1) ? axis_hivenet_tready :
                             (connect_other   == 1'b1) ? axis_other_tready   :
                             1'b0;

assign axis_arp_tdata      = axis_int_tdata;
assign axis_icmp_tdata     = axis_int_tdata;
assign axis_hivenet_tdata  = axis_int_tdata;
assign axis_other_tdata    = axis_int_tdata;

assign axis_arp_tkeep      = axis_int_tkeep;
assign axis_icmp_tkeep     = axis_int_tkeep;
assign axis_hivenet_tkeep  = axis_int_tkeep;
assign axis_other_tkeep    = axis_int_tkeep;

assign axis_arp_tlast      = axis_int_tlast;
assign axis_icmp_tlast     = axis_int_tlast;
assign axis_hivenet_tlast  = axis_int_tlast;
assign axis_other_tlast    = axis_int_tlast;

assign axis_arp_tvalid     = (connect_arp     == 1'b1) ? axis_int_tvalid : 1'b0;
assign axis_icmp_tvalid    = (connect_icmp    == 1'b1) ? axis_int_tvalid : 1'b0;
assign axis_hivenet_tvalid = (connect_hivenet == 1'b1) ? axis_int_tvalid : 1'b0;
assign axis_other_tvalid   = (connect_other   == 1'b1) ? axis_int_tvalid : 1'b0;

always@(posedge aclk)
begin
  if(aresetn == 1'b0) begin
    first_int <= 1'b1;
  end else begin
    if(first_int == 1'b1) begin
      if((axis_int_tvalid == 1'b1) && (axis_int_tready == 1'b1) && (axis_int_tlast == 1'b0)) begin
        first_int <= 1'b0;
      end
    end else begin
      if((axis_int_tvalid == 1'b1) && (axis_int_tready == 1'b1) && (axis_int_tlast == 1'b1)) begin
        first_int <= 1'b1;
      end
    end
  end
end

always@(posedge aclk)
begin
  if(aresetn == 1'b0) begin
    my_ip_address_reg <= 'b0;
    roce_port_reg <= 'b0;
  end else begin
    my_ip_address_reg <= my_ip_address;
    roce_port_reg[7:0] <= roce_port[15:8];
    roce_port_reg[15:8] <= roce_port[7:0];
  end
end

always @(posedge aclk) begin
  if (aresetn == 1'b0) begin
    is_arp_packet_r <= 1'b0;
  end else begin
    if((first_int == 1'b1) && (axis_int_tvalid == 1'b1) && (axis_int_tready == 1'b1)) begin
      is_arp_packet_r <= is_arp_packet_w;
    end
  end
end

always @(posedge aclk) begin
  if (aresetn == 1'b0) begin
    is_icmp_packet_r <= 1'b0;
  end else begin
    if((first_int == 1'b1) && (axis_int_tvalid == 1'b1) && (axis_int_tready == 1'b1)) begin
      is_icmp_packet_r <= is_icmp_packet_w;
    end
  end
end

always @(posedge aclk) begin
  if (aresetn == 1'b0) begin
    is_hivenet_packet_r <= 1'b0;
  end else begin
    if((first_int == 1'b1) && (axis_int_tvalid == 1'b1) && (axis_int_tready == 1'b1)) begin
      is_hivenet_packet_r <= is_hivenet_packet_w;
    end
  end
end

always @(posedge aclk) begin
  if (aresetn == 1'b0) begin
    is_other_packet_r <= 1'b0;
  end else begin
    if((first_int == 1'b1) && (axis_int_tvalid == 1'b1) && (axis_int_tready == 1'b1)) begin
      is_other_packet_r <= is_other_packet_w;
    end
  end
end

axis_register_ip_handler#
(
  .BYPASS (IN_RAW_REG_SLICE_BYPASS)
)
axis_register_ip_handler_input_inst
(
  .aclk          (aclk             ),
  .aresetn       (aresetn          ),
  .s_axis_tdata  (s_axis_raw_tdata ),
  .s_axis_tkeep  (s_axis_raw_tkeep ),
  .s_axis_tlast  (s_axis_raw_tlast ),
  .s_axis_tvalid (s_axis_raw_tvalid),
  .s_axis_tready (s_axis_raw_tready),
  .m_axis_tdata  (axis_int_tdata   ),
  .m_axis_tkeep  (axis_int_tkeep   ),
  .m_axis_tlast  (axis_int_tlast   ),
  .m_axis_tvalid (axis_int_tvalid  ),
  .m_axis_tready (axis_int_tready  )
);

axis_register_ip_handler#
(
  .BYPASS (OUT_ARP_REG_SLICE_BYPASS)
)
axis_register_ip_handler_arp_inst
(
  .aclk          (aclk             ),
  .aresetn       (aresetn          ),
  .s_axis_tdata  (axis_arp_tdata   ),
  .s_axis_tkeep  (axis_arp_tkeep   ),
  .s_axis_tlast  (axis_arp_tlast   ),
  .s_axis_tvalid (axis_arp_tvalid  ),
  .s_axis_tready (axis_arp_tready  ),
  .m_axis_tdata  (m_axis_arp_tdata ),
  .m_axis_tkeep  (m_axis_arp_tkeep ),
  .m_axis_tlast  (m_axis_arp_tlast ),
  .m_axis_tvalid (m_axis_arp_tvalid),
  .m_axis_tready (m_axis_arp_tready)
);

axis_register_ip_handler#
(
  .BYPASS (OUT_ICMP_REG_SLICE_BYPASS)
)
axis_register_ip_handler_icmp_inst
(
  .aclk          (aclk              ),
  .aresetn       (aresetn           ),
  .s_axis_tdata  (axis_icmp_tdata   ),
  .s_axis_tkeep  (axis_icmp_tkeep   ),
  .s_axis_tlast  (axis_icmp_tlast   ),
  .s_axis_tvalid (axis_icmp_tvalid  ),
  .s_axis_tready (axis_icmp_tready  ),
  .m_axis_tdata  (m_axis_icmp_tdata ),
  .m_axis_tkeep  (m_axis_icmp_tkeep ),
  .m_axis_tlast  (m_axis_icmp_tlast ),
  .m_axis_tvalid (m_axis_icmp_tvalid),
  .m_axis_tready (m_axis_icmp_tready)
);


axis_register_ip_handler#
(
  .BYPASS (OUT_HIVENET_REG_SLICE_BYPASS)
)
axis_register_ip_handler_hivenet_inst
(
  .aclk          (aclk                 ),
  .aresetn       (aresetn              ),
  .s_axis_tdata  (axis_hivenet_tdata   ),
  .s_axis_tkeep  (axis_hivenet_tkeep   ),
  .s_axis_tlast  (axis_hivenet_tlast   ),
  .s_axis_tvalid (axis_hivenet_tvalid  ),
  .s_axis_tready (axis_hivenet_tready  ),
  .m_axis_tdata  (m_axis_hivenet_tdata ),
  .m_axis_tkeep  (m_axis_hivenet_tkeep ),
  .m_axis_tlast  (m_axis_hivenet_tlast ),
  .m_axis_tvalid (m_axis_hivenet_tvalid),
  .m_axis_tready (m_axis_hivenet_tready)
);


axis_register_ip_handler#
(
  .BYPASS (OUT_OTHER_REG_SLICE_BYPASS)
)
axis_register_ip_handler_other_inst
(
  .aclk          (aclk               ),
  .aresetn       (aresetn            ),
  .s_axis_tdata  (axis_other_tdata   ),
  .s_axis_tkeep  (axis_other_tkeep   ),
  .s_axis_tlast  (axis_other_tlast   ),
  .s_axis_tvalid (axis_other_tvalid  ),
  .s_axis_tready (axis_other_tready  ),
  .m_axis_tdata  (m_axis_other_tdata ),
  .m_axis_tkeep  (m_axis_other_tkeep ),
  .m_axis_tlast  (m_axis_other_tlast ),
  .m_axis_tvalid (m_axis_other_tvalid),
  .m_axis_tready (m_axis_other_tready)
);


endmodule

module axis_register_ip_handler#
(
  parameter BYPASS = 1
)
(
  input          aclk,
  input          aresetn,

  input  [511:0] s_axis_tdata,
  input  [ 63:0] s_axis_tkeep,
  input          s_axis_tlast,
  input          s_axis_tvalid,
  output         s_axis_tready,

  output [511:0] m_axis_tdata,
  output [ 63:0] m_axis_tkeep,
  output         m_axis_tlast,
  output         m_axis_tvalid,
  input          m_axis_tready
);

generate
  if(BYPASS == 0) begin
    reg  [511:0] tdata_r[1:0];
    reg  [ 63:0] tkeep_r[1:0];
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
    assign m_axis_tlast  = tlast_r[read_tgl_r];
    assign m_axis_tvalid = (status_r[read_tgl_r] == 1'b1) ? 1'b1 : 1'b0;

    always @(posedge aclk) begin
      if(aresetn == 1'b0) begin
        tdata_r[0]  <= 'b0;
        tdata_r[1]  <= 'b0;
        tkeep_r[0]  <= 'b0;
        tkeep_r[1]  <= 'b0;
        tlast_r[0]  <= 'b0;
        tlast_r[1]  <= 'b0;
        status_r    <= 'b0;
        write_tgl_r <= 'b0;
        read_tgl_r  <= 'b0;
      end else begin
        if(wr_en_w == 1'b1) begin
          tdata_r[write_tgl_r]  <= s_axis_tdata;
          tkeep_r[write_tgl_r]  <= s_axis_tkeep;
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
  end else begin
    assign m_axis_tdata  = s_axis_tdata;
    assign m_axis_tkeep  = s_axis_tkeep;
    assign m_axis_tlast  = s_axis_tlast;
    assign m_axis_tvalid = s_axis_tvalid;
    assign s_axis_tready = m_axis_tready;
  end
endgenerate

endmodule
