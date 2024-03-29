#include "config.h"
#include "town.h"
#include "population.h"
#include "pools.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define random_gene() ((gene_t)(rand() % NUM_VERTEXES))

fit_t subj_tour_length(Subject * subj, Town *t_list){
    gene_t i = NUM_VERTEXES-1, *tour=subj->tour;
    fit_t total_len = 0;

    for (i=0; i<NUM_VERTEXES-1; i++){
        total_len += tl_distance(t_list, tour[i], tour[i+1]);
    }
    total_len += tl_distance(t_list, tour[i], tour[0]); //the return to the beginning

    return total_len;
}

fit_t calc_fitness(Subject * subj, Town *t_list){
    return -subj_tour_length(subj, t_list);
}

void subj_print_tour(Subject * subj, Town *t_list){
    gene_t i;
    for (i=0; i<NUM_VERTEXES; i++)
        printf("(%u, %u)\n", t_list[i].x, t_list[i].y);
}


/****** Beginning of random_new() and its auxiliary functions ******/

static void add_random_subj(Population * newp, subj_t i){
    gene_t gene_i;
    Subject * subj = newp->pop+i;
    GenePool gp = gp_new();

    for (gene_i=0; gene_i<NUM_VERTEXES; gene_i++)
        subj->tour[gene_i] = gp_get_random(gp);

    gp_destroy(gp);
}

Population *pop_new(Town *t_list){
    Population *newp = (Population*) malloc(sizeof(Population));
    newp->pop        = (Subject*) malloc(POP_SIZE*sizeof(Subject));
    newp->t_list     = t_list;
    return newp;
}

void pop_randomize(Population *newp){
    // randomizes population 
    subj_t i;

    //adds random subjs
    for (i=0; i<POP_SIZE; i++)
        add_random_subj(newp, i);
}

void pop_set_fit(Population *pop){
    subj_t i, fittest;
    fit_t max_fit=FIT_MIN;

    for (i=0; i<POP_SIZE; i++){
        // calcs and sets fitness related stuff
        Subject *subj = pop->pop+i;
        fit_t new_fit = calc_fitness(subj, pop->t_list);
        subj->fitness = new_fit;

        if (new_fit > max_fit){
            max_fit = new_fit;
            fittest = i;
        }
    }
    pop->max_fitness = max_fit;
    pop->fittest     = fittest;
}

/****** End of random_new() and its auxiliary functions ******/



/****** Beginning of next_generation() and its auxiliary functions ******/

//so that crossover point gives at least 1 gene for each parent
#define random_cross_point() (rand() % (NUM_VERTEXES-2) + 1)
static void crossover(gene_t *a_tour, gene_t *b_tour, gene_t *child_tour, GenePool gp ){
    gene_t i, cross_point = random_cross_point();
    gene_t holes[NUM_VERTEXES], nholes=0;

    //first part is equal to parent_a
    for (i=0; i<cross_point; i++){
        child_tour[i] = a_tour[i];
        gp_remove(gp, child_tour[i]);
    }

    //second part is equal to b unless the gene is unusable;
    //if so, keep a track of holes to be filled afterwards randomly
    for (; i<NUM_VERTEXES; i++){
        gene_t tmp = b_tour[i];
        if (gp_usable(gp, tmp)){
            child_tour[i] = tmp;
            gp_remove(gp, tmp);
        }
        else{
            //sets holes vec with index of hole
            holes[nholes++] = i;
        }
    }
            
    //hole filling with random usable genes
    for (i=0; i<nholes; i++){
        child_tour[holes[i]] = gp_get_random(gp);
        gp_remove(gp, child_tour[holes[i]]);
    }
}

static void mutate(Subject * subj){
    gene_t i2, i1=random_gene();
    gene_t tmp;
    do {
        i2 = random_gene();
    } while(i1 == i2);

    tmp = subj->tour[i1];
    subj->tour[i1] = subj->tour[i2];
    subj->tour[i1] = tmp;
}

#define MUST_CROSS  (rand()/(float)RAND_MAX<=CROSS_PROB)
#define MUST_MUTATE (rand()/(float)RAND_MAX<=MUTATION_PROB)
static Subject reproduce(Subject *parent_a, Subject *parent_b, Town * t_list){
    // returns possibly mutated child of a and b
    Subject new_subj;
    gene_t *tour = new_subj.tour , i;
    GenePool gp = gp_new();

    // if crossover shouldn't occur, child is equal to a
    if (MUST_CROSS)
        crossover(parent_a->tour, parent_b->tour, tour, gp);
    else
        for(i=0; i<NUM_VERTEXES; i++)
            new_subj.tour[i] = parent_a->tour[i];

    if (MUST_MUTATE)
        mutate(&new_subj);

    gp_destroy(gp);
    return new_subj;   
}

#define random_parent(old_pop) (old_pop->pop+(rand()%POP_SIZE))
void pop_reproduce(Population *newp, Population *oldp){
    //returns next generation
    subj_t i;
    Town *t_list = oldp->t_list;

    // generates, for every random pair of parents, 2 children
    for (i=0; i<POP_SIZE; i+=2){
        fit_t best_child_fitness;
        subj_t best_child;

        Subject *parent_a, *parent_b;

        parent_a = random_parent(oldp);
        do {
            parent_b = random_parent(oldp);
        } while (parent_a == parent_b);

        newp->pop[i]   = reproduce(parent_a, parent_b, t_list);
        newp->pop[i+1] = reproduce(parent_b, parent_a, t_list);
    }
}

/****** End of next_generation() and its auxiliary functions ******/



/****** Beginning of interface  ******/

//fit_t subj_tour_lenght(Population *pop, subj_t subj_i, Town *t_list){
//    /* length of the tour */
//}


void pop_destroy(Population * p){
    free(p->pop);
    free(p);
}
