
#include "ns3/my-dumbbell.h"
#include "ns3/my_maths_tools.h"
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

#include <fstream>
#include <iostream>
#include <string>

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/enum.h"
//#include "ns3/error-model.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-header.h"
#include "ns3/traffic-control-module.h"
#include "ns3/udp-header.h"
#include "ns3/data-rate.h"

using namespace ns3;


std::string dir;
std::string file_prefix;

static bool firstAgg=true;                      //!< First congestion window.
static std::map<uint32_t, bool> firstCwnd;                      //!< First congestion window.
static std::map<uint32_t, bool> firstInFlight;                      //!< First congestion window.
static std::map<uint32_t, bool> firstSshThr;                    //!< First SlowStart threshold.
static std::map<uint32_t, bool> firstRtt;                       //!< First RTT.
static std::map<uint32_t, bool> firstRto;                       //!< First RTO.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream; //!< Congstion window output stream.
static Ptr<OutputStreamWrapper> AggStream ; //!< Congstion window output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> sinkRxStream; //!< Congstion window output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> ssThreshStream; //!< SlowStart threshold output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;      //!< RTT output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rtoStream;      //!< RTO output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> nextTxStream;   //!< Next TX output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> nextRxStream;   //!< Next RX output stream.
static std::map<uint32_t, Ptr<OutputStreamWrapper>> inFlightStream; //!< In flight output stream.
static std::map<uint32_t, uint32_t> cWndValue;                      
static std::map<uint32_t, uint32_t> inFlightValue;                      
static std::map<uint32_t, double> RttValue;                      
static std::map<uint32_t, uint32_t> ssThreshValue;                  //!< SlowStart threshold value.
static std::map<uint32_t, uint32_t> bytes_rx;
static std::map<uint32_t, uint32_t> last_bytes_rx;
static std::map<uint32_t, double> time_bytes_rx;
static std::map<uint32_t, double> last_time_bytes_rx;
static std::map<uint32_t, double> last_sending_rate;

static uint64_t  AggTxPkts;
static uint64_t  AggRetransPkts;
static uint64_t  AggLostPkts;
static uint32_t  pktsize = 1448; //1448 default but may be changed during simulation
static SequenceNumber32          UnackSeq;

static std::map<uint32_t, uint64_t>          TxPkts;
static std::map<uint32_t, uint64_t>          RetransPkts;
static std::map<uint32_t, uint64_t>          LostPkts;

static std::vector<uint64_t>  Last_Source_UnackSeq_value;
static std::vector<double>  Last_Source_UnackSeq_time;
static std::vector<double>  measured_source_btlBW;

std::string transport_prot = "TcpWestwoodPlus";
double error_p = 0.0;
bool tracing = true;
bool DoFairness = false;
uint32_t SegmentSize = 1448;
uint8_t num_flows = 3;
std::string queue_disc_type = "PfifoFastQueueDisc";
std::string recovery = "TcpClassicRecovery";

std::string btlBW = "10Mbps";
std::string accessBW = "10Gbps";
std::string btlDelay = "100ms";
std::string accessDelay = "0.01ms";
double BdpMultiplier = 5; //packets
double sim_duration = 100.0;
std::string sim_name="default";
double rttTargetAlpha = 1.5;

/**
 * Get the Node Id From Context.
 *
 * \param context The context.
 * \return the node ID.
 */
static uint32_t
GetNodeIdFromContext(std::string context)
{
    const std::size_t n1 = context.find_first_of('/', 1);
    const std::size_t n2 = context.find_first_of('/', n1 + 1);
    return std::stoul(context.substr(n1 + 1, n2 - n1 - 1));
}

/**
 * Congestion window tracer.
 *
 * \param context The context.
 * \param oldval Old value.
 * \param newval New value.
 */
static void
CwndTracer(std::string context, uint32_t oldval, uint32_t newval)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

  static std::map<uint32_t, double> last_time;
  static double now_time;
  static double agg_last_time;
  static double last_aggcWnd;
  static std::map<uint32_t, double> insThroughput;
  static double insAggThroughput;
  static std::map<uint32_t, double> aveThroughput;
  static double aveAggThroughput;
  static std::map<uint32_t, double> insGoodput;
  static double insAggGoodput;
  static std::map<uint32_t, double> aveGoodput;
  static double aveAggGoodput;
  static std::map<uint32_t, uint64_t> last_TxPkts;
  static uint64_t last_AggTxPkts;
  static std::map<uint32_t, uint64_t> last_RetransPkts;
  static uint64_t last_AggRetransPkts;
  static std::map<uint32_t, uint64_t> last_cWndValue;

    cWndValue[nodeId] = newval;
    now_time =  Simulator::Now().GetSeconds();
  if (firstCwnd[nodeId])
  {
    last_time[nodeId] = now_time;
    insThroughput[nodeId] = 0;
    aveThroughput[nodeId] = 0;
    last_TxPkts[nodeId] = 0;
    last_RetransPkts[nodeId] = 0;
    firstCwnd[nodeId] = false;
    last_cWndValue[nodeId] = 0;
  }

  if (now_time - last_time[nodeId] > 0)
  {      
      insThroughput[nodeId] = (TxPkts[nodeId] - last_TxPkts[nodeId])*8*pktsize*1e-6/
                           (now_time - last_time[nodeId]);
      aveThroughput[nodeId] = TxPkts[nodeId]*8*pktsize*1e-6/now_time;

      insGoodput[nodeId] = ((TxPkts[nodeId] - RetransPkts[nodeId])  - (last_TxPkts[nodeId] - last_RetransPkts[nodeId]))*
                            8*pktsize*1e-6/
                            (now_time - last_time[nodeId]);
      aveGoodput[nodeId] = (TxPkts[nodeId]-RetransPkts[nodeId])*8*pktsize*1e-6/now_time;

      last_TxPkts[nodeId] = TxPkts[nodeId];
      last_RetransPkts[nodeId] = RetransPkts[nodeId];
      last_time[nodeId] = now_time;
  }

  if (now_time - agg_last_time > 0)
  {      
      insAggThroughput = (AggTxPkts - last_AggTxPkts)*8*pktsize*1e-6/
                           (now_time - agg_last_time);
      aveAggThroughput = AggTxPkts*8*pktsize*1e-6/now_time;

      insAggGoodput = ((AggTxPkts - AggRetransPkts)  
                            - (last_AggTxPkts - last_AggRetransPkts))*
                            8*pktsize*1e-6/
                            (now_time - agg_last_time);
      aveAggGoodput = (AggTxPkts-AggRetransPkts)*8*pktsize*1e-6/now_time;

      last_AggTxPkts = AggTxPkts;
      last_AggRetransPkts = AggRetransPkts;
      agg_last_time = now_time;
  }

  DataRate BW(btlBW);
  Time delay(btlDelay);
  double RTT = 2*delay.GetSeconds ();
  double Rt = rttTargetAlpha*RTT;
  double bdp = BW.GetBitRate ()*Rt/8/1448.0;

  double cost = ((inFlightValue[nodeId]/1448.0/bdp - 1)/inFlightValue.size ())*((inFlightValue[nodeId]/1448.0/bdp - 1)/inFlightValue.size ())
            + (RttValue[nodeId] - Rt)*(RttValue[nodeId] - Rt) 
            + ((cWndValue[nodeId]/1448.0 - last_cWndValue[nodeId]/1448.0)/bdp)*((cWndValue[nodeId]/1448.0 - last_cWndValue[nodeId]/1448.0)/bdp);

  *cWndStream[nodeId]->GetStream() << now_time 
            << " " << cWndValue[nodeId]
            << " " << inFlightValue[nodeId]
            << " " << RttValue[nodeId]
            << " " << insThroughput[nodeId]  
            << " " << aveThroughput[nodeId]
            << " " << insGoodput[nodeId]  
            << " " << aveGoodput[nodeId]
            << " " << TxPkts[nodeId]
            << " " << RetransPkts[nodeId]
            << " " << cost
            << std::endl;

    std::cout << now_time
              << " NodeId=" << nodeId
              << " inFlightValue=" << inFlightValue[nodeId]/1448 <<"pkts"
              << " RttValue=" << RttValue[nodeId] << "s"
              << " aveGoodput=" << std::ceil(100*aveGoodput[nodeId])/100 <<"Mbps"  
              << " cost=" << std::ceil(1000*cost)/1000  
              << std::endl;

    double aggcWnd=0;
    for (auto it=cWndValue.begin (); it != cWndValue.end(); it++)
    {
        aggcWnd = aggcWnd + it->second;
    }

    double agginFlight=0;
    for (auto it=inFlightValue.begin (); it != inFlightValue.end(); it++)
    {
        agginFlight = agginFlight + it->second;
    }

    double sumRtt=0;
    for (auto it=RttValue.begin (); it != RttValue.end(); it++)
    {
        sumRtt = sumRtt + it->second;
    }
    double aveRtt= sumRtt/RttValue.size ();

    double fairness=0.0;
    double sum_x=0.0, sum_x2=0.0;
    for (auto it=aveThroughput.begin (); it != aveThroughput.end(); it++)
    {
        sum_x = sum_x + it->second;
        sum_x2 = sum_x2 + (it->second)*(it->second);
    }
    fairness = (sum_x*sum_x/sum_x2)/aveThroughput.size ();
    if(isnan(fairness)) fairness = 0;

    double aggcost = (agginFlight/1448.0/bdp - 1)*(agginFlight/1448.0/bdp - 1)
            + (aveRtt - Rt)*(aveRtt - Rt) 
            + ((aggcWnd/1448.0 - last_aggcWnd/1448.0)/bdp)*((aggcWnd/1448.0 - last_aggcWnd/1448.0)/bdp);


  if (firstAgg)
  {
        *AggStream->GetStream() << "| now_time " 
                << "| aggcWnd "
                << "| agginFlight "
                << "| aveRtt "
                << "| insAggThroughput "  
                << "| aveAggThroughput "
                << "| insAggGoodput "  
                << "| aveAggGoodput "
                << "| AggTxPkts "
                << "| AggRetransPkts "
                << "| fairness "
                << "| aggcost |"
                << std::endl; 
        firstAgg = false;

  }

  *AggStream->GetStream() << now_time 
            << " " << aggcWnd
            << " " << agginFlight
            << " " << aveRtt
            << " " << insAggThroughput  
            << " " << aveAggThroughput
            << " " << insAggGoodput  
            << " " << aveAggGoodput
            << " " << AggTxPkts
            << " " << AggRetransPkts
            << " " << fairness
            << " " << aggcost
            << std::endl;

    std::cout << now_time
              << " NodeId=" << nodeId
              << " agginFlight=" << agginFlight/1448 <<"pkts"
              << " aggcWnd=" << aggcWnd/1448 <<"pkts"
              << " aveRtt=" << aveRtt << "s"
              << " sumRtt=" << sumRtt << "s" <<std::endl
              << " aveAggGoodput=" << std::ceil(100*aveAggGoodput)/100 <<"Mbps" 
              << " aveAggThroughput=" << std::ceil(100*aveAggThroughput)/100 <<"Mbps"  << std::endl 
              << " fairness=" << std::ceil(100*fairness)/100 
              << std::endl;
    //getchar();

    last_cWndValue[nodeId] = cWndValue[nodeId];
    last_aggcWnd = aggcWnd;
}


/**
 * RTT tracer.
 *
 * \param context The context.
 * \param oldval Old value.
 * \param newval New value.
 */
static void
RttTracer(std::string context, Time oldval, Time newval)
{
    uint32_t nodeId = GetNodeIdFromContext(context);

    RttValue[nodeId] = newval.GetSeconds();

    if (firstRtt[nodeId])
    {
        //*rttStream[nodeId]->GetStream() << "0.0 " << oldval.GetSeconds() << std::endl;
        firstRtt[nodeId] = false;
    }


 /*   *rttStream[nodeId]->GetStream()
        << Simulator::Now().GetSeconds() << " " << newval.GetSeconds() << std::endl;*/

}

static void
InFlightTracer(std::string context, uint32_t old [[maybe_unused]], uint32_t inFlight)
{
    uint32_t nodeId = GetNodeIdFromContext(context);
    static std::map<uint32_t, uint64_t> last_cWndValue;

    inFlightValue[nodeId] = inFlight;

    if (firstInFlight[nodeId])
  {
    firstInFlight[nodeId] = false;
    last_cWndValue[nodeId] = 0;
  }

  DataRate BW(btlBW);
  Time delay(btlDelay);
  double RTT = 2*delay.GetSeconds ();
  double Rt = rttTargetAlpha*RTT;
  double bdp = BW.GetBitRate ()*Rt/8/1448.0;

  double cost = ((inFlightValue[nodeId]/1448.0/bdp - 1)/inFlightValue.size ())*((inFlightValue[nodeId]/1448.0/bdp - 1)/inFlightValue.size ())
            + (RttValue[nodeId] - Rt)*(RttValue[nodeId] - Rt) 
            + ((cWndValue[nodeId]/1448.0 - last_cWndValue[nodeId]/1448.0)/bdp)*((cWndValue[nodeId]/1448.0 - last_cWndValue[nodeId]/1448.0)/bdp);

/*    *inFlightStream[nodeId]->GetStream() << Simulator::Now().GetSeconds() 
                                         << " " << inFlightValue[nodeId]
                                         << " " << cWndValue[nodeId]
                                         << " " << RttValue[nodeId]
                                         << " " << cost
                                         << std::endl;*/

    last_cWndValue[nodeId] = cWndValue[nodeId];
}

static void TcpTxDataTracer (std::string context, const Ptr<const Packet> packet, 
                                    const TcpHeader& header, const Ptr<const TcpSocketBase> socket)
{
 uint32_t nodeId = GetNodeIdFromContext(context);

  if (packet->GetSize () > 100) //to make sure we have the correct trigger
    {
      //*PktsSentStream->GetStream () << Simulator::Now ().GetSeconds ();
      pktsize = packet->GetSize ();

      AggTxPkts += 1;
      TxPkts[nodeId] += 1;

      //*PktsSentStream->GetStream () << " S" << source << " " << 1;
      if (socket->GetTxBuffer ()->IsHeadRetransmitted ()) 
      {
        AggRetransPkts += 1;
        RetransPkts[nodeId] += 1;
      }
      else
      {
        //*PktsSentStream->GetStream () << " " << 0;
      }
      if (socket->GetTxBuffer ()->IsLost(UnackSeq)) 
      {
        AggLostPkts += 1;

      }
    }  

}


/**
 * Congestion window trace connection.
 *
 * \param cwnd_tr_file_name Congestion window trace file name.
 * \param nodeId Node ID.
 */
static void
TraceCwnd(std::string cwnd_tr_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    cWndStream[nodeId] = ascii.CreateFileStream(cwnd_tr_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
                    MakeCallback(&CwndTracer));
}


/**
 * RTT trace connection.
 *
 * \param rtt_tr_file_name RTT trace file name.
 * \param nodeId Node ID.
 */
static void
TraceRtt(std::string rtt_tr_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    //rttStream[nodeId] = ascii.CreateFileStream(rtt_tr_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) + "/$ns3::TcpL4Protocol/SocketList/0/RTT",
                    MakeCallback(&RttTracer));
}


/**
 * In flight trace connection.
 *
 * \param in_flight_file_name In flight trace file name.
 * \param nodeId Node ID.
 */
static void
TraceInFlight(std::string& in_flight_file_name, uint32_t nodeId)
{
    AsciiTraceHelper ascii;
    //inFlightStream[nodeId] = ascii.CreateFileStream(in_flight_file_name);
    Config::Connect("/NodeList/" + std::to_string(nodeId) +
                        "/$ns3::TcpL4Protocol/SocketList/0/BytesInFlight",
                    MakeCallback(&InFlightTracer));
}

static void TraceTcpTxData(uint32_t nodeId)
{
    Config::Connect ("/NodeList/" + std::to_string(nodeId) + 
              "/$ns3::TcpL4Protocol/SocketList/*/Tx", 
              MakeCallback (&TcpTxDataTracer));
}


void ChangeBW (dumbbell dumbbellSim, uint64_t bps)
{
  PointToPointNetDevice *device=static_cast<PointToPointNetDevice *>(PeekPointer(dumbbellSim.GetRouterDevices (0)));
  device->SetDataRate(DataRate(bps));
}

void StochBW (dumbbell dumbbellSim)
{
  static uint64_t mean_bps;
  static uint64_t StochBW_last_bps = 0;

  PointToPointNetDevice *device=static_cast<PointToPointNetDevice *>(PeekPointer(dumbbellSim.GetRouterDevices (0)));
  DataRateValue bps;
  device->GetAttribute("DataRate", bps);
  
  if (StochBW_last_bps != bps.Get ().GetBitRate ()) mean_bps = bps.Get ().GetBitRate ();

  static std::default_random_engine generator;
  std::normal_distribution<double> 
                distribution(mean_bps, 0.2*mean_bps);
  StochBW_last_bps = (uint64_t) distribution(generator);
  if (StochBW_last_bps > mean_bps) StochBW_last_bps = mean_bps;

  ChangeBW(dumbbellSim, StochBW_last_bps);
  Simulator::Schedule (Seconds (1), &StochBW, dumbbellSim);
  
}

void ChangeDelay (dumbbell dumbbellSim, double delay)
{
  PointToPointNetDevice *device=static_cast<PointToPointNetDevice *>(PeekPointer(dumbbellSim.GetRouterDevices (0)));
  Ptr <PointToPointChannel> p2pch = DynamicCast <PointToPointChannel> (device->GetChannel ());
  p2pch->SetAttribute ("Delay", TimeValue (Seconds (delay)));
}


int main (int argc, char *argv[])
{

    CommandLine cmd (__FILE__);
    cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, TcpLinuxReno, "
                "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat, "
                "TcpLp, TcpDctcp, TcpCubic, TcpBbr, TcpBbrV2, TcpQtCol, TcpQtColFair", transport_prot);
    cmd.AddValue ("error_p", "Packet error rate", error_p);
    cmd.AddValue ("btlBW", "Bottleneck bandwidth", btlBW);
    cmd.AddValue ("btlDelay", "Bottleneck delay", btlDelay);
    cmd.AddValue ("accessBW", "Access link bandwidth", accessBW);
    cmd.AddValue ("accessDelay", "Access link delay", accessDelay);
    cmd.AddValue ("sim_name", "Prefix of output trace file", sim_name);
    cmd.AddValue ("queue_disc_type", "Queue Disc Type", queue_disc_type);
    cmd.AddValue ("BdpMultiplier", "Size of the Queue Disc", BdpMultiplier);
    cmd.AddValue ("num_flows", "Number of flows", num_flows);
    cmd.AddValue ("sim_duration", "simulation duration in seconds", sim_duration);
    cmd.AddValue ("rttTargetAlpha", "simulation duration in seconds", rttTargetAlpha);
    cmd.AddValue ("DoFairness", "simulation duration in seconds", DoFairness);
    cmd.Parse (argc, argv);

    //This configuration is important to ensure certainity or minimal requirement for uncontrolled queues
    Config::SetDefault ("ns3::DropTailQueue<Packet>::MaxSize", StringValue ("1p"));
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::" + transport_prot));
    Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1073741824));
    Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1073741824));
    Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));
    uint32_t delAckCount = 2;
    Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (delAckCount));
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1448)); 

    if (transport_prot == "TcpQtCol")
    {
        Config::SetDefault ("ns3::TcpQtCol::rttTargetAlpha", DoubleValue (rttTargetAlpha));
        std::cout << " transport_prot = " <<  transport_prot << "_" << rttTargetAlpha << "minRTT" << std::endl;
    }
    else if (transport_prot == "TcpQtColFair")
    {
        Config::SetDefault ("ns3::TcpQtColFair::rttTargetAlpha", DoubleValue (rttTargetAlpha));
        std::cout << " transport_prot = " <<  transport_prot << "_" << rttTargetAlpha << "minRTT" << std::endl;
    }
    else
    {
        std::cout << " transport_prot = " <<  transport_prot << std::endl;
    }

    sim_duration = 200.0;
    // Set the simulation start and stop time
    double start_time = 0.001;
    double stop_time = start_time + sim_duration;

    DataRate access_b(accessBW);
    DataRate bottle_b(btlBW);
    Time access_d(accessDelay);
    Time bottle_d(btlDelay);

    uint32_t queueSize_bytes = static_cast<uint32_t>(BdpMultiplier * (std::min(access_b, bottle_b).GetBitRate() / 8) *
                                          ((access_d +  bottle_d) * 2).GetSeconds());
    uint32_t queueSize_pkts = queueSize_bytes/SegmentSize;

    std::vector<double> app_start;
    std::vector<double> app_stop;

    num_flows = 5;
    for (uint32_t i = 0; i<num_flows; i++)
    {
        app_start.push_back(start_time);
        app_stop.push_back(stop_time);
    }
    app_start[2] = 10;
    app_start[3] = 35;
    app_start[4] = 60;

    dumbbell dumbbellSim(num_flows, btlBW, btlDelay, accessBW, accessDelay, 
            queue_disc_type, queueSize_pkts, error_p, app_start, app_stop);

    std::cout << "dumbbell created successfully " << std::endl;

    // Simulator::Schedule (Seconds (0.00001), &TraceInFlight, file_prefix + "-inflight.data");
    // Create a new directory to store the output of the program

    std::string file_transport_prot = transport_prot;
    if (transport_prot == "TcpQtCol" || transport_prot == "TcpQtColFair")
    {
        uint32_t num1 = rttTargetAlpha*10;
        uint32_t num2 = num1/10;
        uint32_t num3 = num1 % 10;

        file_transport_prot = file_transport_prot + "_" + std::to_string(num2)+ "." + std::to_string(num3) + "minRTT";
    }

    file_prefix = file_transport_prot  + "-" + std::to_string(num_flows) + "-flows-" 
              + btlBW + "-" + btlDelay + "-" 
              + std::to_string(queueSize_pkts) + "p";

    dir = sim_name + "/" + file_transport_prot + "/" + std::to_string(num_flows) + "-flows/" 
              + btlBW + "-" + btlDelay + "/" + std::to_string(queueSize_pkts) + "p-btlqueue/";
    std::string dirToSave = "mkdir -p " + dir;

    if (system (dirToSave.c_str ()) == -1)
    {
      exit (1);
    } 
    //std::cout << "Debug point 1 - Press ENTER to continue" << std::endl;
    //getchar();



    // Set up tracing if enabled
    if (tracing)
    {
        AsciiTraceHelper ascii;
        std:: string cwnd_tr_file_name = dir + file_prefix + "-Agg"+ "-cwnd.data";
        AggStream = ascii.CreateFileStream(cwnd_tr_file_name);

        for (uint16_t index = 0; index < num_flows; index++)
        {
            uint32_t source_nodeId = dumbbellSim.GetLeftLeaf (index)->GetId ();
            //uint32_t sink_nodeId = dumbbellSim.GetRightLeaf (index)->GetId ();

            std::string flowString;

            flowString = "-flow" + std::to_string(index);

            TxPkts[source_nodeId] = 0;
            RetransPkts[source_nodeId] = 0;
            LostPkts[source_nodeId] = 0;
            cWndValue[source_nodeId] = 0;
            inFlightValue[source_nodeId] = 0;
            RttValue[source_nodeId] = 0;

            Simulator::Schedule(Seconds(app_start[index] + 0.001),
                                &TraceCwnd,
                                dir + file_prefix + flowString + "-cwnd.data",
                                source_nodeId);

            Simulator::Schedule(Seconds(app_start[index] + 0.001),
                                &TraceRtt,
                                dir + file_prefix + flowString + "-rtt.data",
                                source_nodeId);

            Simulator::Schedule(Seconds(app_start[index] + 0.001),
                                &TraceInFlight,
                                dir + file_prefix + flowString + "-inflight.data",
                                source_nodeId);

            Simulator::Schedule(Seconds(app_start[index] + 0.001),
                                &TraceTcpTxData,
                               source_nodeId);
        }

    }
    std::cout << "simulation schedule done " << std::endl;

    // Simulator::Schedule (Seconds (0.0001), &ChangeBW, dumbbellSim, 100e6);
    // Simulator::Schedule (Seconds (25), &ChangeBW, dumbbellSim, 400e6);
    // Simulator::Schedule (Seconds (55), &ChangeBW, dumbbellSim, 100e6);
    //Simulator::Schedule (Seconds (40), &ChangeBW, dumbbellSim, 5e6);
    //Simulator::Schedule (Seconds (1), &StochBW, dumbbellSim);
    //Simulator::Schedule (Seconds (20), &ChangeDelay, dumbbellSim, 0.01);
    //Simulator::Schedule (Seconds (40), &ChangeDelay, dumbbellSim, 0.1);

    Simulator::Stop (Seconds (sim_duration));
    std::cout << "simulation started and duration is " 
                << Seconds (sim_duration).GetSeconds () << " seconds" << std::endl;
    Simulator::Run ();
    std::cout << "simulation completed " << std::endl;

    Simulator::Destroy();
    return 0;
}

