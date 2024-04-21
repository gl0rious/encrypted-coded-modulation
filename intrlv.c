#include "contex.h"
#include "intrlv.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "utils.h"

void init_interleaver_uttcm(int *intrlv, int len, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < len; i++)
        intrlv[i] = i;
    int previ = -1;
    for (int i = 0; i < len; i++) {
        int randi = i;
        do {
            randi = rand() % len;
        } while (abs(randi - i) < 5);
        //        int randi = rand() % len;
        int tmp = intrlv[i];
        intrlv[i] = intrlv[randi];
        intrlv[randi] = tmp;
    }
}

void init_interleaver_ttcm(int *intrlv, int len, unsigned int seed) {
    srand(seed);
    int tmp[2][len / 2];

    for (int i = 0; i < len / 2; i++) {
        tmp[0][i] = 2 * i;
        tmp[1][i] = 2 * i + 1;
    }

    for (int i = 0; i < len / 2; i++) {
        int iv = i;
        do {
            iv = rand() % (len / 2 - i);
        } while (abs(iv - i) < 5);
        //        int iv = i + rand() % (len / 2 - i);
        int t = tmp[0][i];
        tmp[0][i] = tmp[0][iv];
        tmp[0][iv] = t;

        iv = i;
        do {
            iv = rand() % (len / 2 - i);
        } while (abs(iv - i) < 5);
        //        iv = i + rand() % (len / 2 - i);
        t = tmp[1][i];
        tmp[1][i] = tmp[1][iv];
        tmp[1][iv] = t;
    }
    for (int j = 0; j < len / 2; j++) {
        for (int i = 0; i < 2; i++) {
            intrlv[i + j * 2] = tmp[i][j];
        }
    }
}

void init_s_random(int* intrlv, int len, int d) {
    assert(len <= CTXLEN);

    int S = d;
    int N = len;

    while (true) {
        int NN[CTXLEN];
        for (int i = 0; i < N; i++)
            NN[i] = i;
//        int v[CTXLEN];
        int* v = intrlv;
        int vk = 0;

        int lp = 1;
        for (int i = 0; i < N; i++) {
            if (lp <= N) {
                lp = 1;
                int test = 1;
                int N1 = N - i;
                int kk;
                while (test == 1 && lp <= N) {
                    kk = rand() % N1;
                    int k = NN[kk];
                    int d = N;
                    int i0 = MAX(0, i - S);
                    for (int i0 = MAX(0, i - S); i0 < i - 1; i++) {
                        d = MIN(d, v[i0] - k);
                    }
                    if (d > S) {
                        v[vk] = k;
                        vk++;
                        test = 0;
                    }
                    lp = lp + 1;
                }
                NN[kk] = NN[N1 - 1];
                //            NN=[NN(1:kk-1) NN(kk+1:N1)];
            }
        }

        if (vk == N)
            break;
    }
}
