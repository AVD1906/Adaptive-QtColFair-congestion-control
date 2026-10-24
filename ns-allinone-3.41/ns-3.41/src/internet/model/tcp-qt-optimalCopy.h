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

#include "windowed-filter.h"
#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"

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

    /**
 	* \brief BBR has the following 4 modes for deciding how fast to send:
 	*/
	enum BbrMode_t
	{
	    BBR_STARTUP,   /**< Ramp up sending rate rapidly to fill pipe */
	    BBR_DRAIN,     /**< Drain any queue created during startup */
	    BBR_PROBE_BW,  /**< Discover, share bw: pace around estimated bw */
	    BBR_PROBE_RTT, /**< Cut inflight to min to probe min_rtt */
	};

	/**
     * \brief Literal names of BBR mode for use in log messages
     */
    static const char* const BbrModeName[BBR_PROBE_RTT + 1];

    bool HasCongControl() const override;
    void CongControl(Ptr<TcpSocketState> tcb,
                     const TcpRateOps::TcpRateConnection& rc,
                     const TcpRateOps::TcpRateSample& rs) override;
    void CongestionStateSet(Ptr<TcpSocketState> tcb,
                            const TcpSocketState::TcpCongState_t newState) override;
    void CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) override;
    uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) override;
    Ptr<TcpCongestionOps> Fork() override;

	void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
	                      const Time& rtt);

	void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
	uint32_t ComputeQtCwnd (Ptr<TcpSocketState> tcb);
	void SetCwnd(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

	void SetRtarget_mult (double Rtarget_mult);
	void SetFairnessIndex (bool fairness_index);

protected:

    /**
     * \brief This method handles the steps related to the ProbeRTT state
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void CheckProbeRTT(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates variables specific to BBR_PROBE_RTT state
     */
    void EnterProbeRTT();

    /**
     * \brief Called on exiting from BBR_PROBE_RTT state, it eithers invoke EnterProbeBW () or
     * EnterStartup ()
     */
    void ExitProbeRTT();

    /**
     * \brief Gets BBR state.
     * \return returns BBR state.
     */
    uint32_t GetBbrState();

    /**
     * \brief Handles the steps for BBR_PROBE_RTT state.
     * \param tcb the socket state.
     */
    void HandleProbeRTT(Ptr<TcpSocketState> tcb);

    /**
     * \brief Modulates congestion window in BBR_PROBE_RTT.
     * \param tcb the socket state.
     */
    void ModulateCwndForProbeRTT(Ptr<TcpSocketState> tcb);

    /**
     * \brief Modulates congestion window in CA_RECOVERY.
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return true if congestion window is updated in CA_RECOVERY.
     */
    bool ModulateCwndForRecovery(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Helper to restore the last-known good congestion window
     * \param tcb the socket state.
     */
    void RestoreCwnd(Ptr<TcpSocketState> tcb);

    /**
     * \brief Helper to remember the last-known good congestion window or
     *        the latest congestion window unmodulated by loss recovery or ProbeRTT.
     * \param tcb the socket state.
     */
    void SaveCwnd(Ptr<const TcpSocketState> tcb);

    /**
     * \brief Updates control parameters congestion windowm, pacing rate, send quantum.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateControlParameters(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates BBR network model (Maximum bandwidth and minimum RTT).
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateModelAndState(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates minimum RTT.
     * \param tcb the socket state.
     */
    void UpdateRTprop(Ptr<TcpSocketState> tcb);

    /**
     * \brief Sets BBR state.
     * \param state BBR state.
     */
    void SetBbrState(BbrMode_t state);

private:
	    /**
	     * \as in Vegas, brief Enable QtOptimal algorithm to start taking QtOptimal samples
	     *
	     * as in Vewgas, QtOptimal algorithm is enabled in the following situations:
	     * 1. at the establishment of a connection
	     * 2. after an RTO
	     * 3. after fast recovery
	     * 4. when an idle connection is restarted
	     *
	     * \param tcb internal congestion state
	     */
	    void EnableQtOptimal(Ptr<TcpSocketState> tcb);

	    /**
	     * \as in Vegas, brief Stop taking QtOptimal samples
	     */
	    void DisableQtOptimal();

	    TracedValue<uint32_t> m_predictedBytesInFlight {0};

private:
	  //MaxBandwidthFilter_t m_maxBwFilter;        //!< Maximum bandwidth filter
      //uint32_t m_bandwidthWindowLength{10}; //!< A constant specifying the length of the BBR.BtlBw max
                                         //!< filter window, default 10 packet-timed round trips.
      //MinRttFilter_t m_minRttFilter;
      //uint32_t m_minRttWindowLength{10};
	  Time m_baseRtt;                    //!< Minimum of all ModNewReno RTT measurements seen during connection
	  double m_minRtt;                     //!< Minimum of all RTT measurements within last RTT
	  double m_Rtarget;
	  uint32_t m_cntRtt;                 //!< Number of RTT measurements during last RTT
	  double m_Rtarget_mult;
	  bool m_fairness_index;
	  bool m_doingQtOptimalNow;         //!< If true, do Vegas for this RTT
	  SequenceNumber32 m_begSndNxt;      //!< Right edge during last RTT
	  uint64_t m_delivered{0}; //!< The total amount of data in bytes delivered so far
	  uint32_t m_txItemDelivered{
        0}; //!< The number of bytes already delivered at the time of new packet transmission
      uint32_t m_nextRoundDelivered{0};           //!< Denotes the end of a packet-timed round trip
      uint32_t m_roundCount{0}; //!< Count of packet-timed round trips
      uint32_t m_roundCount_rtt{0};
      bool m_roundStart{false}; //!< A boolean that BBR sets to true once per packet-timed round trip
      Time m_probeRttDuration{MilliSeconds(200)}; //!< A constant specifying the minimum duration for
                                                //!< which ProbeRTT state, default 200 millisecs
    uint32_t m_minPipeCwnd {4000}; //!< The minimal congestion window value BBR tries to target, default 4 Segment size
    uint32_t m_priorCwnd {0};              //!< The last-known good congestion window
    Time m_rtProp {Time::Max()}; //!< Estimated two-way round-trip propagation delay of the path, estimated
                      //!< from the windowed minimum recent round-trip delay sample.
    bool m_rtPropExpired {false};         //!< A boolean recording whether the BBR.RTprop has expired
    Time m_rtPropFilterLen {Seconds(10)}; //!< A constant specifying the length of the RTProp min
                                         //!< filter window, default 10 secs.
    Time m_rtPropStamp {Seconds(0)}; //!< The wall clock time at which the current BBR.RTProp sample was obtained
    bool m_idleRestart{false};            //!< When restarting from idle, set it true
    Time m_probeRttDoneStamp {Seconds(0)}; //!< Time to exit from BBR_PROBE_RTT state
    bool m_probeRttRoundDone{false};      //!< True when it is time to exit BBR_PROBE_RTT
    BbrMode_t m_state {BbrMode_t::BBR_PROBE_BW}; //!< Current state of BBR state machine
    uint32_t m_appLimited {
        0}; //!< The index of the last transmitted packet marked as application-limited
    bool m_packetConservation{false};     //!< Enable/Disable packet conservation mode
    bool m_isInitialized{false}; //!< Set to true after first time initializtion variables

};



} //end namespace ns3

#endif /* TCP_QTOPTIMAL_H */

