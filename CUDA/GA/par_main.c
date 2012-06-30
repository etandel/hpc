#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "town.h"
#include "population.h"
#include <mpi.h>

#define MASTER 0

int main (int argc, char * argv[]){
    Town *t_list;
    Population *parents, *children, *dummy;
    fit_t max_fitness;
    Subject fittest;
    int stag_count=0, iter=0;

    int rank, size;

    double start;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand((int)time(NULL)); //seed pseudo-rand generator

    // puts("Executing parallel version");

    if (rank == MASTER){
        start = MPI_Wtime();
    }

    // allocation
    t_list = tl_new(NUM_VERTEXES);
    parents  = pop_new(t_list);
    children = pop_new(t_list);
    
    // initialization
    if (rank == MASTER){
        tl_randomize(t_list);
        pop_randomize(parents);
    }
    pop_set_fit(parents);
    if (rank == MASTER){
        max_fitness = parents->max_fitness;
        fittest     = parents->pop[parents->fittest];
    }
    MPI_Bcast(t_list, NUM_VERTEXES*sizeof(Town), MPI_BYTE, MASTER, MPI_COMM_WORLD);

    do {
        if (rank == MASTER){
            iter++;
            pop_reproduce(children, parents);
        }
        pop_set_fit(children);
        if (rank == MASTER) {
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
        }
    } while(stag_count < STAG_COUNT);

    if (rank == MASTER) {
        printf("Sequential iterations / time = %f\n", iter / (MPI_Wtime() - start));
        /*
        printf("Sequential iterations: %d ; sequential time: %f seconds.\n", iter, MPI_Wtime() - start);
        printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
        // */
    }

    pop_destroy(parents);
    pop_destroy(children);
    tl_destroy(t_list);

    MPI_Finalize();
    return 0;
}
