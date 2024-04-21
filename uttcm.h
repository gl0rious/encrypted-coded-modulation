#ifndef UTTCM_H
#define UTTCM_H

#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include <complex.h>

    void encode_uttcm(ctxParams *param, ctxSim *sim);
    void decode_uttcm(ctxParams *param, ctxSim *sim);

#ifdef __cplusplus
}
#endif

#endif /* UTTCM_H */

