#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "town.h"
#include "population.h"
#include <mpi.h>

int main (const int argc, const char * argv[]){
    Town *t_list;
    Population *parents, *children, *dummy;
    fit_t max_fitness = FIT_MIN;
    Subject fittest;
    int stag_count=0, iter=0;

    double start;
    
    // puts("Executing parallel version");

    srand((int)time(NULL)); //seed pseudo-rand generator
    start = MPI_Wtime();

    // initialization
    t_list = town_list_init(NUM_VERTEXES);
    parents  = pop_new(t_list);
    pop_randomize(parents);
    max_fitness = parents->max_fitness;
    fittest     = parents->pop[parents->fittest];

    children = pop_new(t_list);
    do {
        iter++;
        pop_reproduce(children, parents);
        if (children->max_fitness > max_fitness){
            fittest     = children->pop[children->fittest];
            //printf("fittest fitness: %f\n", fittest.fitness);
            max_fitness = children->max_fitness;
            stag_count  = 0;
            printf("New max fitness: %.17f\n", max_fitness);
        }
        else
            stag_count++;

        dummy    = parents;
        parents  = children;
        children = dummy;
    } while(stag_count < STAG_COUNT);

    printf("Sequential iterations / time = %f\n", iter / (MPI_Wtime() - start));
    /*
    printf("Sequential iterations: %d ; sequential time: %f seconds.\n", iter, MPI_Wtime() - start);
    printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
    // */

    pop_destroy(parents);
    pop_destroy(children);
    town_list_destroy(t_list);

    return 0;
}
