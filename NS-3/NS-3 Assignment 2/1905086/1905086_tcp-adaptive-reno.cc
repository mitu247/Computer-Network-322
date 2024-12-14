#include "1905086_tcp-adaptive-reno.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE("MyTcpAdaptiveReno");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(MyTcpAdaptiveReno);

TypeId
MyTcpAdaptiveReno::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::MyTcpAdaptiveReno")
            .SetParent<TcpWestwoodPlus>()
            .SetGroupName("Internet")
            .AddConstructor<MyTcpAdaptiveReno>()
  ;
            
    return tid;
}
MyTcpAdaptiveReno::MyTcpAdaptiveReno() 
    :TcpWestwoodPlus(),
    m_minRtt(Time(0)),
    m_previousConjRtt(Time(0)),
    m_jthConjRtt(Time(0)),
    m_currentRtt(Time(0)),
    m_jPacketLossRtt(Time(0)),

    m_wndInc(0),
    m_wndBase(0),
    m_wndProbe(0)
    {
        NS_LOG_FUNCTION(this);
    }
MyTcpAdaptiveReno::MyTcpAdaptiveReno (const MyTcpAdaptiveReno& sock)
    :TcpWestwoodPlus(sock),
    m_minRtt(Time(0)),
    m_previousConjRtt(Time(0)),
    m_jthConjRtt(Time(0)),
    m_currentRtt(Time(0)),
    m_jPacketLossRtt(Time(0)),

    m_wndInc(0),
    m_wndBase(0),
    m_wndProbe(0)
    {
        NS_LOG_FUNCTION(this);
        NS_LOG_LOGIC("Invoked the copy constructor");
    }
MyTcpAdaptiveReno::~MyTcpAdaptiveReno()
{
}
void
MyTcpAdaptiveReno::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                        const Time& rtt)
{
    NS_LOG_FUNCTION(this << tcb << packetsAcked << rtt);
     if(m_minRtt.IsZero()){
        m_minRtt = rtt;
        }

    if (rtt.IsZero())
    {
        NS_LOG_WARN("RTT measured is zero!");
        return;
    }
    m_currentRtt = rtt;
    
    if(rtt < m_minRtt){
        m_minRtt = rtt;
    }
    NS_LOG_LOGIC("MinimumRTT: "<<m_minRtt.GetSeconds()<< "s"<<"CurrentRtt: "<<m_currentRtt.GetSeconds()<<"s");

    m_ackedSegments += packetsAcked;
    TcpWestwoodPlus::EstimateBW(rtt, tcb);
}
double
MyTcpAdaptiveReno::EstimateCongestionLevel()
{
    double a = 0.85;
    m_jthConjRtt = Seconds(a*m_previousConjRtt.GetSeconds() + (1-a)*m_jPacketLossRtt.GetSeconds());
    //m_previousConjRtt = m_jthConjRtt;
    double c = std::min(1.0,(m_currentRtt.GetSeconds() - m_minRtt.GetSeconds())/(m_jthConjRtt.GetSeconds()- m_minRtt.GetSeconds()));
    return c;
}
void 
MyTcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb)
{
  double M = 1000;
  double alpha = 10;
  double MSS = tcb->m_segmentSize*tcb->m_segmentSize;
  double m_wndMax = m_currentBW.Get().GetBitRate()/(M*MSS);

  double congestion_level = EstimateCongestionLevel();

  double beta = 2*m_wndMax* ((1/alpha) - (((1/alpha)+ 1)/(std::exp(alpha))));
  double gamma = 1 - (2 * m_wndMax * ((1/alpha) - (((1/alpha) + 0.5)/(std::exp(alpha)))));

  int value = (m_wndMax/std::exp(alpha * congestion_level)) + (beta * congestion_level) + gamma;
  m_wndInc = value;
}
void
MyTcpAdaptiveReno::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (segmentsAcked > 0)
    {
        double adder = static_cast<double>(tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get();
        adder = std::max(1.0, adder);
        m_wndBase += static_cast<uint32_t>(adder);
        EstimateIncWnd(tcb);
        m_wndProbe = std::max(((m_wndProbe + m_wndInc)/ (int)tcb->m_cWnd.Get()), 0);
        tcb->m_cWnd = m_wndBase + m_wndProbe;
        NS_LOG_INFO("In CongAvoid, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                     << tcb->m_ssThresh);
    }
}

uint32_t
MyTcpAdaptiveReno::GetSsThresh (Ptr<const TcpSocketState> tcb,uint32_t bytesInFlight)
{
    double congestion_level = EstimateCongestionLevel();

    uint32_t ssthresh = std::max(2 * tcb->m_segmentSize,(uint32_t)(tcb->m_cWnd/(1.0+congestion_level)));
    m_wndBase = ssthresh;
    m_wndProbe = 0;
    m_previousConjRtt = m_jthConjRtt;
    m_jPacketLossRtt = m_currentRtt;
    
  return ssthresh;
}

Ptr<TcpCongestionOps>
MyTcpAdaptiveReno::Fork ()
{
  return CreateObject<MyTcpAdaptiveReno> (*this);
}
}