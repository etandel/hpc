#ifndef _CONFIG
#define _CONFIG

/*Here are the importat constants. Changing some of them may require changes
on type definitions below (marked with an *). */

#define GRID_SIZE      255  // grid goes from 0 to 255 in both axes *
#define NUM_VERTEXES   255  // number of towns and genes *
#define POP_SIZE       1000 // size of population must be even *
#define CROSS_PROB     0.9  // probability of crossover occuring
#define MUTATION_PROB  0.01 //probabily of mutation occuring
// number of generations not improved that stop execution (<= max_int)
#define STAG_COUNT     500
// number proportional to the severeness of fitness evaluation
#define FIT_PRECISION  10000000



/* type definitions dependent on above constants */

typedef unsigned char coord_t; //type of coordinates (>= GRID_SIZE)
typedef unsigned short int gene_t; //type for indexing town list ( > NUM_VERTEXES)
typedef unsigned int pop_index_t; //type for indexing subjects on pop. (>= POP_SIZE)
typedef double fit_t; //type for fitness (double strongly recommended)

#endif
