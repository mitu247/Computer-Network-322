#!/bin/bash

path="scratch/plot"


if [[ $1 = "1" ]]; then
    label1="Throughput"
    label2="Bottleneck DataRate"
elif [[ $1 = "2" ]]; then
    label1="Throughput"
    label2="PacketLossRate"
else
    label1="Congestion Window"
    label2="Time"
fi

model_t="$2"

gnuplot -persist <<-EOFMarker
    set terminal png size 640,480
    set key box
    set key width 1
    set key font "Arial,14"
    set grid
    set border 3
    set tics nomirror
    set output "$path/${label1}VS${label2}${model_t}.png"  # Use double quotes around the path
    set xlabel "${label2}"
    set ylabel "${label1}"
    plot "${path}/1.data" using 1:2 with linespoints title 'TCP New Reno', "${path}/2.data" using 1:2 with linespoints title "${model_t}"
EOFMarker
