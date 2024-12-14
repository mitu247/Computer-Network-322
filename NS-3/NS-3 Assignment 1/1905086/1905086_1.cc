/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"


// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;
#define PACKETSIZE 1024
#define WIRE_DATA_RATE "1Mbps"
#define WIRE_DELAY "1ms"
#define SIMULATION_TIME 6.0
uint32_t recPacketsCnt, sentPacketsCnt;
std::string path = "scratch/static";
Ptr<OutputStreamWrapper> stream;
std::ostringstream oss;

NS_LOG_COMPONENT_DEFINE("ThirdScriptExample");

static void
ReceivedPackets(Ptr< const Packet > packet, const Address &address) {
    ++recPacketsCnt;
}

static void
SentPackets(Ptr<const Packet> packet) {
    ++sentPacketsCnt;
}

double
CalculateMetric(ApplicationContainer sinks, int plotCat) {
    double val = 0.0;

    if (plotCat == 1) {
        // throughput
        for (uint32_t i=0; i < sinks.GetN(); i++) {
            Ptr<PacketSink> app = StaticCast<PacketSink>(sinks.Get(i));
            val += app->GetTotalRx() * 8;
        }
        val = val / (1e6 * SIMULATION_TIME); // Mbps
    }
    else if (plotCat == 2) {
        val = (1.0 * recPacketsCnt)/sentPacketsCnt;
    }

    return val;
}
static void
Plot(std::string file, uint32_t param, double metric){
    oss << path << "/" << file;
    
    AsciiTraceHelper ath;
    stream = ath.CreateFileStream(oss.str(), std::ios::app);

    *stream->GetStream() << param << "\t" << metric << std::endl;
}
int
main(int argc, char* argv[])
{
    uint32_t nNode = 20;
    uint32_t nFlow = 10;
    uint32_t no_packets_persec = 100;
    uint32_t coverage_area = 5;
    uint32_t senders;
    uint32_t receivers;
    uint32_t app_per_node = 1;
    int plotCat = 0; // 1 -> throughput, 2 -> packet delivery ratio
    int plotNo = 0; // 1 -> nodes, 2 -> flows, 3 -> packets, 4 -> coverage

    CommandLine cmd(__FILE__);
    cmd.AddValue("nNode", "Number of nodes", nNode);
    cmd.AddValue("nFlow", "Number of flows", nFlow);
    cmd.AddValue("no_packets_persec", "Number of packets sent per second", no_packets_persec);
    cmd.AddValue("coverage_area", "coverage area", coverage_area);
    cmd.AddValue("plot_category", "plot category", plotCat);
    cmd.AddValue("plot_number", "plot number", plotNo);

    cmd.Parse(argc, argv);

    senders = receivers = (nNode/2);
    if (senders > nFlow) nFlow = senders;
    app_per_node = (nFlow/senders);
    uint32_t datarate = PACKETSIZE * no_packets_persec * 8;
    coverage_area = coverage_area*5;

    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.


    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(WIRE_DATA_RATE));
    pointToPoint.SetChannelAttribute("Delay", StringValue(WIRE_DELAY));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    
    
    NodeContainer wifiStaNodesLeft, wifiStaNodesRight;
    wifiStaNodesLeft.Create(senders);
    NodeContainer wifiApNodeLeft = p2pNodes.Get(0);
    wifiStaNodesRight.Create(receivers);
    NodeContainer wifiApNodeRight = p2pNodes.Get(1);

    YansWifiChannelHelper senderChannel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper senderPhy;
    senderChannel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange",DoubleValue(coverage_area));
    senderPhy.SetChannel(senderChannel.Create());

    YansWifiChannelHelper receiverChannel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper receiverPhy;
    receiverChannel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange",DoubleValue(coverage_area));
    receiverPhy.SetChannel(receiverChannel.Create());
   
    WifiMacHelper mac;
    Ssid ssid1 = Ssid("ns-3-ssid");
    Ssid ssid2 = Ssid("ns-3-receiver");

    WifiHelper wifi;

    NetDeviceContainer staDevicesLeft,staDevicesRight;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid1), "ActiveProbing", BooleanValue(false));
    staDevicesLeft = wifi.Install(senderPhy, mac, wifiStaNodesLeft);
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid2), "ActiveProbing", BooleanValue(false));
    staDevicesRight = wifi.Install(receiverPhy, mac, wifiStaNodesRight);
    

    NetDeviceContainer apDevicesLeft,apDevicesRight;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid1));
    apDevicesLeft = wifi.Install(senderPhy, mac, wifiApNodeLeft);

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid2));
    apDevicesRight = wifi.Install(receiverPhy, mac, wifiApNodeRight);
    

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(1.0),
                                  "DeltaY",
                                  DoubleValue(1.0),
                                  "GridWidth",
                                  UintegerValue(5),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.Install(wifiStaNodesLeft);
    mobility.Install(wifiApNodeLeft);
    mobility.Install(wifiStaNodesRight);
    mobility.Install(wifiApNodeRight);

    InternetStackHelper stack;
    stack.Install(wifiStaNodesLeft);
    stack.Install(wifiStaNodesRight);
    stack.Install(p2pNodes);

    Ipv4AddressHelper address;

    address.SetBase("10.1.10.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.20.0", "255.255.255.0");
    Ipv4InterfaceContainer rightInerfaces = address.Assign(staDevicesRight);
    address.Assign(apDevicesRight);

    address.SetBase("10.1.30.0", "255.255.255.0");
    Ipv4InterfaceContainer leftInerfaces = address.Assign(staDevicesLeft);
    address.Assign(apDevicesLeft);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    int cnt = 0;
    ApplicationContainer sinks;

    while(senders--){
        for(uint32_t i = 1; i <= app_per_node; i++ ){
            PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), i+8));
            ApplicationContainer sinkApp = sinkHelper.Install(wifiStaNodesRight.Get(cnt)); 
            sinkApp.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&ReceivedPackets));
            sinkApp.Start(Seconds(0.));
            sinkApp.Stop(Seconds(SIMULATION_TIME));

            OnOffHelper sender_helper("ns3::TcpSocketFactory", (InetSocketAddress(rightInerfaces.GetAddress(cnt), i+8)));
            sender_helper.SetAttribute("PacketSize", UintegerValue(PACKETSIZE));
            sender_helper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
            sender_helper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
            sender_helper.SetAttribute("DataRate", DataRateValue(DataRate(datarate)));
            ApplicationContainer senderApp = sender_helper.Install(wifiStaNodesLeft.Get(cnt));
            senderApp.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&SentPackets));
            senderApp.Start(Seconds(1.));
            senderApp.Stop(Seconds(SIMULATION_TIME));

            sinks.Add(sinkApp.Get(0));
        }
        cnt++;
    }

    // AsciiTraceHelper ascii;
    // p2pNodes.EnableAsciiAll(ascii.CreateFileStream("scratch/track.tr"));

    Simulator::Stop(Seconds(SIMULATION_TIME));
    Simulator::Run();

    double metric = CalculateMetric(sinks, plotCat);
    // std::cout << "Plot cat: " << plotCat << std::endl;

    if (plotCat == 1 and plotNo == 1) Plot("NodeCountVSThroughput.dat", nNode, metric);
    if (plotCat == 1 and plotNo == 2) Plot("FlowCountVSThroughput.dat", nFlow, metric);
    if (plotCat == 1 and plotNo == 3) Plot("PacketCountVSThroughput.dat", no_packets_persec, metric);
    if (plotCat == 1 and plotNo == 4) Plot("CoverageVSThroughput.dat", coverage_area, metric);
    if (plotCat == 2 and plotNo == 1) Plot("NodeCountVSPacketDeliveryRatio.dat", nNode, metric);
    if (plotCat == 2 and plotNo == 2) Plot("FlowCountVSPacketDeliveryRatio.dat", nFlow, metric);
    if (plotCat == 2 and plotNo == 3) Plot("PacketCountVSPacketDeliveryRatio.dat", no_packets_persec, metric);
    if (plotCat == 2 and plotNo == 4) Plot("CoverageVSPacketDeliveryRatio.dat", coverage_area, metric);

    Simulator::Destroy();
    return 0;
}
