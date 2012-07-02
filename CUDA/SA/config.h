#ifndef _CONFIG
#define _CONFIG

/* type definitions of below params */

//defaults of config values
#define GRID_SIZE      100   // grid goes from 0 to 255 in both axes
#define NUM_VERTEXES   500  // max number of towns on a tour
#define ALPHA          0.99 //rate of cooling (temperature *= alpha)
#define EPSILON        0.1   // minimum temperature (stopping condition)
#define TEMPERATURE    10000  //initial temperature

typedef unsigned int coord_t; //type of coordinates (>= GRID_SIZE)
typedef int town_index_t; //type for indexing town list ( > NUM_VERTEXES)

struct config {
    coord_t      grid_size;
    town_index_t num_vertexes;
    double       alpha;
    double       epsilon;
    double       temperature;
};
typedef struct config Config;

#endif
