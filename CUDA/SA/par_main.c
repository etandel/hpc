#include "config.h"
#include "town.h"
#include "tour.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define cron_begin() clock()
#define cron_end(start) ((clock() - start) / CLOCKS_PER_SEC)

static int should_replace(Tour *new, Tour *old, double temperature) {
    // returns true if dl < 0 (new path is better than old)
    // or randomly, based on boltzman probability distribution
    double dl = new->length - old->length;
    return dl<0 ? 1 : rand()/RAND_MAX < exp(-dl/temperature);
}


int main(const int argc, const char *argv[]){
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
    double start;

    //puts("Executing sequential version");

    srand((int)time(NULL));

    start = cron_begin();

    towns = tl_new(state);
    tl_randomize(towns);

    old_tour = tour_new(towns, NUM_VERTEXES); 
    tour_randomize(old_tour);
    tour_set_len(old_tour);

    new_tour = tour_new(towns, NUM_VERTEXES);
    while (state.temperature > state.epsilon){
        i++;
        tour_mutate(new_tour, old_tour);
        tour_set_len(new_tour);
        if (should_replace(new_tour, old_tour, state.temperature)){
            Tour * dummy = old_tour;
            old_tour = new_tour;
            new_tour = dummy;
        }
        
        state.temperature *= state.alpha;
    }
    

    printf("Sequential time: %f\n", cron_end(start));
    //printf("After %d iterations, the best length: %f\n", i, old_tour->length);

    tour_destroy(old_tour);
    tour_destroy(new_tour);
    tl_destroy(towns);
    return 0;
}
