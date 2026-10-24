#!/usr/bin/env python3
# Apply the first Adaptive-Loss QtColFair patch from the NS-3.41 root.
from pathlib import Path
import sys

root = Path.cwd()
hp = root / "src/internet/model/tcp-qt-col-fair.h"
cp = root / "src/internet/model/tcp-qt-col-fair.cc"
if not hp.is_file() or not cp.is_file():
    sys.exit("ERROR: Run this script from the NS-3.41 root directory.")

h = hp.read_text()
c = cp.read_text()

def rep(text, old, new, label):
    n = text.count(old)
    if n != 1:
        raise RuntimeError(f"{label}: expected one match, found {n}; no source files written.")
    return text.replace(old, new, 1)

try:
    h = rep(h,
        '#include "ns3/tcp-congestion-ops.h"',
        '#include "ns3/tcp-congestion-ops.h"\n#include "ns3/tcp-socket-state.h"',
        "header include")
    h = rep(h,
        '        void SetRttTargetMultiplier (double rttTarget_alpha);',
        '        void SetRttTargetMultiplier (double rttTarget_alpha);\n'
        '        uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,\n'
        '                              uint32_t bytesInFlight) override;\n'
        '        void CongestionStateSet (Ptr<TcpSocketState> tcb,\n'
        '                                 const TcpSocketState::TcpCongState_t newState) override;',
        "header declarations")
    h = rep(h,
        '      double m_rttTargetAlpha{1.5};',
        '      double m_rttTargetAlpha{1.2};\n'
        '      Time m_lastRtt {Time::Zero()};\n'
        '      bool m_enableAdaptiveLoss {true};\n'
        '      double m_betaMin {0.50};\n'
        '      double m_betaMax {0.95};\n'
        '      bool m_inLossTimeout {false};',
        "header fields")

    c = rep(c,
        'DoubleValue(1.5),\n                            MakeDoubleAccessor(&TcpQtColFair::SetRttTargetMultiplier),',
        'DoubleValue(1.2),\n                            MakeDoubleAccessor(&TcpQtColFair::SetRttTargetMultiplier),',
        "RTT multiplier")
    c = rep(c,
        '                .AddTraceSource("PredictedBytesInFlight",',
        '                .AddAttribute("EnableAdaptiveLoss",\n'
        '                            "Enable RTT-aware congestion-window reduction",\n'
        '                            BooleanValue(true),\n'
        '                            MakeBooleanAccessor(&TcpQtColFair::m_enableAdaptiveLoss),\n'
        '                            MakeBooleanChecker())\n'
        '                .AddAttribute("BetaMin",\n'
        '                            "Minimum congestion-window retention factor",\n'
        '                            DoubleValue(0.50),\n'
        '                            MakeDoubleAccessor(&TcpQtColFair::m_betaMin),\n'
        '                            MakeDoubleChecker<double>(0.0, 1.0))\n'
        '                .AddAttribute("BetaMax",\n'
        '                            "Maximum congestion-window retention factor",\n'
        '                            DoubleValue(0.95),\n'
        '                            MakeDoubleAccessor(&TcpQtColFair::m_betaMax),\n'
        '                            MakeDoubleChecker<double>(0.0, 1.0))\n'
        '                .AddTraceSource("PredictedBytesInFlight",',
        "adaptive attributes")
    c = rep(c,
        '    m_rttTargetAlpha (sock.m_rttTargetAlpha),\n    m_probeRttDuration (sock.m_probeRttDuration),',
        '    m_rttTargetAlpha (sock.m_rttTargetAlpha),\n'
        '    m_lastRtt (sock.m_lastRtt),\n'
        '    m_enableAdaptiveLoss (sock.m_enableAdaptiveLoss),\n'
        '    m_betaMin (sock.m_betaMin),\n'
        '    m_betaMax (sock.m_betaMax),\n'
        '    m_inLossTimeout (sock.m_inLossTimeout),\n'
        '    m_probeRttDuration (sock.m_probeRttDuration),',
        "copy constructor")
    c = rep(c,
        '    m_rttProp= std::min(m_rttProp, rtt);\n'
        '    m_rttTarget = m_rttTargetAlpha*m_rttProp;\n'
        '    //UpdateRttProp(tcb);',
        '    m_lastRtt = rtt;\n\n'
        '    const Time now = Simulator::Now();\n'
        '    if (m_rttProp == Time::Max() ||\n'
        '        rtt <= m_rttProp ||\n'
        '        now - m_rttPropStamp > m_rttPropFilterLen)\n'
        '    {\n'
        '        m_rttProp = rtt;\n'
        '        m_rttPropStamp = now;\n'
        '        m_rttPropExpired = false;\n'
        '    }\n\n'
        '    m_rttTarget = m_rttTargetAlpha * m_rttProp;',
        "RTT tracking")
    c = rep(c,
        'std::string\nTcpQtColFair::GetName () const',
        'uint32_t\n'
        'TcpQtColFair::GetSsThresh (Ptr<const TcpSocketState> tcb,\n'
        '                           uint32_t bytesInFlight)\n'
        '{\n'
        '    const uint32_t legacyThreshold =\n'
        '        std::max(2 * tcb->m_segmentSize, bytesInFlight / 2);\n\n'
        '    if (!m_enableAdaptiveLoss || m_lastRtt.IsZero() ||\n'
        '        m_rttTarget.IsZero() || m_inLossTimeout)\n'
        '    {\n'
        '        m_inLossTimeout = false;\n'
        '        return legacyThreshold;\n'
        '    }\n\n'
        '    uint32_t effectiveBytes = std::max(bytesInFlight, tcb->m_cWnd.Get());\n'
        '    if (m_probeRtt || m_usePriorInFlight)\n'
        '    {\n'
        '        effectiveBytes = std::max(effectiveBytes, m_priorInFlight);\n'
        '    }\n\n'
        '    const double rtt = m_lastRtt.GetSeconds();\n'
        '    const double target = m_rttTarget.GetSeconds();\n'
        '    double beta = m_betaMax;\n'
        '    if (rtt > target)\n'
        '    {\n'
        '        const double overshoot = (rtt - target) / target;\n'
        '        beta = m_betaMax - (m_betaMax - m_betaMin) * std::min(1.0, overshoot);\n'
        '        beta = std::max(m_betaMin, std::min(m_betaMax, beta));\n'
        '    }\n\n'
        '    const uint32_t targetBytes = static_cast<uint32_t>(beta * effectiveBytes);\n'
        '    const uint32_t minWindow = 4 * tcb->m_segmentSize;\n'
        '    return std::max(minWindow, targetBytes);\n'
        '}\n\n'
        'void\n'
        'TcpQtColFair::CongestionStateSet (\n'
        '    Ptr<TcpSocketState> tcb,\n'
        '    const TcpSocketState::TcpCongState_t newState)\n'
        '{\n'
        '    if (newState == TcpSocketState::CA_LOSS)\n'
        '    {\n'
        '        m_inLossTimeout = true;\n'
        '    }\n'
        '    else if (newState == TcpSocketState::CA_OPEN ||\n'
        '             newState == TcpSocketState::CA_DISORDER)\n'
        '    {\n'
        '        m_inLossTimeout = false;\n'
        '    }\n'
        '    TcpNewReno::CongestionStateSet(tcb, newState);\n'
        '}\n\n'
        'std::string\nTcpQtColFair::GetName () const',
        "adaptive loss methods")
except RuntimeError as e:
    sys.exit("PATCH ABORTED: " + str(e))

hb = hp.with_suffix(".h.pre_adaptive")
cb = cp.with_suffix(".cc.pre_adaptive")
if not hb.exists():
    hb.write_text(hp.read_text())
if not cb.exists():
    cb.write_text(cp.read_text())
hp.write_text(h)
cp.write_text(c)
print("Patch applied successfully.")
print(f"Original header backup: {hb}")
print(f"Original source backup: {cb}")
print("Next, run: ./ns3 build")
