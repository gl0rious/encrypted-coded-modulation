
#ifndef INTRLV_H
#define INTRLV_H

#ifdef __cplusplus
extern "C" {
#endif

void init_interleaver_ttcm(int *intrlv, int len, unsigned int seed);
void init_interleaver_uttcm(int *intrlv, int len, unsigned int seed);


#ifdef __cplusplus
}
#endif

#endif /* INTRLV_H */

