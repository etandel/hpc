#include <stdlib.h>
#include <math.h>
#include "town.h"
#include "config.h"

Town * tl_new(gene_t nvertex){
    Town * newt;
    nvertex = nvertex ? nvertex : NUM_VERTEXES;
    newt = (Town *) malloc(NUM_VERTEXES * sizeof(Town));
    return newt;
}

static coord_t random_coord(coord_t min, coord_t max){
    return (coord_t)(rand() % (max - min) + min);
}

static void add_random_town(Town * t_list, int i){
    t_list[i].x = random_coord(0, GRID_SIZE);
    t_list[i].y = random_coord(0, GRID_SIZE);
}

void tl_randomize(Town *town){
    gene_t i;
    for (i=0; i<NUM_VERTEXES; i++)
        add_random_town(town, i);
}

void tl_destroy(Town * t_list){
    free(t_list);
}

fit_t tl_distance(Town * t_list, gene_t g1, gene_t g2){
    Town *t1 = &t_list[g1], *t2 = &t_list[g2];
    fit_t dx = (fit_t) (t2->x - t1->x);
    fit_t dy = (fit_t) (t2->y - t1->y);
    return (fit_t) sqrt((double)(dx*dx + dy*dy));
}
