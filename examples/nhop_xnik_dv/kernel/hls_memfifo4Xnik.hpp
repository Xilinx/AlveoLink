#ifndef _HLS_MEMFIFO_H_
#define _HLS_MEMFIFO_H_
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

/////////////////////////////////////////////////////////////////
//#define _DEBUG_MEMFIFO_

enum ADDRS_SF { ADDR_SF_R = -2, ADDR_SF_W = -1 };

enum STATE_SF { SFST_CHECK = 0, SFST_WAIT = 1, SFST_WORK = 2 };

template <int W>
void memfifo_getStatus(ap_uint<W>* pBuff, int& p_write, int& p_read) {
#pragma HLS INLINE off
    ap_uint<W> u_r = pBuff[ADDR_SF_R];
    ap_uint<W> u_w = pBuff[ADDR_SF_W];
    p_write = u_w;
    p_read = u_r;
}

template <int W>
int memfifo_getStatus_push(ap_uint<W>* pBuff, int userSize, int& p_write, int& p_read) {
#pragma HLS INLINE
    memfifo_getStatus(pBuff, p_write, p_read);
    int num_space = p_write - p_read;
    return (num_space >= 0) ? (userSize - num_space - 1) : -(num_space + 1);
}

template <int W>
int memfifo_getStatus_pop(ap_uint<W>* pBuff, int userSize, int& p_write, int& p_read) {
#pragma HLS INLINE
    memfifo_getStatus(pBuff, p_write, p_read);
    int num_valid = p_write - p_read;
    return (num_valid >= 0) ? num_valid : userSize + num_valid;
}

template <int W>
void memfifo_syncStatus_w(ap_uint<W>* pBuff, int p_write) {
#pragma HLS INLINE
    ap_uint<W> u_w = p_write;
    pBuff[ADDR_SF_W] = u_w;
}

template <int W>
void memfifo_syncStatus_r(ap_uint<W>* pBuff, int p_read) {
#pragma HLS INLINE
    ap_uint<W> u_r = p_read;
    pBuff[ADDR_SF_R] = u_r;
}
template <int W>
void memfifo_syncStatus(ap_uint<W>* pBuff, int p_write, int p_read) {
    memfifo_syncStatus_w(pBuff, p_write);
    memfifo_syncStatus_r(pBuff, p_read);
}

////////////////////////////////////////////////////////////////
// Begin: Watching Dog
template <int W, class T> // usually 32-bit is not enough as it can only count tens of sceonds when clock is 300MHz
class WatchDog {
   public:
    T t1, t2, t3;
    T cy1, cy2, cy3;

    WatchDog(T c1, T c2, T c3) {
        cy1 = c1;
        cy2 = c2;
        cy3 = c3;
        rst_All();
    }
    int clock() { // return 0 measns no carryin; 1 means t-1 carryin
        int ret = 0;
        if (t1 < cy1 - 1)
            t1++;
        else {
            ret = 1;
            t1 = 0;
            if (t2 < cy2 - 1)
                t2++;
            else {
                ret = 2;
                t2 = 0;
                if (t3 < cy3 - 1)
                    t3++;
                else {
                    ret = 3;
                    t3 = 0;
                }
            }
        }
        return ret;
    }
    void rst_All() { t1 = t2 = t3 = 0; }
    void rst_t2() { t2 = t3 = 0; }
    void rst_t3() { t3 = 0; }
    bool isCy_1() { return (t1 == cy1 - 1); }
    bool isCy_2() { return (t2 == cy2 - 1) && (t1 == cy1 - 1); }
    bool isCy_3() { return (t3 == cy3 - 1) && (t2 == cy2 - 1) && (t1 == cy1 - 1); }
    int isCy() {
        if (isCy_3())
            return 3;
        else if (isCy_2())
            return 2;
        else if (isCy_1())
            return 1;
        else
            return 0;
    }
    void printSelf(const char* head, int id) {
#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
        printf("WDG_%s(%d) t1(%d, %d), t2(%d, %d), t3(%d, %d)\n", head, id, t1, cy1, t2, cy2, t3, cy3);
#endif
#endif
    }
    void printSelf() {
        const char* head = "";
        printSelf(head, 0);
    }
};
// END: Watching Dog
////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// Begin: functions about axi bus data operation
enum TYPE_BUSDATA { DATA_WORKLOAD = 0, DATA_STATUS_FULL, DATA_STATUS_LIVE, DATA_STATUS_OVER };

template <int W>
unsigned char dataType(ap_uint<W> data) { // 0: workload //>0 : status
    unsigned char ret = data(23, 20);     // just is TYPE_BUSDATA
    return ret;
}

template <int W>
ap_uint<W> dataGen(ap_uint<16> id_broadcast, ap_uint<16> id_src, unsigned char type
                   // ap_uint<W> code_full
                   ) {
    ap_uint<W> ret = 0;
    ret(15, 0) = id_broadcast;
    ret(19, 16) = 0; // len_bus
    ret(23, 20) = type;
    if (W > 32) ret(47, 32) = id_src;
    return ret;
}

template <int W>
bool isNotEndCode(ap_uint<W> data) {
    return dataType(data) != DATA_STATUS_OVER;
}

template <class T>
void showStatus(T data) {
    unsigned char type = dataType(data);
#ifndef __SYNTHESIS__
    if (type == DATA_WORKLOAD) {
        printf("DATA_WORKLOAD\n");
    } else if (type == DATA_STATUS_FULL) {
        printf("DATA_STATUS_FULL\n");
    } else if (type == DATA_STATUS_LIVE) {
        printf("DATA_STATUS_LIVE\n");
    } else if (type == DATA_STATUS_OVER) {
        printf("DATA_STATUS_OVER\n");
    } else {
        printf("ERROR, NOT Defined status code\n");
    }
#endif
}

// End: functions about axi bus data operation

template <int W_AXI, int W_TOKEN>
void watchInStrm(ap_uint<16> id_broadcast,
                 ap_uint<16> id_self,
                 ap_uint<W_TOKEN> blen_write, // how many data can send a token for next bust pushing
                 int cycle2sleep,
                 int times_sleep2over,
                 hls::stream<ap_uint<W_AXI> >& strm_in,
                 hls::stream<ap_uint<W_AXI> >& strm_out_wdg, // contains ending code
                 hls::stream<ap_uint<W_TOKEN> >& strm_out_token,
                 hls::stream<bool>& strm_out_status_e,
                 hls::stream<ap_uint<W_AXI> >& strm_out_status

                 ) {
    int cnt_data = 0;
Loop_WDG:
    do {
#pragma HLS pipeline

        // case-1: input is empty
        // case-2: input is workload
        // sub-case-2-1: output stream not full
        // sub-case-2-1-1: cnt_data not meet
        // sub-case-2-1-2: cnt_data meets and write token
        // sub-case-2-2: output stream is full
        // case-3: input is a status report package
        if (strm_in.empty()) {
// case-1: empty, just counting
                if (cnt_data != 0) {
                    strm_out_token.write(cnt_data);
                    cnt_data = 0;
                }
        } else {
            // strm_in not empty
            if (!strm_out_wdg.full()) {
                ap_uint<W_AXI> data = strm_in.read();
                if (DATA_WORKLOAD == dataType(data)) {
                    // case-2: workload, feed(reset) watch-dog; sending status if necessary
                    strm_out_wdg.write(data);
                    if (cnt_data < blen_write - 1) {
                        cnt_data++;
                    } else {
                        strm_out_token.write(blen_write);
                        cnt_data = 0;
                    }
                } else { // case-3: data type is STATUS, just counting but not to over
                    if (DATA_STATUS_OVER == dataType(data)) {
                        strm_out_token.write(cnt_data);
                        cnt_data = 0;
                        break;
                    }
                }
            } else {
                // fifo full, do nothing
            }
        }
    } while (1);
    ap_uint<W_AXI> data = dataGen<W_AXI>(id_broadcast, id_self, DATA_STATUS_OVER);
    strm_out_wdg.write(data);
    strm_out_token.write(1);
    strm_out_status.write(0);
    strm_out_status_e.write(1);
}

template <int W, int W_TOKEN>
void pushToMem(hls::stream<ap_uint<W_TOKEN> >& strm_token,
               hls::stream<ap_uint<W> >& strm_in,
               ap_uint<W>* p_buff,
               int userSize) {
    int cnt = 0;
    ap_uint<W> data;
    int p_write;
    int p_read;
    int num_space;
    int blen;
    bool metEndDode = false;
#ifndef __SYNTHESIS__
    int cyc_loop = 0;
    int cnt_empty = 0;
    int cnt_check = 0;
    int cnt_push = 0;
    int code = 0;
#endif
Loop_PUSH:
    do {
#ifdef _DEBUG_MEMFIFO_
        std::cout << "tokenSize=" << strm_token.size() << " dataSize=" << strm_in.size() << std::endl;
#endif
        if (!strm_token.empty()) {
            blen = strm_token.read();
            if (blen) {
            // Memory operation
            Loop_PUSH_WAIT_BUFF:
                do {
                    num_space = memfifo_getStatus_push(p_buff, userSize, p_write, p_read);
                    if (num_space < blen) {
                    loop_wait:
                        for (int wait = 0; wait < 128; wait++) {
                            // wait;
                        }
                    }
#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
                    printf("PUSH SYNC_OUTFULL cyc=%ld empty=%ld cnt_chck=%d: num_space=%ld, pw=%ld, pr=%ld, blen=%d\n",
                           cyc_loop++, cnt_empty, cnt_check++, num_space, p_write, p_read, blen);
#endif
#endif
                } while (num_space < blen);
            Loop_PUSH_BLEN:
                for (int i = 0; i < blen; i++) {
#pragma HLS PIPELINE
#ifndef __SYNTHESIS__
                    assert(p_write < userSize);
                    assert(p_write >= 0);
#endif

                    data = strm_in.read();
                    p_buff[p_write] = data;
                    p_write = p_write == userSize - 1 ? 0 : p_write + 1;
                    if (!isNotEndCode(data)) metEndDode = true;

#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
                    cyc_loop++;
                    int code = data(31, 24);
                    printf(
                        "PUSH SYNC__WRITE cyc=%ld empty=%ld cnt_push=%d: num_space=%ld, pw=%ld, pr=%ld, blen=%d:: "
                        "code=%d  "
                        " metEndDode=%d\n",
                        cyc_loop++, cnt_empty, cnt_push++, num_space, p_write, p_read, blen, code, metEndDode);
#endif
#endif
                }
                memfifo_syncStatus_w(p_buff, p_write);
            }
        }
    } while (!metEndDode);

#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
    printf("SUM: PUSH SYNC cyc=%ld empty=%ld cnt_chck=%d: num_space=%ld, pw=%ld, pr=%ld, blen=%d\n", cyc_loop++,
           cnt_empty, cnt_check++, num_space, p_write, p_read, blen);
#endif
#endif
}

template <int W>
void popFromMem(int pop_blen, // suggested burst reading lenght, it not met, then waiting pop_ii_check cycles and check
                              // again, it check times met pop_max_check then read anyway
                int pop_ii_check,  // watint cycles for next checking
                int pop_max_check, // bigest checking times when pop_blen not met
                hls::stream<bool>& strm_out_e,
                hls::stream<ap_uint<W> >& strm_out,
                ap_uint<W>* p_buff,
                int userSize) {
    int cnt = 0;
    ap_uint<W> data;
    int p_write = 0;
    int p_read = 0;
    int num_valid = 0;
    int cnt_check = 0;
    int cnt_wait = 0;

    STATE_SF state;
    state = SFST_CHECK;
    bool metEndDode = false;
#ifndef __SYNTHESIS__
    int cyc_loop = 0;
#endif
Loop_POP:
    do {
        if (state == SFST_CHECK) {
            num_valid = memfifo_getStatus_pop(p_buff, userSize, p_write, p_read);
#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
            printf("POP SYNC0_CHCK cyc=%ld state=%d num_valid=%ld, pw=%ld, pr=%ld, cnt_check=%d, pop_check=%d\n",
                   cyc_loop++, state, num_valid, p_write, p_read, cnt_check, pop_max_check);
#endif
#endif
            if (num_valid >= pop_blen) {
                state = SFST_WORK;
                cnt_check = 0;
            } else { // num_valid <pop_blen
                if (cnt_check < pop_max_check) {
                    state = SFST_WAIT;
                    cnt_check++;
                } else {
                    if (num_valid > 0) {
                        state = SFST_WORK;
                        cnt_check = 0;
                    } else { // cnt_check > pop_max_check && num_valid==0
                        state = SFST_WAIT;
                        cnt_check = 0;
                    }
                }
            }
        } else if (state == SFST_WAIT) {
#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
            printf(
                "POP SYNC1_WAIT cyc=%ld state=%d num_valid=%ld, pw=%ld, pr=%ld, cnt_check=%d, cnt_wait=%d, "
                "ii_check=%d\n",
                cyc_loop++, state, num_valid, p_write, p_read, cnt_check, cnt_wait, pop_ii_check);
#endif
#endif
            if (cnt_wait < pop_ii_check) {
                state = SFST_WAIT;
                cnt_wait++;
            } else {
                state = SFST_CHECK;
                cnt_wait = 0;
            }
        } else if (state == SFST_WORK) {
#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
            printf("POP SYNC2_WORK cyc=%ld state=%d num_valid=%ld, pw=%ld, pr=%ld, cnt_check=%d\n", cyc_loop++, state,
                   num_valid, p_write, p_read, cnt_check);
            cyc_loop += num_valid;
#endif
#endif
            if (p_read + num_valid >= userSize) { // have to rollback
                int len_bst = userSize - p_read;
            Loop_POP_READ1:
                for (int i = 0; i < len_bst; i++) {
#pragma HLS PIPELINE II = 1
                    data = p_buff[p_read++];
                    if (isNotEndCode(data)) {
                        strm_out.write(data);
                        strm_out_e.write(0);
                    } else
                        metEndDode = true; // this should be impossible otherwise it's a error
                }
                p_read = 0;
                len_bst = p_write - p_read;
            Loop_POP_READ2:
                for (int i = 0; i < len_bst; i++) {
#pragma HLS PIPELINE II = 1
                    data = p_buff[p_read++];
                    if (isNotEndCode(data)) {
                        strm_out.write(data);
                        strm_out_e.write(0);
                    } else
                        metEndDode = true;
                }
            } else {
                int len_bst = num_valid;
            Loop_POP_READ3:
                for (int i = 0; i < len_bst; i++) {
#pragma HLS PIPELINE II = 1
                    data = p_buff[p_read++];
                    if (isNotEndCode(data)) {
                        strm_out.write(data);
                        strm_out_e.write(0);
                    } else
                        metEndDode = true;
                }
            }
            state = SFST_CHECK;
            memfifo_syncStatus_r(p_buff, p_read);
        }
    } while (!metEndDode);
    strm_out.write(0);
    strm_out_e.write(1);
#ifndef __SYNTHESIS__
#ifdef _DEBUG_MEMFIFO_
    printf("SUM: POP SYNC1_WAIT cyc=%ld state=%d num_valid=%ld, pw=%ld, pr=%ld, cnt_check=%d\n", cyc_loop++, state,
           num_valid, p_write, p_read, cnt_check);
#endif
#endif
}

void hls_memfifo(ap_uint<512>* buff1,
                 ap_uint<512>* buff2,
                 // for both push and popwhich
                 int userSize, // (HBM-PC)/ sizeof(ap_uint<512>) - 2
                 // only for watch dog
                 int cycle2sleep,      // try large number firstly eg. 1,000,000(4ms) -> 1ms
                 int times_sleep2over, // try large number firstly eg. 32 (4ms *32 = 128ms )
                 // only for pop
                 int pop_blen_read, // suggested burst reading lenght, it not met, then waiting pop_ii_check cycles and
                                    // check again, it check times met pop_max_check then read anyway
                 // compariable number of batch size, eg. size_batch/4  but should be small than batch size
                 int pop_ii_check,  // watint cycles for next checking . eg. size_batch * 4
                 int pop_max_check, // biggest checking times when pop_blen_read not met. It should not be too big to
                                    // avoid long stay-time. eg.  1~2
                 ap_uint<16> id_broadcast, // ID for reports need to be broadcasted. There might be a post-processing
                                           // module to translate the ID to exact IDs according to ID table
                 ap_uint<16> id_self,      // ID of current kernel
                 ap_uint<8> blen_write,    // burst length for pushing data into memory-fifo

                 hls::stream<ap_uint<512> >& strm_in, // input stream
                 hls::stream<ap_uint<512> >& strm_out,         // output data
                 hls::stream<bool>& strm_out_e,                // output end singnal
                 hls::stream<ap_uint<512> >& strm_out_status,  // output of status report in which can be heart-beating,
                                                               // kernel-over or error reports
                 hls::stream<bool>& strm_out_status_e          // output end singnal for status
                 );
#endif
