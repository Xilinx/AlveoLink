`timescale 1 ns / 1 ps 


module ecn_timer (
        diff,
        saved_time,
        ecn
);


input  [63:0] diff;
input  [63:0] saved_time;
output  [0:0] ecn;

assign ecn = ((diff < saved_time) ? 1'b1 : 1'b0);

endmodule //ecn_timer
