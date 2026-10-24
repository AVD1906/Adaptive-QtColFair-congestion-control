#ifndef TCP_OPTIMAL_H
#define TCP_OPTIMAL_H

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup tcp-optimal Description of the tcp-optimal
 */

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"
#include <deque>
#include <eigen3/Eigen/Eigen>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>

namespace ns3
{

// Each class should be documented using Doxygen,
// and have an \ingroup tcp-optimal directive

class TcpSocketState;

/**
* \brief tcp-optimal implementation
	 *
	 *
	 * 
	 */
class TcpOptimal : public TcpNewReno
{
	public:
	  /**
	   * \brief Get the type ID.
	   * \return the object TypeId
	   */
		static TypeId GetTypeId ();

		TcpOptimal ();

	  /**
	   * \brief Copy constructor.
	   * \param sock object to copy.
	   */
		TcpOptimal (const TcpOptimal& sock);

		~TcpOptimal () override;

		std::string GetName () const;

		virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
	                      const Time& rtt);
		virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
		//virtual void IncreaseWindow_option1 (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
		virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
	                            uint32_t bytesInFlight);
		virtual Ptr<TcpCongestionOps> Fork () override;


	private:
	  Time m_baseRtt;                    //!< Minimum of all ModNewReno RTT measurements seen during connection
	  Time m_minRtt;                     //!< Minimum of all RTT measurements within last RTT
	  //uint32_t m_cntRtt;                 //!< Number of RTT measurements during last RTT
	  SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT
	  double m_RTT_allowable;
	  double m_maxRTT_prefer;
	  double m_minRTT_prefer;
};


} //end namespace ns3

#endif /* TCP_OPTIMAL_H */

