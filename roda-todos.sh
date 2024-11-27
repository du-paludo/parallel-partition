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
MAX_EXECS=10
echo "MAX_EXECS " $MAX_EXECS

# Loop para as threads (1 a 4)
for i in 1 2 3 4;
do
    echo "Executando $NTIMES vezes com $1 elementos e $i threads:"
    
    total_time=0  # Variável para armazenar o tempo total de execução para cada número de threads
    total_throughput=0  # Variável para armazenar o throughput total

    for j in $(seq 1 $NTIMES);
    do
        echo "-----------------------" >>saida.txt
        if [ $j -le $MAX_EXECS ];
        then 
          # Executa o comando e captura tanto o tempo quanto o throughput
          result=$(./multi-partition $1 $i | tee -a saida.txt)
          
          # Extrai o tempo e throughput da saída
          time=$(echo "$result" | grep -Eo 'total_time_in_seconds: [^ ]*' | awk '{print $2}')
          throughput=$(echo "$result" | grep -Eo 'Throughput: [^ ]*' | awk '{print $2}')

          # Exibe o tempo e throughput para cada execução
          echo "Execucao $j para $i threads - Tempo: $time segundos, Throughput: $throughput" >>saida.txt
          echo "Execucao $j para $i threads - Tempo: $time segundos, Throughput: $throughput"

          # Acumula o tempo total e throughput total
          total_time=$(echo "$total_time + $time" | bc)
          total_throughput=$(echo "$total_throughput + $throughput" | bc)
        else
          echo "nao executado" | tee -a saida.txt 
        fi  
    done

    # Calcula o tempo médio e throughput médio
    avg_time=$(echo "scale=2; $total_time / $NTIMES" | bc)
    avg_throughput=$(echo "scale=2; $total_throughput / $NTIMES" | bc)

    # Exibe as médias para cada número de threads
    echo "Tempo médio para $i threads: $avg_time segundos, Throughput médio: $avg_throughput" >>saida.txt
    echo "Tempo médio para $i threads: $avg_time segundos, Throughput médio: $avg_throughput"
done

echo "O tempo total dessa shell foi de" $SECONDS "segundos"
echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE

# Imprime informações do job SLURM
squeue -j $SLURM_JOBID
