#!/bin/bash
echo "$0 rodando no host " `hostname`  
echo "$0 rodando no host " `hostname` >saida.txt

NTIMES=1
MAX_EXECS=1

# Limpar arquivos no início
echo "Marcador: Tempo por thread" > TempoXThread.txt
echo "Threads, Tempo" >> TempoXThread.txt
echo "Marcador: MEPs por thread" > MEPsXThread.txt
echo "Threads, MEPs" >> MEPsXThread.txt

# Loop de threads
for i in 1 2 3 4 5 6 7 8
do
    for j in $(seq 1 $NTIMES);
    do
        if [ $j -le $MAX_EXECS ]; then
            # Executar e capturar saída
            output=$(./multi-partition $1 $i)

            # Extração de valores
            tempo=$(echo "$output" | grep -Eo 'Total time: [^ ]*' | awk '{print $3}')
            throughput=$(echo "$output" | grep -Eo 'Throughput: [^ ]*' | awk '{print $2}')

            # Verificar se os valores foram capturados corretamente
            if [ -z "$tempo" ]; then
                tempo="erro"
            fi
            if [ -z "$throughput" ]; then
                throughput="erro"
            fi

            # Escrever nos arquivos
            echo "$i, $tempo" >> TempoXThread.txt
            echo "$i, $throughput" >> MEPsXThread.txt
            echo "$i, $tempo" >> saida.txt
        else
            echo "nao executado" | tee -a saida.txt 
        fi  
    done
done

# Exibir informações do SLURM (se disponível)
squeue -j $SLURM_JOBID
