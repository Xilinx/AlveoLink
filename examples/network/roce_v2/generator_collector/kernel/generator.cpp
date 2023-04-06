/*
 * Copyright 2019-2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

typedef ap_axiu<512,0,0,13> out_t;

typedef ap_axiu<512,0,0,13> in_t;
extern "C" void generator_collector(ap_uint<1> &gen_col_i,
						 			ap_uint<1> &start_i,
									ap_uint<13> &id_i,
									ap_uint<48> &pkt_cnt_o,

									hls::stream<out_t> &out_to_Hivenet_o,
										ap_uint<48> packets_to_send_i,

									hls::stream<in_t> &input_from_Hivenet_i,
										ap_uint<1> &error_o,

									ap_uint<16> & dim_i
								){
#pragma HLS INTERFACE mode=s_axilite port=dim_i
#pragma HLS INTERFACE mode=s_axilite port=error_o
#pragma HLS INTERFACE mode=s_axilite port=packets_to_send_i
#pragma HLS INTERFACE mode=axis register_mode=both port=input_from_Hivenet_i register
#pragma HLS INTERFACE mode=axis register_mode=both port=out_to_Hivenet_o register
#pragma HLS INTERFACE mode=s_axilite port=start_i
#pragma HLS INTERFACE mode=s_axilite port=pkt_cnt_o
#pragma HLS INTERFACE mode=s_axilite port=id_i
#pragma HLS INTERFACE mode=s_axilite port=gen_col_i
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS PIPELINE II=1

	static ap_uint<8> byte_counter_coll[8192]={0};
	static ap_uint<48> packet_counter_coll[8192]={0};
	static ap_uint<8> byte_counter=0;

	static ap_uint<1> error=0;
	static ap_uint<48> packet_counter_all=0;

	pkt_cnt_o=packet_counter_all;
	error_o=error;
	static ap_uint<13> id=0;
	static ap_uint<16> dim=1;
	static ap_uint<16> final_dim=1;

	static enum{BRAM_READ,BRAM_WRITE} collector_state=BRAM_READ;
	static ap_uint<8> byte_counter_loc=0;
	static ap_uint<48> packet_counter_loc=0;
	static bool first_col=true;
	static ap_uint<13> dest_for_write=0;
	static ap_uint<1> can_be_reseted=1;

	if(start_i==0 && can_be_reseted==1){//reset
		byte_counter=0;
		error=0;
		packet_counter_all=0;
		byte_counter_coll[id]=0;
		packet_counter_coll[id]=0;
		collector_state=BRAM_READ;
		byte_counter_loc=0;
		packet_counter_loc=0;
		first_col=true;
		dest_for_write=0;
		id++;
		dim=1;
		final_dim=1;
	}else{
		if(dim_i<16)
			final_dim=dim_i;
		else
			final_dim=1;

		in_t dat;

		if(gen_col_i==1){//generator
			if(packet_counter_all<packets_to_send_i){
				out_t dat;
				dat.dest=id_i;
				dat.keep=0xFFFFFFFFFFFFFFFF;

				for(int i=0;i<64;++i){
					dat.data.range((i+1)*8-1,i*8)=id_i^byte_counter^packet_counter_all.range(7,0);
					byte_counter++;
				}
				if(dim==final_dim){
					dat.last=1;
					dim=1;
					packet_counter_all++;
					can_be_reseted=1;
				}else{
					dat.last=0;
					dim++;
					can_be_reseted=0;
				}
				out_to_Hivenet_o.write(dat);
			}else{
				can_be_reseted=1;
			}
		}else{//collector
			switch(collector_state){
				case BRAM_READ:

					if(input_from_Hivenet_i.read_nb(dat)){
						if(first_col){
							byte_counter_loc=byte_counter_coll[dat.dest];
							packet_counter_loc=packet_counter_coll[dat.dest];
							first_col=false;
							dest_for_write=dat.dest;
						}
						for(int i=0;i<64;++i){
							error|=((((dat.data.range((i+1)*8-1,i*8))^(packet_counter_loc.range(7,0))^(byte_counter_loc))==id_i)?0:1);
							byte_counter_loc++;
						}

						if(dim==final_dim){
							error|=dat.last==1?0:1;
						}else{
							dim++;
						}
						if(dat.last==1){
							collector_state=BRAM_WRITE;
							first_col=true;
						}
					}

				break;
				case BRAM_WRITE:
					packet_counter_loc++;
					packet_counter_all++;
					byte_counter_coll[dest_for_write]=byte_counter_loc;
					packet_counter_coll[dest_for_write]=packet_counter_loc;
					collector_state=BRAM_READ;
					dim=1;

				break;
			}
			can_be_reseted=1;
		}

	}
}
