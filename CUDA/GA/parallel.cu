#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "config.h"
#include "pools.h"
#include "town.h"
#include "population.h"

Subject *d_subjs;

/******************** POOLS.C **********************/

struct genepool {
    unsigned char *pool;
    gene_t *seq;
    gene_t i;
    gene_t pool_size;
    gene_t seq_size;
};

GenePool gp_new(void){
    gene_t i;
    GenePool gp   = (GenePool) malloc(sizeof(struct genepool));
    gp->i         = 0;
    gp->seq_size  = NUM_VERTEXES;
    gp->seq       = (gene_t *) malloc(gp->seq_size*sizeof(gene_t));

    gp->pool_size = (size_t)ceil(gp->seq_size/8.0);
    gp->pool      = (unsigned char *) malloc(gp->pool_size);
    memset(gp->pool, 0xff, gp->pool_size);

    for (i=0; i<gp->seq_size; i++)
        gp->seq[i] = i;

    gp_reset(gp);
    return gp; 
}

void gp_destroy(GenePool gp){
    free(gp->seq);
    free(gp->pool);
    free(gp);
}

int gp_usable(GenePool gp, gene_t g){
    return (((unsigned char)1 << g%8 & gp->pool[g/8]) != 0);
}

int gp_unusable(GenePool gp, gene_t g){
    return (((unsigned char)1 << g%8 & gp->pool[g/8]) == 0);
}


void gp_remove(GenePool gp, gene_t g){
    unsigned char mask = (unsigned char)1 << g%8, current = gp->pool[g/8];
    gp->pool[g/8] = current & mask ? current ^ mask : current;
}

void gp_reset(GenePool gp){
    int i;

    for (i=gp->seq_size-1; i>0; i--){
        gene_t j = rand() % (i+1);
        gene_t tmp = gp->seq[i];
        gp->seq[i] = gp->seq[j];
        gp->seq[j] = tmp;
    }
}

gene_t gp_get_random(GenePool gp){
    gene_t g = gp->seq[gp->i++];
    gp_remove(gp, g);
    return g;
}


/******************** TOWN.C **********************/


Town * tl_new(gene_t nvertex){
    Town * newt;
    nvertex = nvertex ? nvertex : NUM_VERTEXES;
    newt = (Town *) malloc(NUM_VERTEXES * sizeof(Town));
    return newt;
}

static coord_t random_coord(coord_t min, coord_t max){
    return (coord_t)(rand() % (max - min) + min);
}

static void add_random_town(Town * t_list, int i){
    t_list[i].x = random_coord(0, GRID_SIZE);
    t_list[i].y = random_coord(0, GRID_SIZE);
}

void tl_randomize(Town *town){
    gene_t i;
    for (i=0; i<NUM_VERTEXES; i++)
        add_random_town(town, i);
}

void tl_destroy(Town * t_list){
    free(t_list);
}

fit_t tl_distance(Town * t_list, gene_t g1, gene_t g2){
    Town *t1 = &t_list[g1], *t2 = &t_list[g2];
    fit_t dx = (fit_t) (t2->x - t1->x);
    fit_t dy = (fit_t) (t2->y - t1->y);
    return (fit_t) sqrt((double)(dx*dx + dy*dy));
}

/******************** POPULATION.C **********************/

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


/******************** MAIN.C **********************/

#define cron_begin clock
#define cron_end(start) ((clock() - start) / CLOCKS_PER_SEC)

int main (const int argc, const char * argv[]){
    Town *t_list, *d_tlist;
    Population *parents, *children, *dummy;
    fit_t max_fitness = FIT_MIN;
    Subject fittest;
    int stag_count=0, iter=0;

    double start;
    
    //puts("Executing parallel version");

    srand((int)time(NULL)); //seed pseudo-rand generator
    start = cron_begin();

    // initialization
    t_list = tl_new(NUM_VERTEXES);
    tl_randomize(t_list);
    cudaMalloc((void**)&d_tlist, NUM_VERTEXES*sizeof(Town));
    cudaMemcpy(d_tlist, t_list, NUM_VERTEXES*sizeof(Town), cudaMemcpyHostToDevice);
    tl_destroy(t_list);

    cudaMalloc((void**)&d_subjs, POP_SIZE*sizeof(Subject));

    parents  = pop_new(d_tlist);
    pop_randomize(parents);
    pop_set_fit(parents);
    max_fitness = parents->max_fitness;
    fittest     = parents->pop[parents->fittest];

    children = pop_new(d_tlist);
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

    printf("Parallel iterations / time = %f\n", iter / cron_end(start));
    /*
        printf("Parallel iterations: %d ; parallel time: %f seconds.\n", iter, MPI_Wtime() - start);
        printf("length: %.17f\n", subj_tour_length(&fittest, t_list));
    // */
    cudaFree(d_tlist);
    pop_destroy(parents);
    pop_destroy(children);
    cudaFree(d_subjs);

    return 0;
}
