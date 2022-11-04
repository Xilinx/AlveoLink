
#include "nhop_functions.hpp"

#include "ap_int.h"
#include "ctime.hpp"
#include "nHopUtils.hpp"
#include "utils.hpp"
#include "dvNetLayer.hpp"
#include "hwManagerHost.hpp"
#include "graphPktDefs.hpp"
#include "nHopHost.hpp"
#include "nHopPartition.hpp"

//run manager+nhop
int runManagerAndNhop(commentInfo &commentInfo, bool has_manager) {
   
    AlveoLink::common::FPGA l_card;

    
    unsigned int l_waitCycles = 200;
    unsigned int l_flushCounter = 1;
    commentInfo.useXnik = 1;

    //#ifdef XILINX_NHOP_DEBUG_ON
            std::cout << "DEBUG: " << __FUNCTION__ << std::endl;
            std::cout << " has_manager" << has_manager << std::endl;
            std::cout << "DEBUG: nhop command options:"
                    << "\n    xclbin_path=" << commentInfo.xclbin_path
                    << "\n    foldername=" << commentInfo.foldername
                    << "\n     num_hop =" << commentInfo.num_hop
                    << "\n     devId = "<< commentInfo.devId
                    << "\n     myId = " << commentInfo.myId
                    << "\n     hostId =" << commentInfo.hostId
                    << "\n     startId ="<< commentInfo.startId
                    << "\n     numIds ="<< commentInfo.numIds
                    << "\n     netfile ="<< commentInfo.netfile
                    //<< "\n     batchPkts = 128"<< commentInfo.batchPkts
                    //<< "\n     timeOutCycles ="<< commentInfo.timeOutCycles
                    << "\n     numKernel ="<< commentInfo.numKernel
                    << "\n     numPuPerKernel ="<< commentInfo.numPuPerKernel
                    << "\n     sz_bat ="<< commentInfo.sz_bat
                    << "\n     byPass ="<< commentInfo.byPass
                    << "\n     duplicate ="<< commentInfo.duplicate
                    << "\n     useXnik ="<< commentInfo.useXnik
                    << std::endl;
    //#endif

    l_card.setId(commentInfo.devId);
    l_card.load_xclbin(commentInfo.xclbin_path);

    

    // wait untill all links are up and ready
    AlveoLink::network_dv::dvNetLayer<AL_numInfs, AL_maxConnections, AL_destBits> l_dvNetLayer;
    l_dvNetLayer.init(&l_card);
    std::bitset<AL_numInfs> l_linkUp = l_dvNetLayer.getLinkStatus();
    while (!l_linkUp.all()) {
        std::cout << "INFO: wait until all QSFP links are ready!" << std::endl;
    }
    std::cout << "INFO: all links are up." << std::endl;
    uint16_t l_numDests = 0;
    uint16_t* l_ids = l_dvNetLayer.getIds();
    for (auto i = 0; i < AL_numInfs; ++i) {
        std::cout << "INFO: port " << i << " has id " << l_ids[i] << std::endl;
    }
    l_numDests = l_dvNetLayer.getNumDests();
    std::cout << "INFO: total number of available dests are " << l_numDests << std::endl;
    l_dvNetLayer.clearCounters();

    // for debug
    std::string filepath = commentInfo.foldername;
    
    std::string fn = filepath.substr(filepath.find_last_of('/') + 1);
    commentInfo.filename = fn.substr(0, fn.rfind(".")) + ".hop";  

    PartitionHop<unsigned> par1;
    par1.m_card = &l_card;
    double Limit_MB_v = 128;
    double Limit_MB_e = 128;
    for (int i = 0; i < commentInfo.numKernel; ++i) {
        if ((i >= commentInfo.startId) && (i < commentInfo.startId + commentInfo.numIds)) {
            commentInfo.myId = i;
            par1.initKernel(commentInfo, commentInfo.numKernel, commentInfo.numPuPerKernel, Limit_MB_v * 4 * (1 << 20),
                            Limit_MB_e * 4 * (1 << 20));
        }
    }

// start manager first
    AlveoLink::kernel::hwManagerHost<AL_maxConnections> l_manager;
    if(has_manager){
        l_manager.init(&l_card);
        l_manager.createCU(0);
        l_manager.setConfigBuf(commentInfo.numKernel+1, l_waitCycles, l_flushCounter, 0);
        l_manager.sendBO();
        l_manager.runCU();
        std::cout << "INFO: manager started..." << std::endl;

    }
    // start nhop
    IndexStatistic stt;
    timeInfo timeInfo;
    unsigned skipped;
    unsigned nv;
    unsigned ne;
    std::fstream infoFile;
    infoFile.open(filepath + "/partition_info.txt");
    infoFile >> skipped >> skipped >> nv >> ne;

    par1.LoadPair2Buff(nv, ne, commentInfo.num_hop, commentInfo, &timeInfo, &stt);

    if(has_manager){
        l_manager.finish();
        std::cout << "INFO: manager done." << std::endl;
    }

    par1.PrintRpt(commentInfo.num_hop, commentInfo, timeInfo, stt);

    //start debugging code //only used by c++
#ifdef DEBUG
        std::vector<uint64_t> l_pktTxCnts = l_dvNetLayer.getLaneTxPktsCnt();
        for (auto i=0; i<AL_numInfs; ++i) {
            std::cout << "INFO: port " << i << std::endl;
            for (auto j=0; j<4; ++j) {
                std::cout << "    lane " << j << " has transmitted " << l_pktTxCnts[i*4+j] << " packets." << std::endl;
            }
        }
        std::vector<uint64_t> l_pktCnts = l_dvNetLayer.getLaneRxPktsCnt();
        for (auto i=0; i<AL_numInfs; ++i) {
            std::cout << "INFO: port " << i << std::endl;
            for (auto j=0; j<4; ++j) {
                std::cout << "    lane " << j << " has received " << l_pktCnts[i*4+j] << " packets." << std::endl;
            }
        }
        for (auto i=0; i<AL_numInfs; ++i) {
            for (auto j=0; j<4; ++j) {
                std::vector<uint32_t> l_lastRxPkt = l_dvNetLayer.getLastTxPkt(i, j);
                std::cout << "INFO: last tx pkt for port " << i << " lane " << j << " is: " << std::endl;
                for (auto k=0; k<4; ++k) {
                    std::cout << "    w" << k << " = 0x" << std::hex << l_lastRxPkt[k];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        for (auto i=0; i<AL_numInfs; ++i) {
            for (auto j=0; j<4; ++j) {
                std::vector<uint32_t> l_lastRxPkt = l_dvNetLayer.getLastRxPkt(i, j);
                std::cout << "INFO: last rx pkt for port " << i << " lane " << j << " is: " << std::endl;
                for (auto k=0; k<4; ++k) {
                    std::cout << "    w" << k << " = 0x" << std::hex << l_lastRxPkt[k];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    // end of debugging code
#endif
    return 0;
}

//run merge
int runMerge(commentInfo& commentInfo) {
    std::string filepath = commentInfo.foldername;
    std::string fn = filepath.substr(filepath.find_last_of('/') + 1);
    commentInfo.filename = fn.substr(0, fn.rfind(".")) + ".hop";

    PartitionHop<unsigned> par1;

    par1.MergeResult(&commentInfo);

    return 0;
}

