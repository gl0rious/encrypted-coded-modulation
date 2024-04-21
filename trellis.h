#ifndef TRELLIS_H
#define TRELLIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "utils.h"
    
void init_trellis(int k, int m, int clen, u8 *coef,
        void *nxs, void *prs, void *nxo, void *pro);


#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_H */

