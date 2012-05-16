##1) About
This is my code for the classes of Introduction to High Performance Computing
at Pontifical Catholic University of Rio de Janeiro (PUC-Rio).

The dirs are divided in themes (openMP, openMPI, CUDA etc.) and inside them there are the implementation of the Traveling Salesman Problem in different methods:

GA -> Genetic Algorithm
SA -> Simulated Annealing


##2) Instalation
Every implementation has the same processes, but the following examples are for openMP/SA:
* Go to implementation directory:
    cd openMP/SA #asumming you want to test the openMP implementation of SA
* Configure:
    make config
* Compile:
    make            #if you want to build all
    make sequential #if you want to build only sequential
    make parallel   #if you want to build only parallel

##3) Execution and Benchmarking
To execute each version (sequential or parallel) individually, do
    ./sequential
    ./parallel

If you want to execute both and compare time results (benchmark), do
    ./benchmark

##4) Authors
Elias Tandel Barrionovo \<elias 'dot' tandel 'at' gmail 'dot' com \> and Marcelo Neves.
