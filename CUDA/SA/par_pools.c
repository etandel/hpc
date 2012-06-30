#include "config.h"
#include "pools.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct townpool{
    town_index_t num_indexes;
    town_index_t n_unused;
    unsigned char *arr;
};

TownPool * tp_new(town_index_t num_indexes){
    size_t size = (size_t) ceil(num_indexes/8.0);
    TownPool *tp =  (TownPool*) malloc(sizeof(TownPool));
    if (!tp)puts("TROLLOLOL");
    tp->arr = (unsigned char *) malloc(size);
    if (!tp->arr)puts("TROLLOLOL2");
    memset(tp->arr, 0xff, size);

    tp->n_unused    = num_indexes;
    tp->num_indexes = num_indexes;

    return tp; 
}

TownPool * tp_destroy(TownPool *tp){
    free(tp->arr);
    free(tp);
    return tp = NULL;
}

int tp_usable(TownPool *tp, town_index_t i){
    return ((unsigned char)1 << i%8 & tp->arr[i/8]) != 0;
}

int tp_unusable(TownPool *tp, town_index_t i){
    return ((unsigned char)1 << i%8 & tp->arr[i/8]) == 0;
}

void tp_remove(TownPool *tp, town_index_t i){
    unsigned char mask = (unsigned char)1 << i%8, current = tp->arr[i/8];
    tp->arr[i/8] = current & mask ? current ^ mask : current;
}

void tp_reset(TownPool *tp){
    memset(tp->arr, 0xff, (size_t)ceil(tp->num_indexes/8.0));
}

#define random_town() (rand() % NUM_VERTEXES)
town_index_t tp_random_town(TownPool *tp){
    town_index_t index = -1;
    if (tp->n_unused > 0){
        do {
            index = ((town_index_t)rand()) % tp->num_indexes;
        } while(tp_unusable(tp, index));
        tp_remove(tp, index);
        tp->n_unused--;
    }
    return index;
}
