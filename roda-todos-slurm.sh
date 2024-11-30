#!/bin/bash
echo "USAGE: ./rodaTodos.sh <nElements>"
echo "$0 rodando no host " `hostname`  
echo "$0 rodando no host " `hostname` >saida.txt

echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE

NTIMES=10
echo "nt " $NTIMES
MAX_EXECS=5
echo "MAX_EXECS " $MAX_EXECS

for i in 1 2 3 4 5 6 7 8
do
    echo "-----------------------" >> saida.txt
    echo "Executando $NTIMES vezes com $1 elementos e $i threads:"
    echo "Executando $NTIMES vezes com $1 elementos e $i threads:" >> saida.txt
    for j in $(seq 1 $NTIMES);
    do
        if [ $j -le $MAX_EXECS ];
        then 
        #   ./multi-partition $1 $i | grep -oP '(?<=Total Time: )[^ ]*'
            tempo=$(./multi-partition $1 $i | awk '/Total time:/ {print $3}') 
            echo "$i, $tempo " >> saida.txt
            echo "$i, $tempo "

          #./reduceSumPth $1 $i 
        else
          echo "nao" | tee -a saida.txt 
        fi  
    done
done
echo "O tempo total dessa shell foi de" $SECONDS "segundos"
echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE
#imprime infos do job slurm (e.g. TEMPO até aqui no fim do job)
squeue -j $SLURM_JOBID




