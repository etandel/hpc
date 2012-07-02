#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "pools.h"
#include "config.h"
#include "town.h"
#include "tour.h"


/********************** POOLS.CU ***************************/

struct townpool{
    town_index_t num_indexes;
    town_index_t n_unused;
    unsigned char *arr;
};

TownPool * tp_new(town_index_t num_indexes){
    size_t size = (size_t) ceil(num_indexes/8.0);
    TownPool *tp =  (TownPool*) malloc(sizeof(TownPool));
    tp->arr = (unsigned char *) malloc(size);
    memset(tp->arr, 0xff, size);

    tp->n_unused    = num_indexes;
    tp->num_indexes = num_indexes;

    return tp; 
}

TownPool * tp_destroy(TownPool *tp){
    free(tp->arr);
    free(tp);
    return tp = NULL;
}

int tp_usable(TownPool *tp, town_index_t i){
    return ((unsigned char)1 << i%8 & tp->arr[i/8]) != 0;
}

int tp_unusable(TownPool *tp, town_index_t i){
    return ((unsigned char)1 << i%8 & tp->arr[i/8]) == 0;
}

void tp_remove(TownPool *tp, town_index_t i){
    unsigned char mask = (unsigned char)1 << i%8, current = tp->arr[i/8];
    tp->arr[i/8] = current & mask ? current ^ mask : current;
}

void tp_reset(TownPool *tp){
    memset(tp->arr, 0xff, (size_t)ceil(tp->num_indexes/8.0));
}

#define random_town() (rand() % NUM_VERTEXES)
town_index_t tp_random_town(TownPool *tp){
    town_index_t index = -1;
    if (tp->n_unused > 0){
        do {
            index = ((town_index_t)rand()) % tp->num_indexes;
        } while(tp_unusable(tp, index));
        tp_remove(tp, index);
        tp->n_unused--;
    }
    return index;
}


/********************** TOWN.CU ***************************/


TownList * tl_new(Config config){
    TownList * newt = (TownList *) malloc(sizeof(TownList));
    newt->list = (Town*) malloc(NUM_VERTEXES * sizeof(Town));
    newt->config = config;
    return newt;
}

#define RANDOM_COORD(min, max) (coord_t)(rand() % (max - min) + min);
static void add_random_town(TownList * t_list, town_index_t i){
    t_list->list[i].x = RANDOM_COORD(0, t_list->config.grid_size);
    t_list->list[i].y = RANDOM_COORD(0, t_list->config.grid_size);
}

void tl_randomize(TownList *tl){
    town_index_t i;
    for (i=0; i < NUM_VERTEXES; i++)
        add_random_town(tl, i);
}

TownList * tl_destroy(TownList * t_list){
    free(t_list->list);
    free(t_list);
    return t_list = NULL;
}


/********************** TOUR.CU ***************************/


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

static town_index_t * d_tour_new(Tour *h_t){
    town_index_t *d_t;
    cudaMalloc((void**)&d_t, NUM_VERTEXES*sizeof(town_index_t));
    cudaMemcpy(d_t, h_t->tour, NUM_VERTEXES*sizeof(town_index_t), cudaMemcpyHostToDevice);
    return d_t;
}

__device__ double tl_distance(TownList * t_list, town_index_t i1, town_index_t i2){
    Town *t1 = &t_list->list[i1], *t2 = &t_list->list[i2];
    double dx = t2->x - t1->x;
    double dy = t2->y - t1->y;
    return sqrt(dx*dx + dy*dy);
}

static __global__ void calc_len(TownList *tl, town_index_t *t, double *part_len){
    __shared__ double s_c[THREADS_PER_BLOCK];

	int tx = threadIdx.x;
	unsigned i = blockIdx.x * blockDim.x + threadIdx.x;

	// Initialize shared memory.
	s_c[tx] = tl_distance(tl, t[i], t[(i+1)%NUM_VERTEXES]);
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
    town_index_t *d_tour;
    double len = 0, h_part_len[NBLOCKS], *d_part_len;
    int i;

    d_tour = d_tour_new(tour);
    cudaMalloc((void**)&d_part_len, NBLOCKS*sizeof(double));

    calc_len<<<NBLOCKS, THREADS_PER_BLOCK>>>(tour->town_list, d_tour, d_part_len);
    cudaMemcpy(h_part_len, d_part_len, NBLOCKS*sizeof(double), cudaMemcpyDeviceToHost);

    for (i=0; i<NBLOCKS; i++)
        len += h_part_len[i];

    cudaFree(d_part_len);
    cudaFree(d_tour);

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

/********************** MAIN.CU ***************************/

#define cron_begin clock
#define cron_end(start) ((clock()-start)/CLOCKS_PER_SEC)

int should_replace(Tour *newt, Tour *old, double temperature){
    double dl = newt->length - old->length;
    return dl<0 ? 1 : rand()/RAND_MAX < exp(-dl/temperature);
}


int main(const int argc, const char *argv[]){
    Config state = {
        GRID_SIZE,
        NUM_VERTEXES,
        ALPHA,
        EPSILON,
        TEMPERATURE
    };

    TownList * h_towns, *d_towns;
    Tour *old_tour, *new_tour;
    int i = 0;
    double start;

    //puts("Executing sequential version");

    srand((int)time(NULL));

    start = cron_begin();

    h_towns = tl_new(state);
    tl_randomize(h_towns);
    cudaMalloc((void**)&d_towns, NUM_VERTEXES*sizeof(Town));
    cudaMemcpy(d_towns, h_towns, NUM_VERTEXES*sizeof(Town), cudaMemcpyHostToDevice);
    tl_destroy(h_towns);

    old_tour = tour_new(d_towns, NUM_VERTEXES); 
    tour_randomize(old_tour);
    tour_set_len(old_tour);

    new_tour = tour_new(d_towns, NUM_VERTEXES);
    while (state.temperature > state.epsilon){
        i++;
        tour_mutate(new_tour, old_tour);
        tour_set_len(new_tour);
        if (should_replace(new_tour, old_tour, state.temperature)){
            Tour * dummy = old_tour;
            old_tour = new_tour;
            new_tour = dummy;
        }
        
        state.temperature *= state.alpha;
    }
    

    printf("Sequential time: %f\n", cron_end(start));
    //printf("After %d iterations, the best length: %f\n", i, old_tour->length);

    tour_destroy(old_tour);
    tour_destroy(new_tour);
    cudaFree(d_towns);
    return 0;
}
