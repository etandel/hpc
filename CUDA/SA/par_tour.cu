#include "tour.h"
#include "config.h"
#include "town.h"
#include "pools.h"
#include <stdlib.h>

#define NBLOCKS 5
#define THREADS_PER_BLOCK (NUM_VERTEXES/NBLOCKS)

Tour * tour_new(TownList *towns, town_index_t num_vertexes){
    Tour * newtour = (Tour*) malloc(sizeof(Tour));
    newtour->tour = (town_index_t *) malloc(sizeof(town_index_t)*num_vertexes);
    newtour->town_list = towns;
    newtour->num_vertexes = num_vertexes;
    return newtour;
}

void tour_randomize(Tour * tour) {
    town_index_t i, num_vertexes=tour->num_vertexes;
    TownPool *tp = tp_new(num_vertexes);
    
    for (i=0; i<num_vertexes; i++){
        tour->tour[i] = tp_random_town(tp);
    }

    tp_destroy(tp);
}

Tour * tour_destroy(Tour * tour){
    free(tour->tour);
    free(tour);
    return tour = NULL;
}

static Tour * d_tour_new(Tour *h_t){
    Tour *d_t;
    cudaMalloc((void**)&d_t, sizeof(Tour));
    cudaMalloc((void**)&d_t->tour, NUM_VERTEXES*sizeof(town_index_t));
    d_t->town_list = h_t->town_list;
    return d_t;
}

__device__ double tl_distance(TownList * t_list, town_index_t i1, town_index_t i2){
    Town *t1 = &t_list->list[i1], *t2 = &t_list->list[i2];
    double dx = t2->x - t1->x;
    double dy = t2->y - t1->y;
    return sqrt(dx*dx + dy*dy);
}

static __global__ void calc_len(Tour * t, double *part_len){
    __shared__ double s_c[THREADS_PER_BLOCK];

	int tx = threadIdx.x;
	unsigned i = blockIdx.x * blockDim.x + threadIdx.x;

	// Initialize shared memory.
	s_c[tx] = tl_distance(t->town_list, i, (i+1)%NUM_VERTEXES);
	__syncthreads();

	// Reduce in shared memory.
	// Good version (v2)
	int stride = THREADS_PER_BLOCK >> 1 ;
	for (  ; tx < stride; stride >>= 1 )	
	{		
	s_c[tx] += s_c[tx + stride];
	__syncthreads();
	}

	// Thread zero writes the result to global memory.
	if ( tx < 1 )
		part_len[ blockIdx.x ] = s_c[ 0 ] ; // v1
}

double tour_set_len(Tour * tour){
    Tour *d_tour;
    double len = 0, h_part_len[NBLOCKS], *d_part_len;
    int i;

    d_tour = d_tour_new(tour);
    cudaMalloc((void**)&d_part_len, NBLOCKS*sizeof(double));

    calc_len<<<NBLOCKS, THREADS_PER_BLOCK>>>(d_tour, d_part_len);
    cudaMemcpy(h_part_len, d_part_len, NBLOCKS*sizeof(double), cudaMemcpyDeviceToHost);

    for (i=0; i<NBLOCKS; i++)
        len += h_part_len[i];

    cudaFree(d_part_len);

    tour->length = len;
    return len;
}

void tour_mutate(Tour *new_tour, Tour *old_tour) {
    town_index_t i;
    TownPool *tp;

    tp = tp_new(NUM_VERTEXES);

    for (i=0; i<NUM_VERTEXES; i++){
        new_tour->tour[i] = old_tour->tour[i];
    }

    {
        town_index_t i1, i2;
        i1 = (town_index_t)tp_random_town(tp);
        i2 = (town_index_t)tp_random_town(tp);
        new_tour->tour[i1] = old_tour->tour[i2];
        new_tour->tour[i2] = old_tour->tour[i1];
    }

    tp_destroy(tp);
}
