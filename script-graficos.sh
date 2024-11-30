#!/bin/bash
echo "$0 rodando no host " `hostname`  
echo "$0 rodando no host " `hostname` >saida.txt

NTIMES=1
MAX_EXECS=1

# echo "$1" >> saida.txt
echo "Marcador "Tempo por thread" " >> saida.txt
echo "Threads, Tempo " >> saida.txt
for i in 1 2 3 4 5 6 7 8
do
    # echo "Executando $NTIMES vezes com $1 elementos e $i threads:"
    for j in $(seq 1 $NTIMES);
    do
        if [ $j -le $MAX_EXECS ];
        then 
        #   ./multi-partition $1 $i | grep -oP '(?<=Total Time: )[^ ]*'
            tempo=$(./multi-partition $1 $i | awk '/Total time:/ {print $3}')
            echo "$i, $tempo " >> saida.txt

          #./reduceSumPth $1 $i 
        else
          echo "nao executado" | tee -a saida.txt 
        fi  
    done
done
#imprime infos do job slurm (e.g. TEMPO até aqui no fim do job)
squeue -j $SLURM_JOBID




