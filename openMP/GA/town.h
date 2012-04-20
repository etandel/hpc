#ifndef _TOWN_H
#define _TOWN_H

#include "config.h"

struct town{
    coord_t x;
    coord_t y;
};
typedef struct town Town;


Town * town_list_init(void); //returns list of randomly generated towns
void town_list_destroy(Town *); //destroy list of towns

//returns distance between two towns
fit_t town_distance(Town *, gene_t, gene_t);

#endif
