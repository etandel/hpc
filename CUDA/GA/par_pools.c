#include "config.h"
#include "pools.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
