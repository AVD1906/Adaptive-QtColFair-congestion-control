#include "tcp-optimal.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("TcpOptimal");
NS_OBJECT_ENSURE_REGISTERED(TcpOptimal);

TypeId TcpOptimal::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpOptimal")
                            .SetParent<TcpNewReno>()
                            .AddConstructor<TcpOptimal>()
                            .SetGroupName("Internet");
    return tid;
}


TcpOptimal::TcpOptimal ()
  : TcpNewReno(),
    m_baseRtt (Time::Max ()),
    m_minRtt (Time::Max ()),
    //m_doingModNewRenoNow (true),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpOptimal::TcpOptimal (const TcpOptimal& sock)
  : TcpNewReno (sock),
    m_baseRtt (sock.m_baseRtt),
    m_minRtt (sock.m_minRtt),
    //m_cntRtt (sock.m_cntRtt),
    //m_doingModNewRenoNow (true),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpOptimal::~TcpOptimal (void)
{
    NS_LOG_FUNCTION (this);
}

void
TcpOptimal::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                     const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);

  if (rtt.IsZero ())
    {
      return;
    }

  m_minRtt = std::min (m_minRtt, rtt);
  NS_LOG_DEBUG ("Updated m_minRtt = " << m_minRtt);

  if (m_baseRtt.GetSeconds () == 0) m_baseRtt = m_minRtt;

  m_baseRtt = std::min (m_baseRtt, rtt);
  NS_LOG_DEBUG ("Updated m_baseRtt = " << m_baseRtt);

  m_maxRTT_prefer = 1.3*m_baseRtt.GetSeconds ();
  m_minRTT_prefer = 1.2*m_baseRtt.GetSeconds ();

  // Update RTT counter
  //m_cntRtt++;
  //NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);
}


/**
 * \brief Try to increase the cWnd following the NewReno specification
 *
 * \see SlowStart
 * \see CongestionAvoidance
 *
 * \param tcb internal congestion state
 * \param segmentsAcked count of segments acked
 */
void
TcpOptimal::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);


    if (tcb->m_lastAckedSeq >= m_begSndNxt)
    { // cwnd adjustment every RTT.
      NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");
        // Save the current right edge for next ModNewReno cycle
        m_begSndNxt = tcb->m_nextTxSequence;
        double Rt = 1.2*m_minRtt.GetSeconds ();

        double RTT = tcb->m_lastRtt.Get ().GetSeconds () ;
        uint32_t L1 = tcb->m_bytesInFlight/tcb->m_segmentSize;
        uint32_t W1 = tcb->GetCwndInSegments ();
        uint32_t Lpred = L1 + W1 - W1*RTT/Rt;
        Lpred = std::max((int) Lpred, 2);
        uint32_t Wnew = W1 + m_minRtt.GetSeconds ()*((double) Lpred - (double) L1)/RTT;
        Wnew = std::max((int) Wnew, 2);
        if (Wnew == W1) Wnew = W1 + 1; //required for persistent excitation

/*        cnt++;
        if (cnt > 50)
        {
          Wnew = 100;
          cnt=0;
        }*/

/*        std::cout << std::endl << "time = " << Simulator::Now().GetSeconds() << ": " << std::endl
                  //<< "W0 = " << W0 << std::endl
                  << "L1 = " << L1 << std::endl
                  << "W1 = " << W1 << std::endl
                  << "RTT = " << RTT << std::endl
                  << " W1*Rt/RTT1 = " <<  W1*RTT/Rt << std::endl
                  << "Rt*((double) Lpred - (double) L1)/RTT = " 
                  <<  Rt*((double) Lpred - (double) L1)/RTT << std::endl
                  << "Lpred = " << Lpred << std::endl
                  << "Wnew = " << Wnew << std::endl
                  << "tcb->m_cWnd = " << tcb->m_cWnd << std::endl
                  << "m_maxRTT_prefer = " <<1.2*m_baseRtt.GetSeconds () << std::endl;
                  
        getchar ();*/


        tcb->m_cWnd = Wnew*tcb->m_segmentSize;
        //k += 1;
    }
}

std::string
TcpOptimal::GetName () const
{
  return "TcpOptimal";
}

uint32_t
TcpOptimal::GetSsThresh (Ptr<const TcpSocketState> tcb,
                       uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << tcb << bytesInFlight);
  return std::max (std::min (tcb->m_ssThresh.Get (), tcb->m_cWnd.Get () - tcb->m_segmentSize), 2 * tcb->m_segmentSize);
}

Ptr<TcpCongestionOps>
TcpOptimal::Fork ()
{
  return CopyObject<TcpOptimal> (this);
}


}
