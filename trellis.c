#include "trellis.h"
#include "utils.h"
#include <math.h>
#include "contex.h"

int oct2dec(int o) {
    int i = 0;
    int n = 0;
    while (o % 10) {
        n += (int) pow(8, i) * (o % 10);
        o = o / 10;
        i++;
    }
    return n;
}

void init_trellis(int k, int m, int clen, u8 *coef,
        void *nxs, void *prs, void *nxo, void *pro) {

    int K = 1 << k;
    int M = 1 << m;

    u8 (*next_state)[K] = nxs;
    u8 (*prev_state)[K] = prs;
    u8 (*next_out)[K] = nxo;
    u8 (*prev_out)[K] = pro;

    assert(k == (clen - 1));
    u8 fb = oct2dec(coef[0]);
    for (u8 sym = 0; sym < K; sym++) {
        for (u8 state = 0; state < M; state++) {
            u8 out = state & 1;
            for (int bit = 0; bit < k; bit++) {
                out ^= BIT(sym, k - bit - 1) * BIT(oct2dec(coef[bit + 1]), 0);
            }
            next_out[state][sym] = (sym << 1) | out;

            u8 next = state;
            for (int bit = 0; bit < k; bit++) {
                next ^= BIT(sym, k - bit - 1) * oct2dec(coef[bit + 1]);
            }
            next ^= BIT(state, 0) * fb;
            next = next >> 1;
            next_state[state][sym] = next;
            prev_state[next][sym] = state;
            prev_out[next][sym] = next_out[state][sym];
        }
    }
}