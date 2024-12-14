#ifndef TCP_ADAPTIVE_RENO_H
#define TCP_ADAPTIVE_RENO_H

#include "tcp-congestion-ops.h"
#include "tcp-westwood-plus.h"
#include "ns3/data-rate.h"
#include "ns3/event-id.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/traced-value.h"
#include "ns3/sequence-number.h"

namespace ns3
{

class Time;


class MyTcpAdaptiveReno : public TcpWestwoodPlus
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    MyTcpAdaptiveReno();
    /**
     * \brief Copy constructor
     * \param sock the object to copy
     */
    MyTcpAdaptiveReno(const MyTcpAdaptiveReno& sock);
    ~MyTcpAdaptiveReno() override;

    /**
     * \brief Filter type (None or Tustin)
     */
    enum FilterType 
    {
        NONE,
        TUSTIN
    };

    uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) override;

    void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked, const Time& rtt) override;

    Ptr<TcpCongestionOps> Fork() override;

  private:
    /**
     * Update the total number of acknowledged packets during the current RTT
     *
     * \param [in] acked the number of packets the currently received ACK acknowledges
     */
    double EstimateCongestionLevel();
    void EstimateIncWnd(Ptr<TcpSocketState> tcb);

    /**
     * Estimate the network's bandwidth
     *
     * \param [in] rtt the RTT estimation.
     * \param [in] tcb the socket state.
     */

  protected:
    Time m_minRtt;
    Time m_previousConjRtt;
    Time m_jthConjRtt;
    Time m_currentRtt;
    Time m_jPacketLossRtt;

    int32_t m_wndInc;
    uint32_t m_wndBase;
    int32_t m_wndProbe;

    virtual void CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

};

}
#endif