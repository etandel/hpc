#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "town.h"
#include "population.h"
#include <omp.h>
#include <float.h>

int main (const int argc, const char * argv[]){
    Town * t_list;
    Population * pop;
    fit_t max_fitness = -DBL_MAX/2;
    Subject fittest;
    int stag_count=0, iter=0;

    double start;

    srand((int)time(NULL)); //seed pseudo-rand generator

    omp_set_num_threads(4);
    start = omp_get_wtime();

    t_list = town_list_init();

    pop = pop_new(NULL, t_list);
    do {
        iter++;
        if (pop->max_fitness > max_fitness){
            fittest     = pop->pop[pop->fittest];
            //printf("fittest fitness: %f\n", fittest.fitness);
            max_fitness = pop->max_fitness;
            stag_count  = 0;
            //printf("New max fitness: %.17f\n", max_fitness);
        }
        else
            stag_count++;

        pop = pop_new(pop, t_list);
    } while(stag_count < STAG_COUNT);

    printf("After %d iterations in %f seconds,\n", iter, omp_get_wtime() - start);
    printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
    pop_destroy(pop);
    town_list_destroy(t_list);

    return 0;
}
