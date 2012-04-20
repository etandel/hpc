#ifndef _TOUR
#define _TOUR

#include "config.h"
#include "town.h"

struct tour {
    double length;
    TownList * town_list;
    town_index_t * tour;
};
typedef struct tour Tour;

Tour * tour_new(TownList *);
Tour * tour_destroy(Tour *);

double tour_length(Tour *);
Tour * tour_mutate(Tour *);

#endif
