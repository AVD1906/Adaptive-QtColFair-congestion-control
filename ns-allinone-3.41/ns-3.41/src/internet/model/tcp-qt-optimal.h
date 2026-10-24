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


#ifndef TCP_QTOPTIMAL_H
#define TCP_QTOPTIMAL_H


/**
 * \defgroup tcp-qtoptimal Description of the tcp-qtoptimal
 */

#include "ns3/tcp-congestion-ops.h"
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
// and have an \ingroup tcp-qtoptimal directive

class TcpSocketState;

/**
* \brief tcp-qtoptimal implementation
     *
     *
     * 
     */
class TcpQtOptimal : public TcpNewReno
{
    public:
      /**
       * \brief Get the type ID.
       * \return the object TypeId
       */
        static TypeId GetTypeId ();

        TcpQtOptimal ();

      /**
       * \brief Copy constructor.
       * \param sock object to copy.
       */
        TcpQtOptimal (const TcpQtOptimal& sock);

        ~TcpQtOptimal () override;

        std::string GetName () const;

        void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt);

        void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
/*        uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, 
                                uint32_t bytesInFlight) override;*/

        virtual Ptr<TcpCongestionOps> Fork () override;

        void SetRttTargetMultiplier (double rttTarget_alpha);
        void SetDoFairness (bool DoFairness);

        uint32_t ComputeQtCwnd (Ptr<TcpSocketState> tcb);

        void UpdateRttProp(Ptr<TcpSocketState> tcb);
 
        sqlite3 *db;

        TracedValue<uint32_t> m_predictedBytesInFlight {0};

    private:
      Time m_rttPropBase {Time::Max ()};        //!< Minimum of all RTT measurements seen during connection
      Time m_rttProp {Time::Max ()};            //!< Minimum of all RTT measurements within last RTT
      bool m_rttPropExpired{false};         //!< A boolean recording whether the BBR.RTprop has expired
                                             //!< filter window, default 10 secs.
      Time m_rttPropFilterLen{Seconds(10)}; //!< A constant specifying the length of the RTProp min
      Time m_rttPropStamp;
      Time m_rttTarget;
      double m_rttTargetAlpha;
      Time m_probeRttPropStamp;
      Time m_probeRttDuration {Seconds(0.2)};
      uint32_t m_priorCwnd {1024};
      uint32_t m_cntRtt;                 //!< Number of RTT measurements during last RTT
      
      bool m_DoFairness;
      SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT
      bool m_probeRtt {false};
      bool m_probeBw {false};
      SequenceNumber32 m_nowAckedSeq {0};
      SequenceNumber32 m_prevAckedSeq {0};
      Time m_prevAckedSeqTime;
      double m_prevRate {0};
};



} //end namespace ns3

#endif /* TCP_QTOPTIMAL_H */