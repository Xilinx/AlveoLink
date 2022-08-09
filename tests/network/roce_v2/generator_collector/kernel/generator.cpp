#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

typedef ap_axiu<512,0,0,13> out_t;

typedef ap_axiu<512,0,0,13> in_t;
extern "C" void generator_collector(	ap_uint<1> &gen_col_i,
						 	ap_uint<1> &start_i,
							ap_uint<13> &id_i,
							ap_uint<8> &byte_counter_o,
							unsigned int &pkt_cnt_o,

							hls::stream<out_t> &out_to_Hivenet_o,
								unsigned int packets_to_send_i,

							hls::stream<in_t> &input_from_Hivenet_i,
								ap_uint<1> &error_o
							){

	static ap_uint<8> byte_counter=0;
	static ap_uint<32> packet_counter=0;
	static ap_uint<1> error=0;

	byte_counter_o=byte_counter;
	pkt_cnt_o=packet_counter;
	error_o=error;

	if(start_i==0){//reset
		byte_counter=0;
		packet_counter=0;
		error=0;
	}else{
		if(gen_col_i==1){//generator
			if(packet_counter<packets_to_send_i){
				out_t dat;
				dat.dest=id_i;
				dat.keep=0xFFFFFFFFFFFFFFFF;
				dat.last=1;
				for(int i=0;i<64;++i){
					dat.data.range((i+1)*8-1,i*8)=id_i^byte_counter^packet_counter.range(7,0);
					byte_counter++;
				}
				packet_counter++;
				out_to_Hivenet_o.write(dat);
			}
		}else{//collector
			in_t dat;
			if(input_from_Hivenet_i.read_nb(dat)){
				for(int i=0;i<64;++i){
					error|=(((dat.data.range((i+1)*8-1,i*8))^packet_counter.range(7,0)^byte_counter)==id_i?0:1);
					byte_counter++;
				}
				packet_counter++;
			}
		}

	}
}
