// Copyright 2019-2022 Xilinx, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

`timescale 1 ns / 1 ps

module cut_packet_hls_deadlock_idx0_monitor ( // for module cut_packet_cut_packet_inst
    input wire clock,
    input wire reset,
    input wire [1:0] axis_block_sigs,
    input wire [0:0] inst_idle_sigs,
    input wire [0:0] inst_block_sigs,
    output wire block
);

// signal declare
reg monitor_find_block;
wire pp_is_axis_block;

assign block = monitor_find_block;
assign pp_is_axis_block = 1'b0 | axis_block_sigs[0] | axis_block_sigs[1];

always @(posedge clock) begin
    if (reset == 1'b1)
        monitor_find_block <= 1'b0;
    else if (pp_is_axis_block == 1'b1)
        monitor_find_block <= 1'b1;
    else
        monitor_find_block <= 1'b0;
end


// instant sub module
endmodule
