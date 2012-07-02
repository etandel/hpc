#ifndef _TOWN_H
#define _TOWN_H

#include "config.h"

struct town{
    coord_t x;
    coord_t y;
};
typedef struct town Town;

struct townlist {
    Config config;
    Town * list;
};
typedef struct townlist TownList;

TownList * tl_new(Config); //returns pre-allocated but empty town list
void tl_randomize(TownList*); // randomizes pre-allocated town list
TownList * tl_destroy(TownList *); //destroy list of towns

//returns distance between two towns
#ifndef CUDA
double tl_distance(TownList *, town_index_t, town_index_t);
#endif

#endif
