#include "ebno.h"
#include <memory.h>
#include <stdlib.h>

EbN0 *init_EbN02(float from, float to) {
    EbN0 *head = (EbN0*) malloc(sizeof (EbN0));
    head->ebno = from;
    head->err = -1;

//    EbN0 *iter = head;
//    while (iter->ebno < to) {
//        iter->next = (EbN0*) calloc(1, sizeof (EbN0));
//        iter->next->ebno = iter->ebno + step;
//        iter->next->err = -1;
//        iter->next->prev = iter;
//        iter = iter->next;
//    }
    
    head->next = (EbN0*) calloc(1, sizeof (EbN0));
    head->next->ebno = to;
    head->next->err = -1;    
    head->next->prev = head;

    return head;
}

EbN0 *insert_before(EbN0 *iter) {
    if (iter == NULL || iter->prev == NULL)
        return NULL;
    EbN0 *newIter = (EbN0*) calloc(1, sizeof (EbN0));
    newIter->ebno = (iter->prev->ebno + iter->ebno) / 2;
    newIter->err = -1;

    newIter->prev = iter->prev;
    newIter->next = iter;
    iter->prev->next = newIter;
    iter->prev = newIter;
    return newIter;
}