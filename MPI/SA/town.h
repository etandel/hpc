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
    Town list[NUM_VERTEXES];
};
typedef struct townlist TownList;

TownList * tl_new(Config); //returns list of randomly generated towns
TownList * tl_destroy(TownList *); //destroy list of towns

//returns distance between two towns
double tl_distance(TownList *, town_index_t, town_index_t);

#endif
