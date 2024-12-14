#!/bin/bash

plot="scratch/plot"
mkdir -p $plot
clear() {
    rm -rf "$plot/1.data"
    rm -rf "$plot/2.data"
    touch "$plot/1.data"
    touch "$plot/2.data"
}

for i in 1 50 100 150 200 250 300
do
./ns3 run "scratch/1905086.cc --bottleneck_datarate=$i --packet_loss_rate=1e-6 --modelType="TcpHighSpeed" --plotType=1"
done

./scratch/1905086_1.sh 1 "TCPHighSpeed"

clear

for i in 1 50 100 150 200 250 300
do
./ns3 run "scratch/1905086.cc --bottleneck_datarate=$i --packet_loss_rate=1e-6 --modelType="MyTcpAdaptiveReno" --plotType=1"
done

./scratch/1905086_1.sh 1 "TCPAdaptiveReno"

clear

for i in 2 3 4 5 6
do
./ns3 run "scratch/1905086.cc --bottleneck_datarate=50 --packet_loss_rate=1e-$i --modelType="TcpHighSpeed" --plotType=2"
done

./scratch/1905086_1.sh 2 "TCPHighSpeed"

clear

for i in 2 3 4 5 6
do
./ns3 run "scratch/1905086.cc --bottleneck_datarate=50 --packet_loss_rate=1e-$i --modelType="MyTcpAdaptiveReno" --plotType=2"
done

./scratch/1905086_1.sh 2 "TcpAdaptiveReno"

clear

./ns3 run "scratch/1905086.cc --bottleneck_datarate=50 --packet_loss_rate=1e-5 --modelType="TcpHighSpeed" --plotType=3"
./scratch/1905086_1.sh 3 "TCPHighSpeed"

clear
./ns3 run "scratch/1905086.cc --bottleneck_datarate=50 --packet_loss_rate=1e-5 --modelType="MyTcpAdaptiveReno" --plotType=3"
./scratch/1905086_1.sh 3 "TcpAdaptiveReno"

#rm -rf scratch/plot