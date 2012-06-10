#ifndef _POOLS_H
#define _POOLS_H

#include "config.h"
#include "population.h"

typedef struct genepool * GenePool;

GenePool gp_new(void); //creates new pool filled with trues
void gp_destroy(GenePool); //destroys pool

//confirms if this gene hasn't been used
int gp_usable(GenePool gp, gene_t g);

//confirms if this gene has been used
int gp_unusable(GenePool gp, gene_t g);

void gp_reset(GenePool); //resets UNDESTROYED pool
void gp_remove(GenePool, gene_t); //removes gene from pool
gene_t gp_get_random(GenePool); //returns random gene and removes it from pool

#endif
