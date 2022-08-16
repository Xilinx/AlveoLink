#include <ap_int.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"

typedef ap_axiu<512,0,0,13> out_t;

typedef ap_axiu<512,0,0,13> in_t;
extern "C" void generator_collector(ap_uint<1> gen_col_i,
							ap_uint<13> id_i,
						    unsigned int packets_num,
							ap_uint<8> &byte_counter_o,
							unsigned int &pkt_cnt_o,
							ap_uint<1> &error_o,

                            hls::stream<in_t> &input_from_Hivenet_i,
							hls::stream<out_t> &out_to_Hivenet_o) {

#pragma HLS INTERFACE mode=ap_vld port=byte_counter_o
#pragma HLS INTERFACE mode=ap_vld port=pkt_cnt_o
#pragma HLS INTERFACE mode=ap_vld port=error_o

	ap_uint<8> byte_counter=0;
	ap_uint<32> packet_counter=0;
	ap_uint<1> error=0;


    if(gen_col_i==1){//generator
        for (auto i=0; i<packets_num; ++i) {
#pragma HLS PIPELINE II=1
            packet_counter = i;
            out_t dat;
            dat.dest=id_i;
            dat.keep=-1;
            dat.last=1;
            for(int i=0;i<64;++i){
                dat.data.range((i+1)*8-1,i*8)=id_i^byte_counter^packet_counter.range(7,0);
                byte_counter++;
            }
            out_to_Hivenet_o.write(dat);
        }
    }else{//collector
        for (auto n=0; n<packets_num; ++n) {
#pragma HLS PIPELINE II=1
            packet_counter = n;
            in_t dat;
            dat = input_from_Hivenet_i.read();
            for(int i=0;i<64;++i){
                error|=(((dat.data.range((i+1)*8-1,i*8))^packet_counter.range(7,0)^byte_counter)==id_i?0:1);
                byte_counter++;
            }
        }
    }

	byte_counter_o=byte_counter;
	pkt_cnt_o=packets_num;
	error_o=error;
}
