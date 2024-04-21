#include "ttcm.h"
#include "utils.h"
#include <assert.h>
#include <complex.h>
#include <math.h>

void encode_ttcm(ctxParams *param, ctxSim *sim) {
    int k = param->k;
    int tail = param->tail;
    u8* x = sim->x;
    int xlen = param->S;
    void *nxs = param->next_state;
    void *nxo = param->next_out;
    int *intrlv = param->intrlv;
    float complex *map = param->map;
    float complex *tx = sim->tx;

    int K = 1 << k;

    u8(*next_state)[K] = nxs;
    u8(*next_out)[K] = nxo;

    u8 y[xlen+tail];
    u8 state1 = 0;
    u8 state2 = 0;
    for (int i = 0; i < xlen; i++) {
        u8 sym = x[i];
        u8 o1 = next_out[state1][sym];
        state1 = next_state[state1][sym];
        y[i] = o1;
    }
    for (int i = 0; i < xlen; i++) {
        int iv = intrlv[i];
        int sym = x[iv];
        u8 o2 = next_out[state2][sym];
        state2 = next_state[state2][sym];
        if (iv % 2 == 1)
            y[iv] = o2;
    }

    for (int i = 0; i < tail; i++) {
        u8 sym = 0;
        u8 o1 = next_out[state1][sym];
        state1 = next_state[state1][sym];
        u8 o2 = next_out[state2][sym];
        state2 = next_state[state2][sym];
        y[xlen + i] = (xlen + i) % 2 == 0 ? o1 : o2;
    }

    for (int i = 0; i < xlen + tail; i++) {
        u8 sym = y[i];
        tx[i] = map[sym];
    }
}

void decode_ttcm(ctxParams *param, ctxSim *sim) {
    int k = param->k;
    int m = param->m;
    void *nxs = param->next_state;
    void *nxo = param->next_out;
    void *prs = param->prev_state;
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
    u8(*next_state)[K] = nxs;
    u8(*next_out)[K] = nxo;
    u8(*prev_state)[K] = prs;

    float N0 = 2 * sigma*sigma;

    float gamma[S + tail][M][K];
    float alpha[S + tail][M];
    float beta[S + tail][M];
    //alpha(1,2:end)= -INFINITY;
    //% alpha(1,1)= 0;
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

    //    float temp0[K];
    //    float temp1[K];

    for (int si = 0; si < S + tail; si++) {
        float smax = -INFINITY;
        for (int ki = 0; ki < K; ki++) {
            int out0 = 0 + 2 * ki;
            float x0 = -pow(cabs(rx[si] - map[out0]), 2) / N0;
            int out1 = 1 + 2 * ki;
            float x1 = -pow(cabs(rx[si] - map[out1]), 2) / N0;
            float xmax = MAX(x0, x1);
            Va[si][ki] = xmax;
            smax = MAX(smax, xmax);
        }
        for (int ki = 0; ki < K; ki++) {
            Va[si][ki] -= smax;
            if (si % 2 == 0)
                Va[si][ki] = -k * log(2);
            //            printf("%f\t", Va[si][ki]);
        }
        //        printf("\n");
    }

    for (int it = 0; it < iters; it++) {
        for (int ubit = 1; ubit <= 2; ubit++) {
            for (int si = 0; si < S + tail; si++) {
                for (int mi = 0; mi < M; mi++) {
                    for (int ki = 0; ki < K; ki++) {
                        u8 output = next_out[mi][ki];
                        float complex x = ubit == 1 ? rx[si] : rxi[si];
                        float xx = pow(cabs(x - map[output]), 2);
                        //                        if (ubit == 1 && si % 2 == 1 || ubit == 2 && si % 2 == 0) for alpha
                        //                        if (ubit == 1 && si % 2 == 1 || ubit == 2 && si % 2 == 0)
                        //                            xx = 0;
                        gamma[si][mi][ki] = -xx / N0 + Va[si][ki];
                        //                        printf("%f\t", gamma[si][mi][ki]);
                    }
                    //                    printf("\n");
                }
            }


            for (int si = 1; si < S + tail; si++) {
                float alphanorm = -INFINITY;
                for (int mi = 0; mi < M; mi++) {
                    float alphamax = -INFINITY;
                    for (int ki = 0; ki < K; ki++) {
                        u8 prevState = prev_state[mi][ki];
                        float g = gamma[si - 1][prevState][ki];
                        if (ubit == 1 && si % 2 == 0 || ubit == 2 && si % 2 == 1)
                            g = Va[si - 1][ki];
                        alphamax = MAX(alphamax, alpha[si - 1][prevState] + g);
                    }
                    alpha[si][mi] = alphamax;
                    alphanorm = MAX(alphanorm, alpha[si][mi]);
                }
                for (int mi = 0; mi < M; mi++) {
                    alpha[si][mi] -= alphanorm;
                }
            }

            //            for (int si = 0; si < 10; si++) {
            //                for (int mi = 0; mi < M; mi++) {
            //                    printf("%f\t", alpha[si][mi]);
            //                }
            //                printf("\n");
            //            }

            for (int si = S + tail - 2; si >= 0; si--) {
                float alphanorm = -INFINITY;
                for (int mi = 0; mi < M; mi++) {
                    float betamax = -INFINITY;
                    for (int ki = 0; ki < K; ki++) {
                        u8 nextState = next_state[mi][ki];
                        float g = gamma[si + 1][mi][ki];
                        if (ubit == 1 && si % 2 == 0 || ubit == 2 && si % 2 == 1)
                            g = Va[si + 1][ki];
                        betamax = MAX(betamax, beta[si + 1][nextState] + g);
                    }
                    beta[si][mi] = betamax;
                    alphanorm = MAX(alphanorm, alpha[si + 1][mi]);
                }
                for (int mi = 0; mi < M; mi++) {
                    beta[si][mi] -= alphanorm;
                }
            }
            //            for (int si = S + tail - 1; si >= 121; si--) {
            //                for (int mi = 0; mi < M; mi++) {
            //                    printf("%f\t", beta[si][mi]);
            //                }
            //                printf("\n");
            //            }

            for (int si = 0; si < S + tail; si++) {
                float appnorm = -INFINITY;
                for (int ki = 0; ki < K; ki++) {
                    float appmax = -INFINITY;
                    for (int mi = 0; mi < M; mi++) {
                        int nextState = next_state[mi][ki];
                        float g = gamma[si][mi][ki];
                        if (ubit == 1 && si % 2 == 1 || ubit == 2 && si % 2 == 0)
                            g = Va[si][ki];
                        appmax = MAX(appmax, alpha[si][mi] + g
                                + beta[si][nextState]);
                    }
                    app[si][ki] = appmax;
                    appnorm = MAX(appnorm, appmax);
                }
                for (int ki = 0; ki < K; ki++) {
                    app[si][ki] -= appnorm;
                    Va[si][ki] = app[si][ki] - Va[si][ki];
                }
                //                Va_p[si] = V1 - V0 + 2 * m * log(2);
            }

            //            for (int si = 0; si < S + tail; si++) {
            //                for (int ki = 0; ki < K; ki++) {
            //                    printf("%f\t", app[si][ki]);
            //                }
            //                printf("\n");
            //            }
            if (ubit == 1) {
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        iVa[si][ki] = Va[intrlv[si]][ki];
                    }
                }
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        Va[si][ki] = iVa[si][ki];
                    }
                }
            } else {
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        iVa[intrlv[si]][ki] = Va[si][ki];
                    }
                }
                for (int si = 0; si < S; si++) {
                    for (int ki = 0; ki < K; ki++) {
                        Va[si][ki] = iVa[si][ki];
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

    //    float app2[S][K];
    //    for (int si = 0; si < S; si++) {
    //        for (int ki = 0; ki < K; ki++)
    //            app2[intrlv[si]][ki] = app[si][ki];
    //    }
    //    for (int si = 0; si < S; si++) {
    //        for (int ki = 0; ki < K; ki++) {
    //            printf("%f\t", app2[si][ki]);
    //        }
    //        printf("\n");
    //    }

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