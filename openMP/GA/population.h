#ifndef _POP_H
#define _POP_H

#include "config.h"
#include "town.h"

struct subject{
    fit_t fitness;
    gene_t tour[NUM_VERTEXES];
};
typedef struct subject Subject;

struct population {
    fit_t max_fitness;
    pop_index_t fittest; 
    Town * town_list;
    Subject * pop;
};
typedef struct population Population;

// return the length of the subj
fit_t subj_tour_length(Subject *, Town *);

// prints the tour of the subject
void subj_print_tour(Subject *, Town *); 

void pop_destroy(Population *); //destroys population

// returns next generation (if old population is given)
// or new population with random subjects (if NULL is passed as first parameter)
Population * pop_new(Population *, Town *);

#endif
