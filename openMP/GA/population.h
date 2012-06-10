#ifndef _POP_H
#define _POP_H

#include "config.h"
#include "town.h"
#include <float.h>

#define FIT_MIN -DBL_MAX

struct subject{
    fit_t fitness;
    gene_t tour[NUM_VERTEXES];
};
typedef struct subject Subject;

struct population {
    fit_t max_fitness;
    subj_t fittest; 
    Town * town_list;
    Subject * pop;
};
typedef struct population Population;

// return the length of the subj
fit_t subj_tour_length(Subject *, Town *);

// prints the tour of the subject
void subj_print_tour(Subject *, Town *); 

void pop_destroy(Population *); //destroys population

//return new empty population
Population * pop_new(Town * t_list);

// randomizes pre-allocated empty population
void pop_randomize(Population * empty_pop);

// generates children (pre-allocated) based on parents
void pop_randomize(Population *children, Population *parents );

#endif
