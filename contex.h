#ifndef CONTEX_H
#define CONTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <complex.h> 

    #define CTXLEN 200
    #define TRSLEN 64
    typedef uint8_t u8;
    typedef uint32_t u32;

    typedef struct _ctxParams ctxParams;
    typedef struct _ctxSim ctxSim;

    typedef struct _ctxParams{
        char name[24];
        char map_name[24];
//        char* name;
//        char* map_name;
        int use_encrypt;
        int imgcount;
        int simlen;
        int frmlen;
        int datalen;
        int iters;
        int k;
        int K;
        int n;
        int m;
        u8 coef[10];
        int coeflen;

        int N;
        int S;
        int frmcount;
        int tail;
        int M;

        u8 next_state[TRSLEN];
        u8 prev_state[TRSLEN];;
        u8 next_out[TRSLEN];;
        u8 prev_out[TRSLEN];;

//        int intrlv_seed;
        int intrlv[CTXLEN];
        float complex map[64];
        float sigma;

        void (*encode)(ctxParams *param, ctxSim *sim);
        void (*decode)(ctxParams *param, ctxSim *sim);
    } ctxParams;

    void init_param_ctx(ctxParams *param);

    typedef struct _ctxSim{
        unsigned int data_seed;
        unsigned int noise_seed;
        u8 x[CTXLEN];
        u8 xh[CTXLEN];
        u8 y[CTXLEN];
        float complex tx[CTXLEN];
        float complex rx[CTXLEN];
        u8 data[CTXLEN];
        u8 data_iv[CTXLEN];
        u8 data_c[CTXLEN];
        u8 data_d[CTXLEN];
        u8 datah[CTXLEN];
        int bers[10];
    } ctxSim;

    void init_sim_ctx(ctxParams *param, ctxSim *sim);

#ifdef __cplusplus
}
#endif

#endif /* CONTEX_H */

