#include "config.h"
#include "town.h"
#include "tour.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <string.h>

static struct args {
    int verbose;
    int nvertex;
};

static int read_args(const int argc, const char *argv[], struct args * args){
    int i;
    int v = 0, nvertex=NUM_VERTEXES;
    if (argc == 0) return 0;
    for (i=0; i<argc; i++){
        if (strcmp(argv[i], "-v") == 0)
            v = 1;
        else if (strcmp(argv[i], "-n") == 0){
            //-t without any arguments after or with no following integer
            if ((argc == i+1) || (sscanf(argv[i+1], "%d", &nvertex) == 0))
                return 0;
        }
        

    }
    args->verbose = v;
    args->nvertex = nvertex;
    return 1;
}

static int should_replace(Tour * new, Tour * old, double temperature){
    // returns true if dl < 0 (new path is better than old)
    // or randomly, based on boltzman probability distribution
    double dl = new->length - old->length;
    return dl<0 ? 1 : rand()/RAND_MAX < exp(-dl/temperature);
}


int main(const int argc, const char *argv[]){
    struct args args;
    Config state = {
        GRID_SIZE,
        NUM_VERTEXES,
        ALPHA,
        EPSILON,
        TEMPERATURE
    };

    TownList * towns;
    Tour *old_tour, *new_tour;
    int i = 0, verbose, nthreads;
    double start;

    if (!read_args(argc, argv, &args)){
        puts("Bad argument list. Exiting...");
        return 1;
    }

    state.num_vertexes = args.nvertex;
    if (args.verbose)
        puts("Executing sequential version");

    srand((int)time(NULL));

    start = omp_get_wtime();
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
    

    printf("Sequential time: %f\n", omp_get_wtime() - start);
    if (args.verbose)
        printf("After %d iterations, the best length: %f\n", i, tour_length(old_tour));
    tour_destroy(old_tour);
    tl_destroy(towns);
    return 0;
}
