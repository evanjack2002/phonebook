reset
set ylabel 'time(sec)'
set style fill solid
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [:][:0.100]'output.txt' \
using 2:xtic(1) with histogram title 'original', \
'' using ($0-0.2):($2+0.002):2 with labels title '', \
'' using 3:xtic(1) with histogram title 'optimized' , \
'' using ($0-0.03):($3+0.002):3 with labels title '', \
'' using 4:xtic(1) with histogram title 'hashtable1' , \
'' using ($0+0.45):($4+0.002):4 with labels title '', \
'' using 5:xtic(1) with histogram title 'hashtable2' , \
'' using ($0+0.45):($5+0.002):5 with labels title ''
