#!/usr/bin/gnuplot -c

# Configurações Gerais
set encoding utf8
set grid
set style data linespoints
set style line 1 lc rgb "blue" pt 7 ps 1.5 lw 2
set boxwidth 1
set xlabel "Número de Threads"
set ylabel "Tempo (s)"
set title "Tempo em segundos por Thread - Partições = 1000"
set key top left

# Formato do arquivo de entrada
set datafile separator ","

# Configurar o terminal
set terminal qt persist

# Plotando os dados
plot "saida.txt" using 1:2 title "Tempo por Thread" with linespoints linestyle 1

# Gerar arquivo PNG
set terminal png size 1024,768
set output "threads_vs_tempo.png"
replot
unset output

pause -1
