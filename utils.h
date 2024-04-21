#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <complex.h>
#include "contex.h"
    
    // defines
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define LEN(X) (sizeof(X)/sizeof(X[0]))
#define BIT(X,I) (((X)>>(I))&1)
#define DISP_I(X,I,J) printf("%s = [\n",#X);\
    for(int i=0; i<I; i++){\
        for(int j=0; j<J; j++){ \
            printf("\t%i",X[i][j]);\
        }\
        printf("\n");\
    };\
    printf("]\n");
#define DISPV_I(X,I) printf("%s = [\n",#X);\
    for(int i=0; i<I; i++){\
        printf("\t%i\n",X[i]);\
    };\
    printf("]\n");

#define DISP_F(X,I,J) printf("%s = [\n",#X);\
    for(int i=0; i<I; i++){\
        for(int j=0; j<J; j++){ \
            printf("\t%f",X[i][j]);\
        }\
        printf("\n");\
    };\
    printf("]\n");
#define DISPV_F(X,I) printf("%s = [\n",#X);\
    for(int i=0; i<I; i++){\
        printf("\t%f\n",X[i]);\
    };\
    printf("]\n");

#define DISP_C(X,I,J) printf("%s = [\n",#X);\
    for(int i=0; i<I; i++){\
        for(int j=0; j<J; j++){ \
            printf("\t%f + %fi",creal(X[i][j]), cimag(X[i][j]));\
        }\
        printf("\n");\
    };\
    printf("]\n");
#define DISPV_C(X,I) printf("%s = [\n",#X);\
    for(int i=0; i<I; i++){\
        printf("\t%f + %fi\n",creal(X[i]), cimag(X[i]));\
    };\
    printf("]\n");


    
    // types
    typedef uint8_t u8;
    typedef uint32_t u32;

    void rand_bytes(ctxParams *param, ctxSim *sim);
    void XOR(u8* a, u8 *b, u8* c, int len);
    void unpack_bytes(u8* bytes, u8* syms, int k, int bytelen);
    void pack_bytes(u8* bytes, u8* syms, int k, int bytelen);
    void rand_input(ctxParams* param, ctxSim* sim);
    void add_noise(ctxParams* param, ctxSim* sim);
    int biterr(u8* x, u8* y, int len);
    int frmerr(u8* x, u8* y, int len);
    int init_EbN0(float from, float step, float to, float *EbN0list);
    float EbN0_to_sigma(float EbN0, int k);
    void copy(u8 *from, u8* to, int len);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */

