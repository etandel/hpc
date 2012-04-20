#ifndef _POOLS_H
#define _POOLS_H

#include "config.h"
#include "population.h"

typedef unsigned char * GenePool;

GenePool pool_gene_new(void); //creates new pool filled with trues
void pool_gene_destroy(GenePool); //destroys pool

//confirms if this gene hasn't been used
#define pool_gene_usable(gp, g) (((unsigned char)1 << g%8 & gp[g/8]) != 0)
//confirms if this gene has been used
#define pool_gene_unusable(gp, g) (((unsigned char)1 << g%8 & gp[g/8]) == 0)

void pool_gene_reset(GenePool); //resets UNDESTROYED pool
void pool_gene_remove(GenePool, gene_t); //removes gene from pool
gene_t pool_gene_get_random(GenePool); //returns random gene and removes it from pool

#endif
