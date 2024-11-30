#!/usr/bin/gnuplot -c

# Configurações Gerais
set encoding utf8
set grid
set style data linespoints
set style line 1 lc rgb "blue" pt 7 ps 1.5 lw 2
set style line 2 lc rgb "red" pt 5 ps 1.5 lw 2
set boxwidth 1
set xlabel "Número de Threads"
set xtics 1
set key top left

# Configurar o terminal interativo
set terminal qt persist

# Gráfico de Tempo por Thread
set title "Tempo em segundos por Thread - Partições = 1000"
set ylabel "Tempo (s)"
plot "TempoXThread.txt" using 1:2 title "Tempo por Thread" with linespoints linestyle 1

# Pausa para visualização do gráfico de Tempo
pause -1

# Gráfico de MEPs por Thread
set title "MEPs por Thread - Partições = 1000"
set ylabel "MEPs (Milhões de Elementos por Segundo)"
plot "MEPsXThread.txt" using 1:2 title "MEPs por Thread" with linespoints linestyle 2

# Pausa para visualização do gráfico de MEPs
pause -1

# Gerar gráficos PNG
set terminal png size 1024,768
set output "threads_vs_tempo.png"
set title "Tempo em segundos por Thread - Partições = 1000"
plot "TempoXThread.txt" using 1:2 title "Tempo por Thread" with linespoints linestyle 1
unset output

set output "threads_vs_meps.png"
set title "MEPs por Thread - Partições = 1000"
plot "MEPsXThread.txt" using 1:2 title "MEPs por Thread" with linespoints linestyle 2
unset output
