#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/callback.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("NS3OFFLINE2");

class TutorialApp : public Application
{
  public:
    TutorialApp();
    ~TutorialApp() override;

    /**
     * Register this type.
     * \return The TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Setup the socket.
     * \param socket The socket.
     * \param address The destination address.
     * \param packetSize The packet size to transmit.
     * \param st The number of packets to transmit.
     * \param dataRate the data rate to use.
     */
    void Setup(Ptr<Socket> socket,
               Address address,
               uint32_t packetSize,
               uint32_t st,
               DataRate dataRate);

  private:
    void StartApplication() override;
    void StopApplication() override;

    /// Schedule a new transmission.
    void ScheduleTx();
    /// Send a packet.
    void SendPacket();

    Ptr<Socket> m_socket;   //!< The transmission socket.
    Address m_peer;         //!< The destination address.
    uint32_t m_packetSize;  //!< The packet size.
    uint32_t m_st;    //!< The number of packets to send.
    DataRate m_dataRate;    //!< The data rate to use.
    EventId m_sendEvent;    //!< Send event.
    bool m_running;         //!< True if the application is running.
    uint32_t m_packetsSent; //!< The number of packets sent.
};
TutorialApp::TutorialApp()
    : m_socket(nullptr),
      m_peer(),
      m_packetSize(0),
      m_st(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0)
{
}

TutorialApp::~TutorialApp()
{
    m_socket = nullptr;
}

/* static */
TypeId
TutorialApp::GetTypeId()
{
    static TypeId tid = TypeId("TutorialApp")
                            .SetParent<Application>()
                            .SetGroupName("Tutorial")
                            .AddConstructor<TutorialApp>();
    return tid;
}

void
TutorialApp::Setup(Ptr<Socket> socket,
                   Address address,
                   uint32_t packetSize,
                   uint32_t st,
                   DataRate dataRate)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_st = st;
    m_dataRate = dataRate;
}

void
TutorialApp::StartApplication()
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
TutorialApp::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
TutorialApp::SendPacket()
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    m_socket->Send(packet);
    if (Simulator::Now().GetSeconds() < m_st)
    {
        ScheduleTx();
    }
}

void
TutorialApp::ScheduleTx()
{
    if (m_running)
    {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &TutorialApp::SendPacket, this);
    }
}


static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream()<< Simulator::Now ().GetSeconds () << "\t" << newCwnd << std::endl;
}

int main(int argc,char *argv[]){

    std::string sender_receiver_datarate = "1Gbps";
    std::string sender_receiver_delay = "1ms";
    int bottleneck_delay = 100;
    int bottleneck_datarate = 50;
    double packet_loss_rate = (1/std::pow(10,6));
    uint32_t playload = 1024;
    uint32_t simulation_time = 8;
    uint16_t sinkPort = 8000;
    std::string modelType = "MyTcpAdaptiveReno";
    int plotType = 2;
    std::string output_plot = "scratch/plot";

    CommandLine cmd (__FILE__);
    cmd.AddValue("bottleneck_datarate","Max Packets allowed in the device queue",bottleneck_datarate);
    cmd.AddValue("packet_loss_rate", "Packet losses during transmission", packet_loss_rate);
    cmd.AddValue("modelType","1 TcpHighSpeed, 2 TcpAdaptiveReno", modelType);
    cmd.AddValue("plotType","1 thvsbltn, 2 thvspktlsrt,3 cwvstm", plotType);
    cmd.Parse(argc,argv);
    std::string b_data = std::to_string(bottleneck_datarate) +"Mbps";
    std::string b_delay = std::to_string(bottleneck_delay) + "ms";

    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue(playload));
    
    PointToPointHelper p2pleft,p2pright,bottleneck;
    p2pleft.SetDeviceAttribute  ("DataRate", StringValue(sender_receiver_datarate));
    p2pleft.SetChannelAttribute ("Delay", StringValue(sender_receiver_delay));
    p2pright.SetDeviceAttribute  ("DataRate", StringValue(sender_receiver_datarate));
    p2pright.SetChannelAttribute ("Delay", StringValue (sender_receiver_delay));
    bottleneck.SetDeviceAttribute  ("DataRate", StringValue(b_data));
    bottleneck.SetChannelAttribute ("Delay", StringValue(b_delay));
    p2pleft.SetQueue("ns3::DropTailQueue", "MaxSize",StringValue(std::to_string(bottleneck_datarate*bottleneck_delay) + "p"));
    p2pright.SetQueue("ns3::DropTailQueue", "MaxSize",StringValue(std::to_string(bottleneck_datarate*bottleneck_delay) + "p"));

    PointToPointDumbbellHelper dumble(2,p2pleft,2, p2pright,bottleneck);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    em->SetAttribute("ErrorRate", DoubleValue(packet_loss_rate));
    dumble.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel",PointerValue(em));

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    InternetStackHelper stack_flow_1;

    stack_flow_1.Install(dumble.GetLeft(0));
    stack_flow_1.Install(dumble.GetRight(0));

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::"+modelType));
    InternetStackHelper stack_flow_2;

    stack_flow_2.Install(dumble.GetLeft(1));
    stack_flow_2.Install(dumble.GetRight(1));

    stack_flow_1.Install(dumble.GetLeft());
    stack_flow_1.Install(dumble.GetRight());

    dumble.AssignIpv4Addresses (Ipv4AddressHelper ("10.10.1.0", "255.255.255.0"),
                          Ipv4AddressHelper ("10.20.1.0", "255.255.255.0"), 
                          Ipv4AddressHelper ("10.30.1.0", "255.255.255.0")); 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    for(int i = 0; i < 2; i++){   
    AsciiTraceHelper asciiTraceHelper;
    Address sinkAddress(InetSocketAddress(dumble.GetRightIpv4Address(i), sinkPort));
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install(dumble.GetRight(i));
    sinkApps.Start(Seconds(0.));
    sinkApps.Stop(Seconds(8.));

    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(dumble.GetLeft(i), TcpSocketFactory::GetTypeId());
    Ptr<TutorialApp> app = CreateObject<TutorialApp>();
    app->Setup(ns3TcpSocket, sinkAddress,playload,simulation_time, DataRate(sender_receiver_datarate));
    dumble.GetLeft(i)->AddApplication(app);
    app->SetStartTime(Seconds(1.));
    app->SetStopTime(Seconds(8.));
    if(plotType == 3){
        Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream(output_plot+"/"+std::to_string(i+1)+".data");
        ns3TcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback(&CwndChange,stream));
    }
    }
    FlowMonitorHelper flow_monitor;
    flow_monitor.SetMonitorAttribute("MaxPerHopDelay", TimeValue(Seconds(2)));
    Ptr<FlowMonitor> monitor = flow_monitor.InstallAll();

    Simulator::Stop(Seconds(simulation_time));
    Simulator::Run();

    double rxFlow1 = 0, rxFlow2 = 0;
    for (auto flow: monitor->GetFlowStats()) {
        int flowid = flow.first;
        if (flowid == 0 || flowid == 2) {
            rxFlow1 += flow.second.rxBytes;
        }
        else if (flowid == 1 || flowid == 3) {
            rxFlow2 += flow.second.rxBytes;
        }

    }
    AsciiTraceHelper as;
    rxFlow1 = rxFlow1*8;
    rxFlow2 = rxFlow2*8;
    double avg = rxFlow1 / (simulation_time * 1e3); //Kbps
    double avg1 = rxFlow2/(simulation_time * 1e3);
    if (plotType == 1) {
        Ptr<OutputStreamWrapper> stream = as.CreateFileStream(output_plot + "/1.data", std::ios::app);
        *stream->GetStream() << bottleneck_datarate << ' ' << avg << std::endl;
        
        stream = as.CreateFileStream(output_plot + "/2.data", std::ios::app);
        *stream->GetStream() << bottleneck_datarate << ' ' << avg1 << std::endl;
    }
    else if (plotType == 2) {
        Ptr<OutputStreamWrapper> stream = as.CreateFileStream(output_plot + "/1.data", std::ios::app);
        *stream->GetStream() << std::log10(packet_loss_rate) << ' ' << avg << std::endl;
        
        stream = as.CreateFileStream(output_plot + "/2.data", std::ios::app);
        *stream->GetStream() << std::log10(packet_loss_rate) << ' ' << avg1 << std::endl;
    }

    Simulator::Destroy();
}