#!/bin/bash
cd ..

./ns3 run "examples/tcp/tcp_qt_add_new_flows --transport_prot="TcpBbr" --sim_name="../../results/qt_added_flows" --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_add_new_flows --transport_prot="TcpCubic" --sim_name="../../results/qt_added_flows" --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_add_new_flows --transport_prot="TcpQtCol" --sim_name="../../results/qt_added_flows" --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_add_new_flows --transport_prot="TcpQtCol" --sim_name="../../results/qt_added_flows" --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_add_new_flows --transport_prot="TcpQtColFair" --sim_name="../../results/qt_added_flows" --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_add_new_flows --transport_prot="TcpQtColFair" --sim_name="../../results/qt_added_flows" --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

