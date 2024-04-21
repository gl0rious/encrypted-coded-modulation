#ifndef TTCM_H
#define TTCM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "utils.h"
#include <complex.h>

//    void encode_ttcm(int k, int tail, u8* x, int xlen, void *nxs, void *nxo, int *intrlv,
//        float complex *map, float complex *tx);
    void encode_ttcm(ctxParams *param, ctxSim *sim);
//    void decode_ttcm(int k, int m, void *nxs, void *nxo, void *prs, float complex *rx, int S, int tail,
//        int *intrlv, float complex *map, float sigma, int iters, u8 *x, u8 *xh, int *bers);
        void decode_ttcm(ctxParams *param, ctxSim *sim);


#ifdef __cplusplus
}
#endif

#endif /* TTCM_H */

