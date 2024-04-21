#include "uttcm.h"
#include "utils.h"
#include <assert.h>
#include <complex.h>
#define __USE_MISC
#include <math.h>

void encode_uttcm(ctxParams *param, ctxSim *sim) {
    int k = param->k;
    int tail = param->tail;
    u8* x = sim->x;
    int S = param->S;
    int *intrlv = param->intrlv;
    float complex *map = param->map;
    float complex *tx = sim->tx;

    int K = 1 << k;
    u8(*next_state)[K] = (void*)param->next_state;
    u8(*next_out)[K] = (void*)param->next_out;

    u8 y[S];
    u8 state1 = 0;
    u8 state2 = 0;
    for (int i = 0; i < S; i++) {
        u8 sym = x[i];
        u8 o1 = next_out[state1][sym];
        state1 = next_state[state1][sym];
        y[i] = o1 << 1;
    }
    for (int i = 0; i < S; i++) {
        int iv = intrlv[i];
        u8 sym = x[iv];
        u8 o2 = next_out[state2][sym];
        state2 = next_state[state2][sym];
        y[iv] |= o2 & 1;
    }
    for (int i = 0; i < tail; i++) {
        u8 sym = 0;
        u8 o1 = next_out[state1][sym];
        state1 = next_state[state1][sym];
        u8 o2 = next_out[state2][sym];
        state2 = next_state[state2][sym];
        y[S + i] = o1 << 1 | o2 & 1;
    }
    for (int i = 0; i < S + tail; i++) {
        u8 sym = y[i];
        tx[i] = map[sym];
    }
}

void decode_uttcm(ctxParams *param, ctxSim *sim) {
    int k = param->k;
    int m = param->m;
//    void *nxs = param->next_state;
//    void *nxo = param->next_out;
//    void *prs = param->prev_state;
    float complex *rx = sim->rx;
    int S = param->S;
    int tail = param->tail;
    int *intrlv = param->intrlv;
    float complex *map = param->map;
    float sigma = param->sigma;
    int iters = param->iters;
    u8 *x = sim->x;
    u8 *xh = sim->xh;
    int *bers = sim->bers;

    int K = 1 << k;
    int M = 1 << m;
    u8(*next_state)[K] = (void*)param->next_state;
    u8(*next_out)[K] = (void*)param->next_out;
    u8(*prev_state)[K] = (void*)param->prev_state;

    float N0 = 2 * sigma*sigma;

    float gamma[S + tail][M][K];
    float alpha[S + tail][M];
    float beta[S + tail][M];

    for (int mi = 0; mi < M; mi++) {
        alpha[0][mi] = -INFINITY;
        beta[S + tail - 1][mi] = 0;
    }
    alpha[0][0] = 0;

    float complex rxi[S + tail];
    for (int si = 0; si < S; si++) {
        rxi[si] = rx[intrlv[si]];
    }
    for (int si = S; si < S + tail; si++) {
        rxi[si] = rx[si];
    }

    float app[S + tail][K];
    float Va[S + tail][K];
    float iVa[S + tail][K];
    float Va_p[S + tail];
    float iVa_p[S + tail];

    float temp0[K];
    float temp1[K];

    float log_k = -log(K);
    float log_2_pi = log(2 * M_PI);
    float log_sigma = log(sigma);
    float log_2 = log(2);
    
    for (int si = 0; si < S + tail; si++)
        for (int ki = 0; ki < K; ki++)
            Va[si][ki] = log_k;

    for (int it = 0; it < iters; it++) {
        for (int ubit = 1; ubit <= 2; ubit++) {
            for (int si = 0; si < S + tail; si++) {
                for (int mi = 0; mi < M; mi++) {
                    for (int ki = 0; ki < K; ki++) {
                        u8 output = next_out[mi][ki];
                        u8 o1 = (ki << 2) | (output & 1) << (2 - ubit);
                        u8 o2 = o1 + ubit;
                        float complex x = ubit == 1 ? rx[si] : rxi[si];
                        float x1 = -pow(cabs(x - map[o1]), 2) / N0
                                - log_sigma - .5 * log_2_pi - Va_p[si] / 2;
                        float x2 = -pow(cabs(x - map[o2]), 2) / N0
                                - log_sigma - .5 * log_2_pi + Va_p[si] / 2;
                        gamma[si][mi][ki] = MAX(x1, x2) + Va[si][ki];
                    }
                }
            }

            for (int si = 1; si < S + tail; si++) {
                float alphanorm = -INFINITY;
                for (int mi = 0; mi < M; mi++) {
                    float alphamax = -INFINITY;
                    for (int ki = 0; ki < K; ki++) {
                        u8 prevState = prev_state[mi][ki];
                        alphamax = MAX(alphamax, alpha[si - 1][prevState] +
                                gamma[si - 1][prevState][ki]);
                    }
                    alpha[si][mi] = alphamax - 2 * m * log_2;
                    alphanorm = MAX(alphanorm, alpha[si][mi]);
                }
                for (int mi = 0; mi < M; mi++) {
                    alpha[si][mi] -= alphanorm;
                }
            }

            for (int si = S + tail - 2; si >= 0; si--) {
                float alphanorm = -INFINITY;
                for (int mi = 0; mi < M; mi++) {
                    float betamax = -INFINITY;
                    for (int ki = 0; ki < K; ki++) {
                        u8 nextState = next_state[mi][ki];
                        betamax = MAX(betamax, beta[si + 1][nextState] +
                                gamma[si + 1][mi][ki]);
                    }
                    beta[si][mi] = betamax - 2 * m * log_2;
                    alphanorm = MAX(alphanorm, alpha[si + 1][mi]);
                }
                for (int mi = 0; mi < M; mi++) {
                    beta[si][mi] -= alphanorm;
                }
            }

            for (int si = 0; si < S + tail; si++) {
                float appnorm = -INFINITY;
                float gama0max = -INFINITY;
                float gama1max = -INFINITY;

                for (int ki = 0; ki < K; ki++) {
                    float appmax = -INFINITY;
                    temp0[ki] = -INFINITY;
                    temp1[ki] = -INFINITY;
                    for (int mi = 0; mi < M; mi++) {
                        int nextState = next_state[mi][ki];
                        float gam = gamma[si][mi][ki] - 2 * m * log_2;
                        appmax = MAX(appmax, alpha[si][mi] + gam
                                + beta[si][nextState]);
                        u8 output = next_out[mi][ki]&1;
                        if (output == 0) {
                            temp0[ki] = MAX(temp0[ki], gam);
                        } else {
                            temp1[ki] = MAX(temp1[ki], gam);
                        }
                    }
                    app[si][ki] = appmax;
                }
                appnorm = app[si][0];
                for (int ki = 0; ki < K; ki++) {
                    app[si][ki] -= appnorm;
                    Va[si][ki] = app[si][ki] - Va[si][ki];
                }
                for (int ki = 0; ki < K; ki++) {
                    gama0max = MAX(gama0max, temp0[ki] + app[si][ki]);
                    gama1max = MAX(gama1max, temp1[ki] + app[si][ki]);
                }
                float V0 = gama0max;
                float V1 = gama1max;
                Va_p[si] = V1 - V0 + 2 * m * log_2;
            }
            if (ubit == 1) {
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        iVa[si][ki] = Va[intrlv[si]][ki];
                        iVa_p[si] = Va_p[intrlv[si]];
                    }
                }
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        Va[si][ki] = iVa[si][ki];
                        Va_p[si] = iVa_p[si];
                    }
                }
            } else {
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        iVa[intrlv[si]][ki] = Va[si][ki];
                        iVa_p[intrlv[si]] = Va_p[si];
                    }
                }
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        Va[si][ki] = iVa[si][ki];
                        Va_p[si] = iVa_p[si];
                    }
                }
            }
        }
        for (int si = 0; si < S; si++) {
            int sym = 0;
            float symmax = -INFINITY;
            for (int ki = 0; ki < K; ki++) {
                if (app[si][ki] > symmax) {
                    symmax = app[si][ki];
                    sym = ki;
                }
            }
            xh[intrlv[si]] = sym;
        }
        bers[it] += biterr(x, xh, S);
    }
    for (int si = 0; si < S; si++) {
        int sym = 0;
        float symmax = -INFINITY;
        for (int ki = 0; ki < K; ki++) {
            if (app[si][ki] > symmax) {
                symmax = app[si][ki];
                sym = ki;
            }
        }
        xh[intrlv[si]] = sym;
    }
}