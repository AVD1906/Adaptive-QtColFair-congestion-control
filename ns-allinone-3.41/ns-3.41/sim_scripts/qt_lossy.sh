#!/bin/bash
cd ..

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.000001 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.00001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.00001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.00001 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.00001 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.0001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.0001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.0001 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.0001 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.001 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.001 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.001 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpBbr" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.01 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpCubic" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.01 --BdpMultiplier=5 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.01 --BdpMultiplier=5 --rttTargetAlpha=1.2 --sim_duration=100"
./ns3 run "examples/tcp/tcp_qt_lossy --transport_prot="TcpQtColFair" --sim_name="../../results/qt_lossy" --num_flows=5 --btlBW="100Mbps" --btlDelay="20ms" --error_p=0.01 --BdpMultiplier=5 --rttTargetAlpha=1.5 --sim_duration=100"

