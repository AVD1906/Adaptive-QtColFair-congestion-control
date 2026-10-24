/*
 * Copyright (c) 2024 Dumisa Ngwenya <dumisa@crocs.co.za; ngwenyad@sentech.co.za>
 *
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
 *
 * Developed as part of PhD work on TCP Congestion Control
 * Dumisa W. Ngwenya - University of Pretoria - Department of Electrical, Electronic and Computer Engineering
 *
 */

#include "tcp-qt-col.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include <algorithm>
#include <random>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("TcpQtCol");
NS_OBJECT_ENSURE_REGISTERED(TcpQtCol);

TypeId TcpQtCol::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpQtCol")
                            .SetParent<TcpNewReno>()
                            .AddConstructor<TcpQtCol>()
                            .SetGroupName("Internet")
                .AddAttribute("rttTargetAlpha",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            DoubleValue(1.5),
                            MakeDoubleAccessor(&TcpQtCol::SetRttTargetMultiplier),
                            MakeDoubleChecker<double>(0.0))
                .AddTraceSource("PredictedBytesInFlight",
                            "The TCP connection's congestion window",
                            MakeTraceSourceAccessor(&TcpQtCol::m_predictedBytesInFlight),
                            "ns3::TracedValueCallback::Uint32");
    return tid;
}

TcpQtCol::TcpQtCol ()
  : TcpNewReno()
{
    NS_LOG_FUNCTION (this);
}

TcpQtCol::TcpQtCol (const TcpQtCol& sock)
  : TcpNewReno (sock),
    m_rttProp (sock.m_rttProp),
    m_rttTarget (sock.m_rttTarget),
    m_rttTargetAlpha (sock.m_rttTargetAlpha),
    m_cntRtt (sock.m_cntRtt),
    m_begSndNxt (0)
{
    NS_LOG_FUNCTION (this);
}

TcpQtCol::~TcpQtCol (void)
{
    NS_LOG_FUNCTION (this);
}

void TcpQtCol::SetRttTargetMultiplier (double rttTargetAlpha)
{
    m_rttTargetAlpha = rttTargetAlpha;
}

void TcpQtCol::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                     const Time& rtt)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);

    if (rtt.IsZero ())
    {
      return;
    }

    m_rttProp= std::min(m_rttProp, rtt);
    m_rttTarget = m_rttTargetAlpha*m_rttProp;

    NS_LOG_DEBUG ("Updated m_rttProp = " << m_rttProp);

    // Update RTT counter
    m_cntRtt++;
    NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);
}

void TcpQtCol::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    if (tcb->m_lastAckedSeq >= m_begSndNxt)
    { // as in Vegas, we do QtCol cwnd adjustment every RTT.
      NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");    

        /*
         * as in Vegas, we perform QtCol calculations only if we got enough RTT samples to
         * insure that at least 1 of those samples wasn't from a delayed ACK.
         */
        if (m_cntRtt <= 2)
        { // We do not have enough RTT samples, so we should behave like Reno
            NS_LOG_LOGIC(
                "We do not have enough RTT samples");
            //TcpNewReno::IncreaseWindow(tcb, segmentsAcked);
        }
        else
        {
            // Save the current right edge for next cycle
            m_begSndNxt = tcb->m_nextTxSequence;
            double Rmin = m_rttProp.GetSeconds ();
            double R = tcb->m_lastRtt.Get ().GetSeconds ();
  
            if (R > 1.01*Rmin)
                tcb->m_ssThresh = std::max (std::min (tcb->m_ssThresh.Get (), 
                  9*tcb->m_cWnd.Get ()/10), 2 * tcb->m_segmentSize);

            tcb->m_cWnd = ComputeCwnd (tcb);
            m_cntRtt = 0;
        }
    }
    else if (tcb->m_cWnd < tcb->m_ssThresh && tcb->m_lastRtt < m_rttTarget)
    {
        std::cout << "SlowStart" << std::endl;
        TcpNewReno::SlowStart(tcb, segmentsAcked);
        m_begSndNxt = tcb->m_nextTxSequence;
    }

}

uint32_t TcpQtCol::ComputeCwnd (Ptr<TcpSocketState> tcb)
{
    //R^s \simeq R^\text{target}
    double Rmin = m_rttProp.GetSeconds ();
    double Rt = m_rttTargetAlpha*Rmin;

    //measured RTT, PacketsInFlight and CwndInSegments
    double R = tcb->m_lastRtt.Get ().GetSeconds ();
    uint32_t L = tcb->m_bytesInFlight/tcb->m_segmentSize;
    uint32_t W = tcb->GetCwndInSegments (); //cwnd in packets

    // predict desired PacketsInFlight
    uint32_t Lpred;
    if (L + W > std::ceil(W*R/Rt))
        Lpred = L + W - std::ceil(W*R/Rt);
    else Lpred = 4;
    Lpred = std::max((int) Lpred, 4);
    if (Lpred == L) Lpred = L + 1;

    std::cout << Simulator::Now().GetSeconds()
                << " m_rttProp=" << m_rttProp.GetSeconds()
                << " m_rttTarget=" << m_rttTarget.GetSeconds() << ", " << Rt
                << " m_rttTargetAlpha=" << m_rttTargetAlpha
                << " m_lastRtt=" << tcb->m_lastRtt.Get().GetSeconds() << std::endl
                << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize 
                << " m_bytesInFlight=" << tcb->m_bytesInFlight/tcb->m_segmentSize
                << " Lpred=" << Lpred 
                << std::endl;

    //compute desired cwnd
    uint32_t Wnew = W + std::ceil(Rmin*((double) Lpred - (double) L)/R);
    Wnew = std::max((int) Wnew, 4);
    if (Wnew == W) Wnew = W + 1; //required for persistent excitation

    std::cout << Simulator::Now().GetSeconds()
            << " m_rttProp=" << m_rttProp.GetSeconds()
            << " m_rttTargetAlpha=" << m_rttTargetAlpha
            << " m_rttTarget=" << m_rttTarget.GetSeconds()
            << " m_lastRtt=" << tcb->m_lastRtt.Get().GetSeconds() << std::endl
            << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize 
            << " m_bytesInFlight=" << tcb->m_bytesInFlight/tcb->m_segmentSize
            << " Lpred=" << Lpred 
            << " Wnew=" << Wnew 
            << std::endl << std::endl;
    //getchar();

    return Wnew * tcb->m_segmentSize;
}

std::string TcpQtCol::GetName () const
{
  return "TcpQtCol";
}

Ptr<TcpCongestionOps>
TcpQtCol::Fork ()
{
  return CopyObject<TcpQtCol> (this);
}


}