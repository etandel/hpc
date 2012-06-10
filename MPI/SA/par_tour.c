#include "tour.h"
#include "config.h"
#include "town.h"
#include "pools.h"
#include <stdlib.h>
#include <mpi.h>

static Tour * new_empty_tour(TownList *towns, town_index_t num_vertexes){
    Tour * newtour = (Tour*) malloc(sizeof(Tour));
    newtour->tour = (town_index_t *) malloc(sizeof(town_index_t)*num_vertexes);
    newtour->town_list = towns;

    return newtour;
}

Tour * tour_new(TownList *towns) {
    town_index_t i, num_vertexes = towns->config.num_vertexes;
    TownPool *tp = tp_new(num_vertexes);
    if (!tp)
        exit(1);
    Tour * newtour = new_empty_tour(towns, num_vertexes);
    
    for (i=0; i<num_vertexes; i++){
        newtour->tour[i] = tp_random_town(tp);
    }
    

    newtour->length = tour_length(newtour);
    tp_destroy(tp);
    return newtour;
}

Tour * tour_destroy(Tour * tour){
    free(tour->tour);
    free(tour);
    return tour = NULL;
}

double tour_length(Tour * tour){
    double len = 0;
    TownList * towns = tour->town_list;
    town_index_t i=1, n_vertexes = towns->config.num_vertexes;
    town_index_t *index_list = tour->tour;

    for(i=1; i<n_vertexes; i++)
        len += tl_distance(towns, index_list[i-1], index_list[i]);
    len += tl_distance(towns, index_list[0], index_list[i-1]);

    return len;
}

Tour * tour_mutate(Tour *old_tour) {
    town_index_t i, num_vertexes;
    TownPool *tp;
    Tour * new_tour;

    num_vertexes = old_tour->town_list->config.num_vertexes;
    tp           = tp_new(num_vertexes);
    new_tour     = new_empty_tour(old_tour->town_list, num_vertexes);

    for (i=0; i<num_vertexes; i++){
        new_tour->tour[i] = old_tour->tour[i];
    }

    {
        town_index_t i1, i2;
        i1 = (town_index_t)tp_random_town(tp);
        i2 = (town_index_t)tp_random_town(tp);
        new_tour->tour[i1] = old_tour->tour[i2];
        new_tour->tour[i2] = old_tour->tour[i1];
    }

    new_tour->length = tour_length(new_tour);

    tp_destroy(tp);
    return new_tour;
}
