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

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <ctime>
#include <stdint.h>
#include <signal.h>

#include "netLayer.hpp"
#include "genColHost.hpp"
#include "popl.hpp"

using namespace popl;
genColHost<2> l_genColHost[2];

static void stop_Gen_Coll(){                              //resetting the kernels 
    std::cout<<std::endl<<"Resetting kernels.\n";
    for(int i=0;i<2;++i){                                 //first resetting the generators so that the packets in the pipeline can reach the collector
        for(int j=0;j<2;++j){
            if(l_genColHost[i].stopIP(j,generator)==-1){
                std::cout<<"Generator_"<<i<<" on device "<<j<<" is not reseting please reset the device."<<std::endl;
            }
        }
    }
    sleep(2);                                            //this is done to wait for all of the packets in the pipeline to reach the collectors
    for(int i=0;i<2;++i){
        for(int j=0;j<2;++j){
            if(l_genColHost[i].stopIP(j,collector)==-1){     //reseting the collector 
                std::cout<<"Collector_"<<i<<" on device "<<j<<" is not reseting please reset the device."<<std::endl;
            }
        }
    }
  std::cout<<"Exiting"<<std::endl;
}

static void exit_handler(int signum) { 
  stop_Gen_Coll();
  exit(0);
}

int main(int argc,char** argv) {
  signal(SIGINT, exit_handler);


    OptionParser op("Allowed options");
    auto help_option = op.add<Switch>("h", "help", "produce help message");
    auto devID_1 = op.add<Value<int>, Attribute::optional>("d", "dev_1", "Device 1 PCIe number", 0);
    auto devID_2 = op.add<Value<int>, Attribute::optional>("k", "dev_2", "Device 2 PCIe number", 1);
    auto xclbin = op.add<Value<std::string> >("x", "xclbin", "xclbin", "build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/binary_container_1.xclbin");
    auto test_duration = op.add<Value<unsigned long long> >("t", "time", "number of seconds to run the test", 32);
    auto packet_dim = op.add<Value<int> >("n", "dim", "packet dimension", 1);

    op.parse(argc, argv);

    if (help_option->count() == 1) {
        std::cout << op << "\n";
        return 0;
    } else if (help_option->count() == 2) {
        std::cout << op.help(Attribute::advanced) << "\n";
        return 0;

    } else if (help_option->count() > 2) {
        std::cout << op.help(Attribute::expert) << "\n";
        return 0;
    }

    AlveoLink::common::FPGA fpga_card[2];
    fpga_card[0].setId(devID_1->value());
    fpga_card[1].setId(devID_2->value());
   
    AlveoLink::network_roce_v2::NetLayer<2> l_netLayer[2];
 
    std::cout << "Loading xclbin to FPGA_" << devID_1->value() << std::endl;
    fpga_card[0].load_xclbin(xclbin->value());
    std::cout << "Loading xclbin to FPGA_" << devID_2->value() << std::endl;
    fpga_card[1].load_xclbin(xclbin->value());

    for (auto i=0; i<2; ++i) {
        l_netLayer[i].init(&(fpga_card[i]));
        for (auto j=0; j<2; ++j) {
            l_netLayer[i].setIPSubnet(j, 0x0000a8c0);        //setting the IP subnet for HiveNet
            l_netLayer[i].setMACSubnet(j, 0x347844332211);   //setting the MAC subnet for HiveNet
            l_netLayer[i].setID(j, i*2+j+1);                 //setting the ID for the HiveNet
            l_netLayer[i].setVlanPFC(j,false);               //VLAN is not used here if the VLAN is needed it should be set like 
                                                             //setVlanPFC(const unsigned int p_portId, uint8_t turnON_VLAN, uint32_t pfc_code, uint32_t vlan_id);
        }
    }
    for (auto i=0; i<2; ++i) {
        for (auto j=0; j<2; ++j) {
            std::cout <<"INFO: turn on RS_FEC and configuring flow control for device " << i << " port " <<j << std::endl;
            l_netLayer[i].turnOn_RS_FEC(j, true);            //turning on the RS_FEC
            l_netLayer[i].turnOn_flow_control(j,true);       //turning on the GLOBAL PAUSE IEEE 802.3x in order to turn on the PFC
                                                             //turnOn_flow_control(const unsigned int p_portId,false,const short pfc_code)
        }
    }
    unsigned int l_totalDevLinksUp = 0;
    while (l_totalDevLinksUp < 2) {                          //checking the link in PHY connectors
        std::cout << "INFO: Waiting for links up on device " << l_totalDevLinksUp << std::endl;
        if (l_netLayer[l_totalDevLinksUp].linksUp()) {
            l_totalDevLinksUp++;
        }
    }

    
    GEN_COLL_TYPE l_genCol[2][2]={{collector,generator}, //device 0
                                  {generator,generator}};//device 1
    unsigned int l_rec_id=1;                         //this is the HiveNetID of the collector 
    unsigned long long l_numPkts=0xffffffffffff;     // this is max number of packets for the generator, this is done to test the design for couple of hours
    
    unsigned int l_dim=packet_dim->value();          //all of the gen/coll should work with the same packet dimension. 
                                                     //This can be implemented so that collector get the packet dimension from the first strob

    for (auto i=0; i<2; ++i) {
        l_genColHost[i].init(&(fpga_card[i]),l_genCol[i],l_rec_id,l_numPkts,l_dim);//initialising the kernels
    }
    for (auto i=0; i<2; ++i) {
        std::cout << "INFO: start collectors on device " << i << std::endl;
        l_genColHost[i].runCollectors();//running the collectors first
    }
    for (auto i=0; i<2; ++i) {
        std::cout << "INFO: start generators on device " << i << std::endl;
        l_genColHost[i].runGenerators();//running the generators
    }

    unsigned long long sentPackets_prev[2][2]={{0,0},
                                               {0,0}};
    unsigned long long timer=0;
    while (timer<=test_duration->value())
    {
        sleep(1);
        timer++;


        unsigned long long sentPackets_now[2][2];

        for (auto i=0; i<2; ++i) {
            for (auto j=0; j<2; ++j) {
                sentPackets_now[i][j]=l_genColHost[i].getCurrPkts(j);//getting current number of packets sent
                std::string kernel_name;
                if(l_genCol[i][j]==generator){
                    kernel_name="Generator_";
                }else{
                    kernel_name="Collector_";
                }
                kernel_name+=std::to_string(j);
                kernel_name+=" dev_";
                kernel_name+=std::to_string(i);


                std::cout<<kernel_name<<": bandwidth is "<<(((sentPackets_now[i][j]-sentPackets_prev[i][j])*(24+74+64*l_dim+4)*8)/((double)1000000000))<<" Gbit/s\n";
                std::cout<<kernel_name<<": throughput is "<<((sentPackets_now[i][j]-sentPackets_prev[i][j])*64*l_dim*8/((double)1000000000))<<" Gbit/s\n\n";
                sentPackets_prev[i][j]=sentPackets_now[i][j];
            }
        }
        for (auto i=0; i<2; ++i) {
            for (auto j=0; j<2; ++j) {
                if(l_genColHost[i].getErrors(j)){
                    std::cout <<"Test failed!" << std::endl;
                    stop_Gen_Coll();
                    return EXIT_FAILURE;
                }
            }
        }
    }
    
    std::cout <<"Test pass!" << std::endl;
    stop_Gen_Coll();
    return EXIT_SUCCESS;
}
