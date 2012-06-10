#include "config.h"
#include "town.h"
#include "tour.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

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

    TownList * towns;
    Tour *old_tour, *new_tour;
    int i = 0;
    int rank, size;
    double start;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //puts("Executing parallel version\n");
    //

    srand((int)time(NULL));

    start = MPI_Wtime();
    towns = tl_new(state);
    old_tour = tour_new(towns); 
    while (state.temperature > state.epsilon){
        i++;
        new_tour = tour_mutate(old_tour);
        if (should_replace(new_tour, old_tour, state.temperature)){
            tour_destroy(old_tour);
            old_tour = new_tour;
        }
        else{
            tour_destroy(new_tour);
        }
        
        state.temperature *= state.alpha;
    }
    

    printf("Parallel time: %f\n", MPI_Wtime() - start);
    //printf("After %d iterations, the best length: %f\n", i, tour_length(old_tour));
    tour_destroy(old_tour);
    tl_destroy(towns);

    MPI_Finalize();
    return 0;
}
