#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "town.h"
#include "population.h"
#include <omp.h>

static struct args {
    int verbose;
    int nthreads;
    gene_t nvertex;
    subj_t npop;
};

static int read_args(const int argc, const char *argv[], struct args * args){
    int i;
    int v = 0, nthreads = 8;
    gene_t nvertex=NUM_VERTEXES;
    subj_t npop=POP_SIZE;
    if (argc == 0) return 0;
    for (i=0; i<argc; i++){
        if (strcmp(argv[i], "-v") == 0)
            v = 1;
        else if (strcmp(argv[i], "-t") == 0){
            //-t without any arguments after or with no following integer
            if ((argc == i+1) || (sscanf(argv[i+1], "%d", &nthreads) == 0))
                return 0;
        }
        else if (strcmp(argv[i], "-n") == 0){
            //-t without any arguments after or with no following integer
            if ((argc == i+1) || (sscanf(argv[i+1], "%d", &nvertex) == 0))
                return 0;
        }
        else if (strcmp(argv[i], "-p") == 0){
            //-t without any arguments after or with no following integer
            if ((argc == i+1) || (sscanf(argv[i+1], "%d", &npop) == 0))
                return 0;
        }
        

    }
    args->verbose = v;
    args->nvertex = nvertex;
    args->npop    = npop;
    return 1;
}

int main (const int argc, const char * argv[]){
    struct args args;
    Town * t_list;
    Population * pop;
    fit_t max_fitness = FIT_MIN;
    Subject fittest;
    int stag_count=0, iter=0;

    double start;

    if (!read_args(argc, argv, &args)){
        puts("Bad argument list. Exiting...");
        return 1;
    }

    if (args.verbose)
        puts("Executing parallel version");

    omp_set_num_threads(args.nthreads);
    srand((int)time(NULL)); //seed pseudo-rand generator
    start = omp_get_wtime();

    t_list = town_list_init(args.nvertex);

    pop = pop_new(NULL, args.npop, t_list, args.nvertex);
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

        pop = pop_new(pop, args.npop, t_list, args.nvertex);
    } while(stag_count < STAG_COUNT);

    printf("Parallel iterations / time = %f\n", iter / (omp_get_wtime() - start));
    if (args.verbose){
        printf("Parallel iterations: %d ; parallel time: %f seconds.\n", iter, omp_get_wtime() - start);
        printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
    }
    pop_destroy(pop);
    town_list_destroy(t_list);

    return 0;
}
