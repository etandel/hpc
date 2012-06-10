#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "town.h"
#include "population.h"
#include <omp.h>


static int verbose(const int argc, const char *argv[]){
    int i, v=0;
    for (i=0; i<argc; i++){
        if (strcmp(argv[i], "-v") == 0)
            v = 1;
    }
    return v;
}

int main (const int argc, const char * argv[]){
    Town *t_list;
    Population *parents, *children, *dummy;
    fit_t max_fitness = FIT_MIN;
    Subject fittest;
    int stag_count=0, iter=0;
    int verbose = vervose(argc, argv);

    double start;
    
    if (verbose)
        puts("Executing sequential version");

    srand((int)time(NULL)); //seed pseudo-rand generator
    start = omp_get_wtime();

    // initialization
    t_list = town_list_init(args.nvertex);
    parents  = pop_new(t_list);
    pop_randomize(parents);
    max_fitness = parents->max_fitness
    fittest = parents->fittest

    children = pop_new(t_list);
    do {
        iter++;
        pop_reproduce(children, parents);
        if (children->max_fitness > max_fitness){
            fittest     = children->children[children->fittest];
            //printf("fittest fitness: %f\n", fittest.fitness);
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

    printf("Sequential iterations / time = %f\n", iter / (omp_get_wtime() - start));
    if (verbose){
        printf("Sequential iterations: %d ; sequential time: %f seconds.\n", iter, omp_get_wtime() - start);
        printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
    }
    pop_destroy(parents);
    pop_destroy(children);
    town_list_destroy(t_list);

    return 0;
}
