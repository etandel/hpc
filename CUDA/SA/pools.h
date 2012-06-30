#ifndef _POOLS_H
#define _POOLS_H

#include "config.h"

typedef struct townpool TownPool;

TownPool * tp_new(town_index_t); //creates new pool filled with trues
TownPool * tp_destroy(TownPool *); //destroys pool and returns NULL;

//confirms if this town hasn't been used
int tp_usable(TownPool *, town_index_t);
//confirms if this town has been used
int tp_unusable(TownPool *, town_index_t);

void tp_reset(TownPool *); //resets UNDESTROYED pool
void tp_remove(TownPool *, town_index_t); //removes town from pool
//returns random usable town and removes it from pool
town_index_t tp_random_town(TownPool *);

#endif
