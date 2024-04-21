#include <stdlib.h>
#include "utils.h"
#include "contex.h"
#include "trellis.h"

void init_param_ctx(ctxParams *param) {
    param->datalen = param->frmlen / 8;
    param->K = 1 << param->k; // 2^k
    param->N = 1 << param->n;

    param->S = param->frmlen / param->k;
    param->frmcount = param->simlen / param->frmlen;

    param->tail = 2;
    param->M = 1 << param->m; // 2^m

    assert((param->K*param->M)<=sizeof(param->next_state));
    
    init_trellis(param->k, param->m, param->coeflen, 
            param->coef, param->next_state, param->prev_state, 
            param->next_out, param->prev_out);
    
     
    assert((param->S+param->tail) <= CTXLEN);
    assert(param->N<=sizeof(param->map));
}

void init_sim_ctx(ctxParams *param, ctxSim *sim){    
    assert((param->S+param->tail) <= CTXLEN);
    assert(param->datalen <= CTXLEN);
    
}