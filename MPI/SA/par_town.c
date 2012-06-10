#include <stdlib.h>
#include <math.h>
#include "town.h"
#include "config.h"

struct town{
    coord_t x;
    coord_t y;
};

TownList * tl_new(Config config){
    TownList * newt = (TownList *) malloc(sizeof(TownList));
    newt->list = (Town*) malloc(NUM_VERTEXES * sizeof(Town));
    newt->config = config;
    return newt;
}

#define RANDOM_COORD(min, max) (coord_t)(rand() % (max - min) + min);
static void add_random_town(TownList * t_list, town_index_t i){
    t_list->list[i].x = RANDOM_COORD(0, t_list->config.grid_size);
    t_list->list[i].y = RANDOM_COORD(0, t_list->config.grid_size);
}

void tl_randomize(TownList *tl){
    town_index_t i;
    for (i=0; i < NUM_VERTEXES; i++)
        add_random_town(tl, i);
}

TownList * tl_destroy(TownList * t_list){
    free(t_list->list);
    free(t_list);
    return t_list = NULL;
}

double tl_distance(TownList * t_list, town_index_t i1, town_index_t i2){
    Town *t1 = &t_list->list[i1], *t2 = &t_list->list[i2];
    double dx = t2->x - t1->x;
    double dy = t2->y - t1->y;
    return sqrt(dx*dx + dy*dy);
}
