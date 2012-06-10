#!/bin/bash

ntests=10
param_nvertex=

############ Sequential ###########
echo -e "Executanto versao sequencial $ntests vezes.\n"
seq_time=0
for i in $(seq $ntests); do
    output=$(./sequential $param_nvertex)
    echo $output
    seq_time=$seq_time+$(echo $output | cut -d' ' -f3)
done
seq_time=$(echo $seq_time | bc)
echo -e "Total time: $seq_time"


############ Parallel 8 ###########
echo -e "\nExecutanto versao paralela com 8 threads $ntests vezes."
par8_time=0
for i in $(seq $ntests); do
    output=$(./parallel -t 8 $param_nvertex)
    echo $output
    par8_time=$par8_time+$(echo $output | cut -d' ' -f3)
done
par8_time=$(echo $par8_time | bc)
echo "Total time: $par8_time"
echo "Speedup 8: $(echo -e "scale=10\n"$seq_time/$par8_time | bc)"


############ Parallel 4 ###########
echo -e "\nExecutanto versao paralela com 4 threads $ntests vezes."
par4_time=0
for i in $(seq $ntests); do
    output=$(./parallel -t 4 $param_nvertex)
    echo $output
    par4_time=$par4_time+$(echo $output | cut -d' ' -f3)
done
par4_time=$(echo $par4_time | bc)
echo "Total time: $par4_time"
echo "Speedup 4: $(echo -e "scale=10\n"$seq_time/$par4_time | bc)"
