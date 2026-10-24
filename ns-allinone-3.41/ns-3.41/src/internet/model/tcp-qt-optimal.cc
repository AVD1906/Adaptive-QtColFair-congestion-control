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
 * Adapted from TCP Vegas
 *
 */

#include "tcp-qt-optimal.h"
#include "ns3/tcp-socket-state.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include <algorithm>
#include <random>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("TcpQtOptimal");
NS_OBJECT_ENSURE_REGISTERED(TcpQtOptimal);

TypeId TcpQtOptimal::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpQtOptimal")
                            .SetParent<TcpNewReno>()
                            .AddConstructor<TcpQtOptimal>()
                            .SetGroupName("Internet")
                .AddAttribute("rttTargetAlpha",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            DoubleValue(1.5),
                            MakeDoubleAccessor(&TcpQtOptimal::SetRttTargetMultiplier),
                            MakeDoubleChecker<double>(0.0))
                .AddAttribute("DoFairness",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            BooleanValue(false),
                            MakeBooleanAccessor(&TcpQtOptimal::SetDoFairness),
                            MakeBooleanChecker ())
                .AddTraceSource("PredictedBytesInFlight",
                            "The TCP connection's congestion window",
                            MakeTraceSourceAccessor(&TcpQtOptimal::m_predictedBytesInFlight),
                            "ns3::TracedValueCallback::Uint32")
                .AddAttribute("RttWindowLength",
                          "Length of RTT windowed filter",
                          TimeValue(Seconds(10)),
                          MakeTimeAccessor(&TcpQtOptimal::m_rttPropFilterLen),
                          MakeTimeChecker());
    return tid;
}

TcpQtOptimal::TcpQtOptimal ()
  : TcpNewReno()
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::TcpQtOptimal (const TcpQtOptimal& sock)
  : TcpNewReno (sock),
    m_predictedBytesInFlight (sock.m_predictedBytesInFlight),
    m_rttPropBase  (sock.m_rttPropBase ),
    m_rttProp (sock.m_rttProp),
    m_rttPropExpired(sock.m_rttPropExpired),
    m_rttPropFilterLen(sock.m_rttPropFilterLen),
    m_rttPropStamp (sock.m_rttPropStamp),
    m_probeRttPropStamp (sock.m_probeRttPropStamp),
    m_cntRtt (sock.m_cntRtt),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::~TcpQtOptimal (void)
{
    NS_LOG_FUNCTION (this);
}

void TcpQtOptimal::SetRttTargetMultiplier (double rttTargetAlpha)
{
    m_rttTargetAlpha = rttTargetAlpha;
}

void TcpQtOptimal::SetDoFairness (bool DoFairness)
{   
    m_DoFairness = DoFairness;
}

void
TcpQtOptimal::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                     const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);

  if (rtt.IsZero ())
    {
      return;
    }

    //std::cout << " PktsAcked  point0" << std::endl;
    //UpdateRttProp(tcb);

    NS_LOG_DEBUG ("Updated m_rttProp = " << m_rttProp);

    if (tcb->m_lastRtt < m_rttProp)
        m_rttProp = tcb->m_lastRtt;
    
    if (m_rttPropBase.GetSeconds() == 0) 
        m_rttPropBase = m_rttProp;

  m_rttPropBase  = std::min (m_rttPropBase , rtt);
  NS_LOG_DEBUG ("Updated m_rttPropBase  = " << m_rttPropBase );

  // Update RTT counter
  m_cntRtt++;
  NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);
}

void
TcpQtOptimal::UpdateRttProp(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    if (m_rttProp == Seconds(0) || m_rttProp == Time::Max ())
    {
        m_rttProp = tcb->m_lastRtt;
        //m_rttPropFilterLen = 50*m_rttProp;
        m_rttTarget = m_rttTargetAlpha*m_rttProp;
        m_rttPropStamp = Simulator::Now ();
        std::cout << " UpdateRttPro point1" << std::endl;
    }
   
    m_rttPropExpired = Simulator::Now() > (m_rttPropStamp + m_rttPropFilterLen);
    if (tcb->m_lastRtt >= Seconds(0) && (tcb->m_lastRtt <= m_rttProp || m_rttPropExpired))
    {
        m_rttProp = tcb->m_lastRtt;
        m_rttTarget = m_rttTargetAlpha*m_rttProp;
        m_rttPropStamp = Simulator::Now();
        std::cout << " UpdateRttPro point2" 
                  << " m_rttProp =" << m_rttProp.GetSeconds() 
                  << std::endl;
        getchar ();
    }

}

void
TcpQtOptimal::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

/*  if (m_rttPropExpired  && !m_probeRtt && m_DoFairness)
    {
        m_probeRtt = true;
        m_probeRttPropStamp = Simulator::Now ();
        m_priorCwnd = tcb->m_cWnd;
        tcb->m_cWnd = 4 * tcb->m_segmentSize;
        m_probeRttDuration = m_rttProp;
        m_rttProp = Time::Max ();
    }

    //UpdateRttProp(tcb);

    if (Simulator::Now () - m_probeRttPropStamp >= m_probeRttDuration 
            && m_probeRtt && tcb->m_bytesInFlight <= 4 * tcb->m_segmentSize)
    {
        m_probeRtt = false;
        m_probeBw = true;
        m_rttPropStamp = Simulator::Now ();
        tcb->m_cWnd = 0.5*m_priorCwnd;
    }*/
    m_probeRtt = false;
    if (!m_probeRtt)
    {
        if (tcb->m_cWnd < tcb->m_ssThresh && tcb->m_lastRtt < m_rttTarget)
        {
            std::cout << "SlowStart" << std::endl;
            TcpNewReno::SlowStart(tcb, segmentsAcked);
            m_begSndNxt = tcb->m_nextTxSequence;
        }
        else if (tcb->m_lastAckedSeq >= m_begSndNxt)
        { // as in Vegas, we do QtOptimal cwnd adjustment every RTT.
          NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");    

            /*
             * as in Vegas, we perform QtOptimal calculations only if we got enough RTT samples to
             * insure that at least 1 of those samples wasn't from a delayed ACK.
             */
            if (m_cntRtt > 2)
            {
                // Save the current right edge for next ModNewReno cycle
                m_begSndNxt = tcb->m_nextTxSequence;
                double Rmin = m_rttProp.GetSeconds ();
                double R = tcb->m_lastRtt.Get ().GetSeconds ();
      
                if (R > 1.01*Rmin)
                    tcb->m_ssThresh = std::max (std::min (tcb->m_ssThresh.Get (), 
                      3*tcb->m_cWnd.Get ()/4), 2 * tcb->m_segmentSize);

                tcb->m_cWnd = ComputeQtCwnd (tcb);
                m_cntRtt = 0;
            }
            
        }
    }

}

uint32_t
TcpQtOptimal::ComputeQtCwnd (Ptr<TcpSocketState> tcb)
{

        //R^s \simeq R^\text{target}
        double Rmin = m_rttProp.GetSeconds ();
        double Rt = m_rttTargetAlpha*Rmin;

        //measured RTT, PacketsInFlight and CwndInSegments
        double R = tcb->m_lastRtt.Get ().GetSeconds ();
        uint32_t L = tcb->m_bytesInFlight/tcb->m_segmentSize;
        uint32_t W = tcb->GetCwndInSegments (); //cwnd in packets

              std::cout << Simulator::Now().GetSeconds()
                << " m_probeRtt=" << m_probeRtt << " m_probeBw=" << m_probeBw
                << std::endl;

        // predict desired PacketsInFlight
        uint32_t Lpred = L + W - W*R/Rt;
        if (m_probeBw)
        {
            Lpred = m_priorCwnd/tcb->m_segmentSize;
            m_probeBw = false;
        }
        if (Lpred == L) Lpred = L + 1;
        Lpred = std::max((int) Lpred, 2);

              std::cout << Simulator::Now().GetSeconds()
                << " m_probeRtt=" << m_probeRtt << " m_probeBw=" << m_probeBw << std::endl
                << " m_rttProp=" << m_rttProp.GetSeconds()
                << " m_rttTarget=" << m_rttTarget.GetSeconds() << ", " << Rt
                << " m_lastRtt=" << tcb->m_lastRtt.Get().GetSeconds() << std::endl
                << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize 
                << " m_bytesInFlight=" << tcb->m_bytesInFlight/tcb->m_segmentSize
                << " Lpred=" << Lpred 
                << std::endl;

        //compute desired cwnd
        uint32_t Wnew = W + std::ceil(Rmin*((double) Lpred - (double) L)/R);

        if (Wnew == W) Wnew = W + 1; //required for persistent excitation
        Wnew = std::max((int) Wnew, 2);

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
    getchar();

        return Wnew * tcb->m_segmentSize;
}

std::string
TcpQtOptimal::GetName () const
{
  return "TcpQtOptimal";
}

/*uint32_t
TcpQtOptimal::GetSsThresh (Ptr<const TcpSocketState> tcb,
                       uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << tcb << bytesInFlight);
  //return std::max (std::min (tcb->m_ssThresh.Get (), 
    //              tcb->m_cWnd.Get () - tcb->m_segmentSize), 2 * tcb->m_segmentSize);

  return TcpNewReno::GetSsThresh (tcb, bytesInFlight);
}*/

Ptr<TcpCongestionOps>
TcpQtOptimal::Fork ()
{
  return CopyObject<TcpQtOptimal> (this);
}


}