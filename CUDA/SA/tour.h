#ifndef _TOUR
#define _TOUR

#include "config.h"
#include "town.h"

struct tour {
    double length;
    TownList * town_list;
    town_index_t * tour;
    town_index_t num_vertexes;
};
typedef struct tour Tour;

Tour * tour_new(TownList * towns, town_index_t num_vertexes); // returns pre-allocated but empty tour
void tour_randomize(Tour *); // randomizes pre-allocated tour, but don't calculate length
double tour_set_len(Tour *); // calculates length, returns it and sets it
Tour * tour_destroy(Tour *);

void tour_mutate(Tour *newt, Tour *oldt); // mutates, but does not calculate length

#endif
