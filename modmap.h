#ifndef MODMAP_H
#define MODMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <complex.h>
#include "contex.h"

    enum map_type {
        PSK8,
        QAM16,
        CIRC16,
        CIRC16_2,
        ROTATED_8_8,
        ROTATED_8_8_2
    };
    
void init_map(enum map_type type, ctxParams* param);

#ifdef __cplusplus
}
#endif

#endif /* MODMAP_H */

