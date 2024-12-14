#!/bin/bash
# filename: NodeCountVSThroughput

pathStatic="scratch/static"
pathMobile="scratch/mobile"

# Node count vs Throughput
for i in 20 40 60 80 100
do
    ./ns3 run "scratch/1905086_1.cc --nNode=$i --nFlow=10 --no_packets_persec=100 --coverage_area=1 --plot_category=1 --plot_number=1"
    ./ns3 run "scratch/1905086_2.cc --nNode=$i --nFlow=10 --no_packets_persec=100 --speed=1 --plot_category=1 --plot_number=1"
done

./scratch/1905086_SimulationPlot.sh $pathStatic NodeCount Throughput
./scratch/1905086_SimulationPlot.sh $pathMobile NodeCount Throughput

# Flow vs Throughput
for i in 10 20 30 40 500
do
    ./ns3 run "scratch/1905086_1.cc --nNode=20 --nFlow=$i --no_packets_persec=100 --coverage_area=1 --plot_category=1 --plot_number=2"
    ./ns3 run "scratch/1905086_2.cc --nNode=20 --nFlow=$i --no_packets_persec=100 --speed=1 --plot_category=1 --plot_number=2"
done

./scratch/1905086_SimulationPlot.sh $pathStatic FlowCount Throughput
./scratch/1905086_SimulationPlot.sh $pathMobile FlowCount Throughput

# no_packet_per_sec vs Throughput
for i in 100 200 300 400 500
do
    ./ns3 run "scratch/1905086_1.cc --nNode=20 --nFlow=10 --no_packets_persec=$i --coverage_area=1 --plot_category=1 --plot_number=3"
    ./ns3 run "scratch/1905086_2.cc --nNode=20 --nFlow=10 --no_packets_persec=$i --speed=1 --plot_category=1 --plot_number=3"
done

./scratch/1905086_SimulationPlot.sh $pathStatic PacketCount Throughput
./scratch/1905086_SimulationPlot.sh $pathMobile PacketCount Throughput

# coverage area/speed vs Throughput
for i in 1 2 3 4 5
do
    ./ns3 run "scratch/1905086_1.cc --nNode=20 --nFlow=10 --no_packets_persec=100 --coverage_area=$i --plot_category=1 --plot_number=4"
    j=$(expr $i \* 5)
    ./ns3 run "scratch/1905086_2.cc --nNode=20 --nFlow=10 --no_packets_persec=100 --speed=$j --plot_category=1 --plot_number=4"
done

./scratch/1905086_SimulationPlot.sh $pathStatic Coverage Throughput
./scratch/1905086_SimulationPlot.sh $pathMobile Speed Throughput

# Node count vs Packet delivery ratio
for i in 20 40 60 80 100
do
    ./ns3 run "scratch/1905086_1.cc --nNode=$i --nFlow=10 --no_packets_persec=100 --coverage_area=1 --plot_category=2 --plot_number=1"
    ./ns3 run "scratch/1905086_2.cc --nNode=$i --nFlow=10 --no_packets_persec=100 --speed=1 --plot_category=2 --plot_number=1"
done

./scratch/1905086_SimulationPlot.sh $pathStatic NodeCount PacketDeliveryRatio
./scratch/1905086_SimulationPlot.sh $pathMobile NodeCount PacketDeliveryRatio

# Flow vs Packet delivery ratio
for i in 10 20 30 40 500
do
    ./ns3 run "scratch/1905086_1.cc --nNode=20 --nFlow=$i --no_packets_persec=100 --coverage_area=1 --plot_category=2 --plot_number=2"
    ./ns3 run "scratch/1905086_2.cc --nNode=20 --nFlow=$i --no_packets_persec=100 --speed=1 --plot_category=2 --plot_number=2"
done

./scratch/1905086_SimulationPlot.sh $pathStatic FlowCount PacketDeliveryRatio
./scratch/1905086_SimulationPlot.sh $pathMobile FlowCount PacketDeliveryRatio

# no_packet_per_sec vs Packet delivery ratio
for i in 100 200 300 400 500
do
    ./ns3 run "scratch/1905086_1.cc --nNode=20 --nFlow=10 --no_packets_persec=$i --coverage_area=1 --plot_category=2 --plot_number=3"
    ./ns3 run "scratch/1905086_2.cc --nNode=20 --nFlow=10 --no_packets_persec=$i --speed=1 --plot_category=2 --plot_number=3"
done

./scratch/1905086_SimulationPlot.sh $pathStatic PacketCount PacketDeliveryRatio
./scratch/1905086_SimulationPlot.sh $pathMobile PacketCount PacketDeliveryRatio

# coverage area/speed vs Packet delivery ratio
for i in 1 2 3 4 5
do
    ./ns3 run "scratch/1905086_1.cc --nNode=20 --nFlow=10 --no_packets_persec=100 --coverage_area=$i --plot_category=2 --plot_number=4"
    j=$(expr $i \* 5)
    ./ns3 run "scratch/1905086_2.cc --nNode=20 --nFlow=10 --no_packets_persec=100 --speed=$j --plot_category=2 --plot_number=4"
done

./scratch/1905086_SimulationPlot.sh $pathStatic Coverage PacketDeliveryRatio
./scratch/1905086_SimulationPlot.sh $pathMobile Speed PacketDeliveryRatio
