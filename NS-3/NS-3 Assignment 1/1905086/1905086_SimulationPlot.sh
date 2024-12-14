#!/bin/bash

path="$1"
label1="$2"
label2="$3"

gnuplot -persist <<-EOFMarker
    # Node vs Throughput
    set terminal png size 640,480
    set output "${path}/${label1}VS${label2}.png"  # Use double quotes around the path
    set xlabel "${label1}"
    set ylabel "${label2}"
    plot "${path}/${label1}VS${label2}.dat" using 1:2 title '${label1} VS ${label2}' with linespoints
EOFMarker