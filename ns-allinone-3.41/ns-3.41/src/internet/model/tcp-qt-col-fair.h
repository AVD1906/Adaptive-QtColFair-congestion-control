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
 * Adapted from TcpVegas but inherited from TcpNewReno
 *
 */


#ifndef TCP_QTCOLFAIR_H
#define TCP_QTCOLFAIR_H


/**
 * \defgroup tcp-qtcol Description of the tcp-qtcol
 */

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-state.h"
#include "tcp-socket-base.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"
#include <deque>
//#include <eigen3/Eigen/Eigen>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sqlite3.h>

namespace ns3
{

// Each class should be documented using Doxygen,
// and have an \ingroup tcp-qtcol directive

class TcpSocketState;

class TcpQtColFair : public TcpNewReno
{
    public:
      /**
       * \brief Get the type ID.
       * \return the object TypeId
       */
        static TypeId GetTypeId ();

        TcpQtColFair ();

      /**
       * \brief Copy constructor.
       * \param sock object to copy.
       */
        TcpQtColFair (const TcpQtColFair& sock);

        ~TcpQtColFair () override;

        std::string GetName () const;

        void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt);
        void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
        void SetRttTargetMultiplier (double rttTarget_alpha);
        uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                              uint32_t bytesInFlight)  override;
        void RtoOccurred()  override;
        void CongestionStateSet (Ptr<TcpSocketState> tcb,
                                 const TcpSocketState::TcpCongState_t newState) override;
        uint32_t ComputeCwnd (Ptr<TcpSocketState> tcb);

        void UpdateRttProp(Ptr<TcpSocketState> tcb);
        virtual Ptr<TcpCongestionOps> Fork () override;
        TracedValue<uint32_t> m_predictedBytesInFlight {0};
        void MinRttChangeDetection(Ptr<TcpSocketState> tcb);

    private:
      Time m_rttProp {Time::Max ()};            //!< Minimum of all RTT measurements within last RTT
      bool m_rttPropExpired{false};         //!< A boolean recording whether the BBR.RTprop has expired
                                             //!< filter window, default 10 secs.
      Time m_rttPropFilterLen{Seconds(10)}; //!< A constant specifying the length of the RTProp min
      Time m_rttPropStamp {Seconds(0)};
      Time m_rttTarget {Seconds(0)};
      double m_rttTargetAlpha{1.2};
      Time m_lastRtt {Seconds(0)};
      bool m_enableAdaptiveLoss {true};
      double m_betaMin {0.50};
      double m_betaMax {0.95};
      bool m_inLossTimeout {false};
      Time m_probeRttDuration {Seconds(0.2)};
      Time m_probeRttPropStamp {Seconds(0)};
      uint32_t m_priorInFlight {1024};
      uint32_t m_cntRtt {0};                 //!< Number of RTT measurements during last RTT
      uint32_t m_rttOvershootCnt {0};

      //SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT
      Time m_cwndChangeStamp {0};
      bool m_probeRtt {false};
      bool m_usePriorInFlight {false};
      SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT

      double m_prev_rtt {0.0};
      double m_prev_pktsInFlight {0.0};
      double m_prev_cwndInPkts {0.0};
      double m_prev_time {0.0};
      bool m_minRttChanged {false};
};


} //end namespace ns3

#endif /* TCP_QTCOLFAIR_H */
