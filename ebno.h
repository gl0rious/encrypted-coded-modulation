#ifndef EBNO_H
#define EBNO_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _ebn0 EbN0;

    typedef struct _ebn0 {
        float ebno;
        int err;
        EbN0 *prev;
        EbN0 *next;
    } EbN0;

    EbN0 *init_EbN02(float from, float to);   
    EbN0 *insert_before(EbN0 *iter);

#ifdef __cplusplus
}
#endif

#endif /* EBNO_H */

