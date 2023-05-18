set title 'Comparação da onda de entrada e saída'
set xlabel 'Amostra'
set ylabel 'Amplitude'
plot 'data.txt' using 1:2 with lines title 'Entrada',                              'data.txt' using 1:3 with lines title 'Saída'
pause mouse close
