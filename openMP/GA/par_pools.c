#include "config.h"
#include "pools.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define random_gene() (rand() % NUM_VERTEXES)

GenePool pool_gene_new(void){
    size_t size = (size_t)ceil(NUM_VERTEXES/8.0);
    GenePool gp = (GenePool) malloc(size);
    memset(gp, 0xff, size);
    return gp; 
}

void pool_gene_destroy(GenePool gp){
    free(gp);
}

void pool_gene_remove(GenePool gp, gene_t g){
    unsigned char mask = (unsigned char)1 << g%8, current = gp[g/8];
    gp[g/8] = current & mask ? current ^ mask : current;
}

void pool_gene_reset(GenePool gp){
    memset(gp, 0xff, (size_t)ceil(NUM_VERTEXES/8.0));
}

gene_t pool_gene_get_random(GenePool gp){
    gene_t gene;
    do {
        gene = random_gene();
    } while(pool_gene_unusable(gp, gene));
    pool_gene_remove(gp, gene);
    return gene;
}
