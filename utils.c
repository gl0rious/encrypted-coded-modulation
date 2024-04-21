#define _GNU_SOURCE

#include "utils.h"
#include <math.h>
#include "contex.h"
#include <stdlib.h>

//extern int rand_r (unsigned int *__seed) __THROW;

void rand_bytes(ctxParams *param, ctxSim *sim) {
    for (int i = 0; i < param->datalen; i++) {
        sim->data[i] = rand_r(&sim->data_seed) % 256;
//        sim->data[i] = rand() % 256;
    }
}

void rand_input(ctxParams *param, ctxSim *sim){ //u8 *x, int len, int K, unsigned int* seed) {
    for (int i = 0; i < param->datalen; i++)
        sim->x[i] = rand_r(&sim->data_seed) % param->K;
//        sim->x[i] = rand() % param->K;
}

void XOR(u8* a, u8 *b, u8* c, int len) {
    for (int i = 0; i < len; i++)
        c[i] = a[i]^b[i];
}

// unpack encrypted bytes to  k symbols

void unpack_bytes(u8* bytes, u8* syms, int k, int bytelen) {
    assert(8 % k == 0);
    int numSym = 8 / k;
    int mask = (1 << k) - 1;
    for (int i = 0; i < bytelen; i++) {
        for (int s = 0; s < numSym; s++) {
            syms[i * numSym + s] = (bytes[i] >> (k * s)) & mask;
        }
    }
}

void pack_bytes(u8* bytes, u8* syms, int k, int bytelen) {
    assert(8 % k == 0);
    int numSym = 8 / k;
    int mask = (1 << k) - 1;
    for (int i = 0; i < bytelen; i++) {
        int tmp = 0;
        for (int s = 0; s < numSym; s++) {
            tmp |= syms[i * numSym + s] << (k * s);
        }
        bytes[i] = tmp;
    }
}

float rand_gauss(ctxSim* sim) {
    float v1, v2, s;

    do {
        v1 = 2.0 * ((float) rand_r(&sim->noise_seed) / (float) RAND_MAX) - 1;
        v2 = 2.0 * ((float) rand_r(&sim->noise_seed) / (float) RAND_MAX) - 1;
//        v1 = 2.0 * ((float) rand() / (float) RAND_MAX) - 1;
//        v2 = 2.0 * ((float) rand() / (float) RAND_MAX) - 1;

        s = v1 * v1 + v2*v2;
    } while (s >= 1.0);

    if (s == 0.0)
        return 0.0;
    else
        return (v1 * sqrt(-2.0 * log(s) / s));
}

void add_noise(ctxParams* param, ctxSim* sim){
    for (int i = 0; i < (param->S + param->tail); i++) {
        sim->rx[i] = sim->tx[i] + 
                param->sigma * (rand_gauss(sim) + 
                rand_gauss(sim) * I);
    }
}

//float rand_gauss() {
//    float u = (float) rand() / RAND_MAX;
//    float v = (float) rand() / RAND_MAX;
//    float n1 = sqrt(-2.0 * log(u)) * cos(2 * M_PI * v);
////    n2 = sqrt(-2.0 * log(u)) * sin(2 * M_PI * v);
//    return n1;
//}

int biterr(u8* x, u8* y, int len) {
    int errcount = 0;
    for (int i = 0; i < len; i++) {
        u8 diff = x[i]^y[i];
        while (diff) {
            errcount += diff & 1;
            diff = diff >> 1;
        }
    }
    return errcount;
}

int frmerr(u8* x, u8* y, int len) {
    for (int i = 0; i < len; i++) {
        if(x[i]!=y[i])
            return 1;
    }
    return 0;
}

int init_EbN0(float from, float step, float to, float *EbN0list) {
    float val = from;
    int EbN0count = 0;
    while (val <= to && EbN0count < 100) {
        EbN0list[EbN0count] = val;
        val += step;
        EbN0count++;
    }
    return EbN0count;
}

float EbN0_to_sigma(float EbN0, int k) {
    float snrdb = EbN0 + 10 * log10(k);
    float N0 = pow(10, -snrdb / 10);
    float sigma = sqrt(N0 / 2);
    return sigma;
}

void copy(u8 *from, u8* to, int len){
    for(int i = 0; i < len; i++)
        to[i] = from[i];
}