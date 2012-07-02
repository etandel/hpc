#ifndef _TOWN_H
#define _TOWN_H

#include "config.h"

struct town{
    coord_t x;
    coord_t y;
};
typedef struct town Town;


Town * tl_new(gene_t); // returns allocated but empy town list
void tl_randomize(Town *); // randomizes pre-allocated town list
void tl_destroy(Town *); //destroy list of towns

#ifndef CUDA
//returns distance between two towns
fit_t tl_distance(Town *, gene_t, gene_t);
#endif

#endif
