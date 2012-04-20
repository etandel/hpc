#!/bin/bash
gcc -o ptst -I/home/echobravo/Projects/HPC/trabalho/openMP/GA ../town.c ../pools.c ../population.c pool_test.c -lm && ./ptst
