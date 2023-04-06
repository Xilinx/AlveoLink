module stream_flag_reg#
(
  parameter DATA_WIDTH = 64,
  parameter DEPTH      = (1 << 5)
)
(
  input                     clk,
  input                     rst_n,
  input  [DATA_WIDTH - 1:0] din,
  input                     wr_en,
  output                    full,
  output [DATA_WIDTH - 1:0] dout,
  input                     rd_en,
  output                    empty
);

localparam ADDRESS_WIDTH = $clog2(DEPTH);

reg  [   DATA_WIDTH - 1:0] memory [0:DEPTH - 1];
reg  [ADDRESS_WIDTH - 1:0] wr_addr;
reg  [ADDRESS_WIDTH - 1:0] rd_addr;

reg                        full_r;
reg                        empty_r;

integer i;
initial begin
  for (i=0;i<=DEPTH;i=i+1)
    memory[i] = 0;
end

always @(posedge clk) begin
  if (rst_n == 1'b0) begin
    wr_addr <= 'b0;
  end else if (full_r == 1'b0 && wr_en == 1'b1) begin
    memory[wr_addr] <= din;
    wr_addr <= wr_addr + 1'b1;
  end
end

always @(posedge clk) begin
  if (rst_n == 1'b0) begin
    rd_addr <= 'b0;
  end else if (empty_r == 1'b0 && rd_en == 1'b1) begin
    rd_addr <= rd_addr + 1'b1;
  end
end

always @(posedge clk) begin
  if (rst_n == 1'b0) begin
    empty_r <= 1'b1;
  end else begin
    if(empty_r == 1'b1 && wr_en == 1'b1) begin
      empty_r <= 1'b0;
    end else if (wr_en == 1'b0 && rd_en == 1'b1 && ((rd_addr + 1'b1) == wr_addr)) begin
      empty_r <= 1'b1;
    end
  end
end

always @(posedge clk) begin
  if (rst_n == 1'b0) begin
    full_r <= 1'b0;
  end else begin
    if(full_r == 1'b1 && rd_en == 1'b1) begin
      full_r <= 1'b0;
    end else if (wr_en == 1'b1 && rd_en == 1'b0 && ((wr_addr + 1'b1) == rd_addr)) begin
      full_r <= 1'b1;
    end
  end
end

assign full   = full_r;
assign empty  = empty_r;
assign dout   = memory[rd_addr];

endmodule
