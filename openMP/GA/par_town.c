#include <stdlib.h>
#include <math.h>
#include "town.h"
#include "config.h"
#include <omp.h>

static coord_t random_coord(coord_t min, coord_t max){
    return (coord_t)(rand() % (max - min) + min);
}

static void add_random_town(Town * t_list, gene_t i){
    t_list[i].x = random_coord(0, GRID_SIZE);
    t_list[i].y = random_coord(0, GRID_SIZE);
}

Town * town_list_init(void){
    Town * newt = (Town *) malloc(NUM_VERTEXES * sizeof(Town));
    gene_t i;

    #pragma omp parallel for
    for (i=0; i<NUM_VERTEXES; i++)
        add_random_town(newt, i);

    return newt;
}

void town_list_destroy(Town * t_list){
    free(t_list);
}

fit_t town_distance(Town * t_list, gene_t g1, gene_t g2){
    Town *t1 = &t_list[g1], *t2 = &t_list[g2];
    fit_t dx = (fit_t) (t2->x - t1->x);
    fit_t dy = (fit_t) (t2->y - t1->y);
    return (fit_t) sqrt((double)(dx*dx + dy*dy));
}
