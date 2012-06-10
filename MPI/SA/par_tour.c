#include "tour.h"
#include "config.h"
#include "town.h"
#include "pools.h"
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0

Tour * tour_new(TownList *towns, town_index_t num_vertexes){
    Tour * newtour = (Tour*) malloc(sizeof(Tour));
    newtour->tour = (town_index_t *) malloc(sizeof(town_index_t)*num_vertexes);
    newtour->town_list = towns;
    newtour->num_vertexes = num_vertexes;
    return newtour;
}

void tour_randomize(Tour * tour) {
    town_index_t i, num_vertexes=tour->num_vertexes;
    TownPool *tp = tp_new(num_vertexes);
    
    for (i=0; i<num_vertexes; i++){
        tour->tour[i] = tp_random_town(tp);
    }

    tp_destroy(tp);
}

Tour * tour_destroy(Tour * tour){
    free(tour->tour);
    free(tour);
    return tour = NULL;
}

double partial_len(Tour *tour, town_index_t num_vertex){
    double len = 0;
    TownList * towns = tour->town_list;
    town_index_t i;
    town_index_t *index_list = tour->tour;

    for(i=1; i<num_vertex; i++)
        len += tl_distance(towns, index_list[i-1], index_list[i]);
    len += tl_distance(towns, index_list[0], index_list[i-1]);

    tour->length = len;
    return len;
}

double tour_set_len(Tour * tour){
    double len, tmp_len;
    int rank, size;
    int tmp_nvertex;
    Tour *tmp_tour;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    tmp_nvertex = NUM_VERTEXES / size;
    tmp_tour = tour_new(tour->town_list, tmp_nvertex);

    MPI_Scatter(tour->tour, tmp_nvertex, MPI_UNSIGNED,
                tmp_tour->tour, tmp_nvertex, MPI_UNSIGNED,
                MASTER, MPI_COMM_WORLD);

    tmp_len = partial_len(tmp_tour, tmp_nvertex);
    MPI_Reduce(&tmp_len, &len, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    tour_destroy(tmp_tour);
    tour->length = len;
    return len;
}

void tour_mutate(Tour *new_tour, Tour *old_tour) {
    town_index_t i;
    TownPool *tp;

    tp = tp_new(NUM_VERTEXES);

    for (i=0; i<NUM_VERTEXES; i++){
        new_tour->tour[i] = old_tour->tour[i];
    }

    {
        town_index_t i1, i2;
        i1 = (town_index_t)tp_random_town(tp);
        i2 = (town_index_t)tp_random_town(tp);
        new_tour->tour[i1] = old_tour->tour[i2];
        new_tour->tour[i2] = old_tour->tour[i1];
    }

    tp_destroy(tp);
}
