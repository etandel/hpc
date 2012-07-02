#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "town.h"
#include "population.h"
#include <mpi.h>

#define cron_begin clock
#define cron_end(start) ((clock() - start) / CLOCKS_PER_SEC)

int main (const int argc, const char * argv[]){
    Town *t_list;
    Population *parents, *children, *dummy;
    fit_t max_fitness = FIT_MIN;
    Subject fittest;
    int stag_count=0, iter=0;

    double start;
    
    //puts("Executing sequential version");

    srand((int)time(NULL)); //seed pseudo-rand generator
    start = cron_begin();

    // initialization
    t_list = tl_new(NUM_VERTEXES);
    tl_randomize(t_list);

    parents  = pop_new(t_list);
    pop_randomize(parents);
    pop_set_fit(parents);
    max_fitness = parents->max_fitness;
    fittest     = parents->pop[parents->fittest];

    children = pop_new(t_list);
    do {
        iter++;
        pop_reproduce(children, parents);
        pop_set_fit(children);
        if (children->max_fitness > max_fitness){
            fittest     = children->pop[children->fittest];
            max_fitness = children->max_fitness;
            stag_count  = 0;
            //printf("New max fitness: %.17f\n", max_fitness);
        }
        else
            stag_count++;

        dummy    = parents;
        parents  = children;
        children = dummy;
    } while(stag_count < STAG_COUNT);

    printf("Sequential iterations / time = %f\n", iter / cron_end(start));
    /*
        printf("Sequential iterations: %d ; sequential time: %f seconds.\n", iter, MPI_Wtime() - start);
        printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
    // */
    pop_destroy(parents);
    pop_destroy(children);
    tl_destroy(t_list);

    return 0;
}
