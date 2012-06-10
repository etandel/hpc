#include "config.h"
#include "town.h"
#include "tour.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define MASTER 0

static int should_replace(Tour * new, Tour * old, double temperature){
    // returns true if dl < 0 (new path is better than old)
    // or randomly, based on boltzman probability distribution
    double dl = new->length - old->length;
    return dl<0 ? 1 : rand()/RAND_MAX < exp(-dl/temperature);
}

int main(int argc, char *argv[]){
    Config state = {
        GRID_SIZE,
        NUM_VERTEXES,
        ALPHA,
        EPSILON,
        TEMPERATURE
    };

    int size, rank;

    TownList * towns;
    Tour *old_tour, *new_tour;
    int i = 0, must_replace;
    double start;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // printf("Executing parallel version with %d threads.\n", args.nthreads);

    srand((int)time(NULL));

    if (rank == MASTER)
        start = MPI_Wtime();

    // generate and broadcast town list
    towns = tl_new(state);
    if (rank == MASTER)
        tl_randomize(towns);
    MPI_Bcast(towns->list, NUM_VERTEXES*sizeof(Town), MPI_BYTE, MASTER, MPI_COMM_WORLD);

    // generate initial tour
    old_tour = tour_new(towns, NUM_VERTEXES); 
    if (rank == MASTER)
        tour_randomize(old_tour);
    tour_set_len(old_tour);

    new_tour = tour_new(towns, NUM_VERTEXES);
    /*
    while (state.temperature > state.epsilon){
        if (rank == MASTER) {
            i++;
            tour_mutate(new_tour, old_tour);
        }
        tour_set_len(new_tour);
        if (rank == MASTER)
            must_replace = should_replace(new_tour, old_tour, state.temperature);
        
        MPI_Bcast(&must_replace, sizeof(int), MPI_INT, MASTER, MPI_COMM_WORLD);
        if (must_replace){
            Tour * dummy = old_tour;
            old_tour = new_tour;
            new_tour = dummy;
        }
        
        state.temperature *= state.alpha;
    }
    

    if (rank == MASTER)
        printf("Parallel time: %f\n", MPI_Wtime() - start);
    //printf("After %d iterations, the best length: %f\n", i, tour_length(old_tour));
    */

    tour_destroy(old_tour);
    tour_destroy(new_tour);
    tl_destroy(towns);
    
    MPI_Finalize();
    return 0;
}
