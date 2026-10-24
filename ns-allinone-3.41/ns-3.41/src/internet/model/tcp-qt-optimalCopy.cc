
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
                .AddAttribute("Rtarget_mult",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            DoubleValue(1.5),
                            MakeDoubleAccessor(&TcpQtOptimal::SetRtarget_mult),
                            MakeDoubleChecker<double>(0.0))
                .AddAttribute("fairness_index",
                            "multiplication factor for Rtarget in proportion to Rmin",
                            BooleanValue(false),
                            MakeBooleanAccessor(&TcpQtOptimal::SetFairnessIndex),
                            MakeBooleanChecker ())
                .AddTraceSource("PredictedBytesInFlight",
                            "The TCP connection's congestion window",
                            MakeTraceSourceAccessor(&TcpQtOptimal::m_predictedBytesInFlight),
                            "ns3::TracedValueCallback::Uint32");
    return tid;
}

TcpQtOptimal::TcpQtOptimal ()
  : TcpNewReno(),
    m_predictedBytesInFlight (0),
    m_baseRtt (Time::Max()),
    m_cntRtt(0),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::TcpQtOptimal (const TcpQtOptimal& sock)
  : TcpNewReno (sock),
    m_baseRtt (sock.m_baseRtt),
    m_cntRtt (sock.m_cntRtt),
    m_begSndNxt (0)
{
  NS_LOG_FUNCTION (this);
}

TcpQtOptimal::~TcpQtOptimal (void)
{
    NS_LOG_FUNCTION (this);
}

const char* const TcpQtOptimal::BbrModeName[BBR_PROBE_RTT + 1] = {
    "BBR_STARTUP",
    "BBR_DRAIN",
    "BBR_PROBE_BW",
    "BBR_PROBE_RTT",
};

void
TcpQtOptimal::UpdateRTprop(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    m_rtPropExpired = Simulator::Now() > (m_rtPropStamp + m_rtPropFilterLen);
    if (tcb->m_lastRtt >= Seconds(0) && (tcb->m_lastRtt < m_rtProp || m_rtPropExpired))
    {
        m_rtProp = tcb->m_lastRtt;
        m_rtPropStamp = Simulator::Now();
    }
    m_Rtarget = m_rtProp.GetSeconds ();
}

void
TcpQtOptimal::EnterProbeRTT()
{
    NS_LOG_FUNCTION(this);
    SetBbrState(BbrMode_t::BBR_PROBE_RTT);
    //m_pacingGain = 1;
    //m_cWndGain = 1;
}

void
TcpQtOptimal::SaveCwnd(Ptr<const TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (tcb->m_congState != TcpSocketState::CA_RECOVERY && m_state != BbrMode_t::BBR_PROBE_RTT)
    {
        m_priorCwnd = tcb->m_cWnd;
    }
    else
    {
        m_priorCwnd = std::max(m_priorCwnd, tcb->m_cWnd.Get());
    }
}

void
TcpQtOptimal::RestoreCwnd(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    tcb->m_cWnd = std::max(m_priorCwnd, tcb->m_cWnd.Get());
}

void
TcpQtOptimal::HandleProbeRTT(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    uint32_t totalBytes = m_delivered + tcb->m_bytesInFlight.Get();
    m_appLimited = (totalBytes > 0 ? totalBytes : 1);

    if (m_probeRttDoneStamp == Seconds(0) && tcb->m_bytesInFlight <= m_minPipeCwnd)
    {
        m_probeRttDoneStamp = Simulator::Now() + m_probeRttDuration;
        m_probeRttRoundDone = false;
        m_nextRoundDelivered = m_delivered;
    }
    else if (m_probeRttDoneStamp != Seconds(0))
    {
        if (m_roundStart)
        {
            m_probeRttRoundDone = true;
        }

        if (m_probeRttRoundDone && Simulator::Now() > m_probeRttDoneStamp)
        {
            m_rtPropStamp = Simulator::Now();
            RestoreCwnd(tcb);
            ExitProbeRTT();
        }
    }
}

void
TcpQtOptimal::CheckProbeRTT(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb);
    if (m_state != BbrMode_t::BBR_PROBE_RTT && m_rtPropExpired && !m_idleRestart)
    {
        EnterProbeRTT();
        SaveCwnd(tcb);
        m_probeRttDoneStamp = Seconds(0);
    }

    if (m_state == BbrMode_t::BBR_PROBE_RTT)
    {
        HandleProbeRTT(tcb);
    }

    if (rs.m_delivered)
    {
        m_idleRestart = false;
    }
}

void
TcpQtOptimal::ModulateCwndForProbeRTT(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (m_state == BbrMode_t::BBR_PROBE_RTT)
    {
        tcb->m_cWnd = std::min(tcb->m_cWnd.Get(), m_minPipeCwnd);
    }
}

void
TcpQtOptimal::SetCwnd(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);

}

void
TcpQtOptimal::UpdateModelAndState(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    UpdateRTprop(tcb);
    CheckProbeRTT(tcb, rs);
}

void
TcpQtOptimal::UpdateControlParameters(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    //SetPacingRate(tcb, m_pacingGain);
    //SetSendQuantum(tcb);
    //SetCwnd(tcb, rs);
}

void
TcpQtOptimal::SetBbrState(BbrMode_t mode)
{
    NS_LOG_FUNCTION(this << mode);
    NS_LOG_DEBUG(Simulator::Now() << " Changing from " << BbrModeName[m_state] << " to "
                                  << BbrModeName[mode]);
    m_state = mode;
}

uint32_t
TcpQtOptimal::GetBbrState()
{
    NS_LOG_FUNCTION(this);
    return m_state;
}

void TcpQtOptimal::SetRtarget_mult (double Rtarget_mult)
{
    if (Rtarget_mult == 0) Rtarget_mult = 1;
    m_Rtarget_mult = Rtarget_mult;
}

void TcpQtOptimal::SetFairnessIndex (bool fairness_index)
{   
    m_fairness_index = fairness_index;
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

    if (m_state == BbrMode_t::BBR_PROBE_RTT)
    {
        HandleProbeRTT(tcb);
    }
    else if (m_rtProp >= Time::Max() || m_rtProp > rtt)
    {
        m_rtProp = rtt;
        m_Rtarget = m_rtProp.GetSeconds ();
    }

    m_rtProp = std::min(m_rtProp, rtt);
    m_Rtarget = m_rtProp.GetSeconds ();

  NS_LOG_DEBUG ("Updated m_minRtt = " << m_minRtt);

  m_baseRtt = std::min (m_baseRtt, m_rtProp);
  NS_LOG_DEBUG ("Updated m_baseRtt = " << m_baseRtt);

  // Update RTT counter
  m_cntRtt++;
  NS_LOG_DEBUG ("Updated m_cntRtt = " << m_cntRtt);

}

void
TcpQtOptimal::CongestionStateSet(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this << tcb << newState);

    if (newState == TcpSocketState::CA_OPEN && !m_isInitialized)
    {
        m_rtProp = tcb->m_lastRtt.Get() != Time::Max() ? tcb->m_lastRtt.Get() : Time::Max();
        m_rtPropStamp = Simulator::Now();
        m_priorCwnd = tcb->m_cWnd;
        tcb->m_ssThresh = tcb->m_initialSsThresh;
        m_minPipeCwnd = 4 * tcb->m_segmentSize;
        //InitRoundCounting();
        m_isInitialized = true;
        std::cout << "CongestionStateSet Reseted" << std::endl;
    }
    else if (newState == TcpSocketState::CA_LOSS)
    {
        NS_LOG_DEBUG("CongestionStateSet triggered to CA_LOSS :: " << newState);
        SaveCwnd(tcb);
        m_roundStart = true;
    }
    else if (newState == TcpSocketState::CA_RECOVERY)
    {
        NS_LOG_DEBUG("CongestionStateSet triggered to CA_RECOVERY :: " << newState);
        SaveCwnd(tcb);
        tcb->m_cWnd =
            tcb->m_bytesInFlight.Get() + std::max(tcb->m_lastAckedSackedBytes, tcb->m_segmentSize);
        m_packetConservation = true;
    }

}


bool
TcpQtOptimal::HasCongControl() const
{
    NS_LOG_FUNCTION(this);
    return true;
}

void
TcpQtOptimal::CongControl(Ptr<TcpSocketState> tcb,
                    const TcpRateOps::TcpRateConnection& rc,
                    const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    m_delivered = rc.m_delivered;
    m_txItemDelivered = rc.m_txItemDelivered;
    UpdateModelAndState(tcb, rs);
    UpdateControlParameters(tcb, rs);
}

void
TcpQtOptimal::ExitProbeRTT()
{
    NS_LOG_FUNCTION(this);
}

void
TcpQtOptimal::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION (this << tcb << segmentsAcked);

    std::cout << "lastRtt=" << tcb->m_lastRtt.Get ().GetSeconds ()
              << " m_rtProp=" << m_rtProp.GetSeconds ()
              << " m_Rtarget=" << m_Rtarget
              << " m_state=" << m_state
              << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize
              << std::endl;

    if (m_state != BbrMode_t::BBR_PROBE_RTT)
    {
        std::cout << "m_state != BbrMode_t::BBR_PROBE_RTT=" <<  BbrMode_t::BBR_PROBE_RTT
              << " m_cntRtt=" << m_cntRtt << std::endl;
        if (tcb->m_lastAckedSeq >= m_begSndNxt)
        { // as in Vegas, we do QtOptimal cwnd adjustment every RTT.
          NS_LOG_LOGIC ("A cycle has finished, we adjust cwnd once per RTT.");

            /*
             * as in Vegas, we perform QtOptimal calculations only if we got enough RTT samples to
             * insure that at least 1 of those samples wasn't from a delayed ACK.
             */
            if (m_cntRtt <= 2)
            { // We do not have enough RTT samples, so we should behave like Reno
                NS_LOG_LOGIC(
                    "We do not have enough RTT samples to do QtOptimal, so we behave like NewReno.");
                TcpNewReno::IncreaseWindow(tcb, segmentsAcked);
            }
            else
            {
                m_begSndNxt = tcb->m_nextTxSequence;
             std::cout << "ComputeQtCwnd (tcb)=" <<  BbrMode_t::BBR_PROBE_RTT
              << " m_cntRtt=" << m_cntRtt << std::endl;
      
                tcb->m_cWnd = ComputeQtCwnd (tcb);
                m_cntRtt = 0;
            }
        }
        else if (tcb->m_cWnd < tcb->m_ssThresh)
        {
            //this mimicks max-min resource allocation
            // with if R > Rt
           TcpNewReno::SlowStart(tcb, segmentsAcked);
        }
    }
    std::cout << "lastRtt=" << tcb->m_lastRtt.Get ().GetSeconds ()
              << " m_rtProp=" << m_rtProp.GetSeconds ()
              << " m_Rtarget=" << m_Rtarget
              << " m_state=" << m_state
              << " m_cWnd=" << tcb->m_cWnd/tcb->m_segmentSize
              << std::endl;
    getchar ();

}


uint32_t
TcpQtOptimal::ComputeQtCwnd (Ptr<TcpSocketState> tcb)
{

        //R^s \simeq R^\text{target}
        double Rmin = m_rtProp.GetSeconds ();
        double Rt = m_Rtarget_mult*Rmin;

        //measured RTT, PacketsInFlight and CwndInSegments
        double R = tcb->m_lastRtt.Get ().GetSeconds ();
        uint32_t L = tcb->m_bytesInFlight/tcb->m_segmentSize;
        uint32_t W = tcb->GetCwndInSegments (); //cwnd in packets

        // predict desired PacketsInFlight
        uint32_t Lpred = L + W - W*R/Rt;
        if (Lpred == L) Lpred = L + 1;
        Lpred = std::max((int) Lpred, 2);

        //compute desired cwnd
        uint32_t Wnew = W + std::ceil(Rmin*((double) Lpred - (double) L)/R);
        if (Wnew == W) Wnew = W + 1; //required for persistent excitation
        Wnew = std::max((int) Wnew, 2);

        return Wnew * tcb->m_segmentSize;
}

std::string
TcpQtOptimal::GetName () const
{
  return "TcpQtOptimal";
}

void
TcpQtOptimal::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
    NS_LOG_FUNCTION(this << tcb << event);
    if (event == TcpSocketState::CA_EVENT_COMPLETE_CWR)
    {
        NS_LOG_DEBUG("CwndEvent triggered to CA_EVENT_COMPLETE_CWR :: " << event);
        m_packetConservation = false;
        RestoreCwnd(tcb);
    }
    else if (event == TcpSocketState::CA_EVENT_TX_START && m_appLimited)
    {
        NS_LOG_DEBUG("CwndEvent triggered to CA_EVENT_TX_START :: " << event);
        m_idleRestart = true;
        //m_ackEpochTime = Simulator::Now();
        //m_ackEpochAcked = 0;
        if (m_state == BbrMode_t::BBR_PROBE_BW)
        {
            //SetPacingRate(tcb, 1);
        }
        else if (m_state == BbrMode_t::BBR_PROBE_RTT)
        {
            if (m_probeRttRoundDone && Simulator::Now() > m_probeRttDoneStamp)
            {
                m_rtPropStamp = Simulator::Now();
                RestoreCwnd(tcb);
                ExitProbeRTT();
            }
        }
    }
}

uint32_t
TcpQtOptimal::GetSsThresh (Ptr<const TcpSocketState> tcb,
                       uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << tcb << bytesInFlight);
  //return std::max (std::min (tcb->m_ssThresh.Get (), 
    //              tcb->m_cWnd.Get () - tcb->m_segmentSize), 2 * tcb->m_segmentSize);

  return TcpNewReno::GetSsThresh (tcb, bytesInFlight);
  //return std::max(2 * tcb->m_segmentSize, bytesInFlight / 2);
}

Ptr<TcpCongestionOps>
TcpQtOptimal::Fork ()
{
  return CopyObject<TcpQtOptimal> (this);
}


}
