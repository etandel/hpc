#!/bin/bash

ntests=4

############ Sequential ###########
echo -e "Executanto versao sequencial $ntests vezes.\n"
seq_time=0
for i in $(seq $ntests); do
    output=$(./sequential)
    echo $output
    seq_time=$seq_time+$(echo $output | cut -d' ' -f3)
done
seq_time=$(echo $seq_time | bc)
echo -e "Total time: $seq_time"


############ Parallel ###########
echo -e "\nExecutanto versao paralela $ntests vezes."
par_time=0
for i in $(seq $ntests); do
    output=$(./parallel)
    echo $output
    par_time=$par_time+$(echo $output | cut -d' ' -f3)
done
par_time=$(echo $par_time | bc)
echo "Total time: $par_time"
echo "Speedup: $(echo -e "scale=10\n"$seq_time/$par_time | bc)"
