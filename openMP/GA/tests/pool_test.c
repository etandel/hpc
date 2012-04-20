#include "pools.h"
#include "config.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>

void dump(unsigned char * p, int size){
    int i;
    for (i=0; i<size; i++)
        printf("%.8x\t%.2x\n", p+i, p[i]);
}

int main(void){
    GenePool gp = pool_gene_new();
    int i;
    unsigned char g;

    srand((int)time(NULL));

    for (i=0; i<NUM_VERTEXES; i++){
        printf("%d\n", pool_gene_get_random(gp));
    }
    return 0;
}




