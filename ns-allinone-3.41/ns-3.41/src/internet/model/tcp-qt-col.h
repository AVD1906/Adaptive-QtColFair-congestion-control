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
 *
 */


#ifndef TCP_QTCOL_H
#define TCP_QTCOL_H


/**
 * \defgroup tcp-qtcol Description of the tcp-qtcol
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
// and have an \ingroup tcp-qtcol directive

class TcpSocketState;

/**
* \brief tcp-qtcol implementation
     */
class TcpQtCol : public TcpNewReno
{
    public:
      /**
       * \brief Get the type ID.
       * \return the object TypeId
       */
        static TypeId GetTypeId ();

        TcpQtCol ();

      /**
       * \brief Copy constructor.
       * \param sock object to copy.
       */
        TcpQtCol (const TcpQtCol& sock);

        ~TcpQtCol () override;

        std::string GetName () const;

        void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt);

        void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);

        virtual Ptr<TcpCongestionOps> Fork () override;

        void SetRttTargetMultiplier (double rttTarget_alpha);

        uint32_t ComputeCwnd (Ptr<TcpSocketState> tcb);

        void UpdateRttProp(Ptr<TcpSocketState> tcb);

        TracedValue<uint32_t> m_predictedBytesInFlight {0};

    private:
      Time m_rttProp {Time::Max ()};            //!< Minimum of all RTT measurements within last RTT
      Time m_rttTarget;
      double m_rttTargetAlpha{1.5};
      uint32_t m_cntRtt;                 //!< Number of RTT measurements during last RTT
      
      SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT
};


} //end namespace ns3

#endif /* TCP_QTCOL_H */