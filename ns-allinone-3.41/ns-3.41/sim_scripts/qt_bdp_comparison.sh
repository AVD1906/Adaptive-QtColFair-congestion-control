#!/bin/bash
cd ..

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.003 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.003 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.003 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.003 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.1 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.1 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.1 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.1 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.25 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.25 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.25 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.25 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.75 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.75 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.75 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.75 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=0.5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=1 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=1 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=1 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=1 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=2 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=2 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=3 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=3 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=3 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=3 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=4 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=4 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=4 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=4 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=6 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=6 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=6 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=6 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=7 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=7 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=7 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=7 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=8 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=8 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=8 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=8 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=9 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=9 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=9 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=9 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpBbr" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=10 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpCubic" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=10 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=10 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_static --transport_prot="TcpQtColFair" --sim_name="../../results/qt_bdp_comparison" --num_flows=1 --btlBW="100Mbps" --btlDelay="20ms" --BdpMultiplier=10 --rttTargetAlpha=1.5 --sim_duration=100"