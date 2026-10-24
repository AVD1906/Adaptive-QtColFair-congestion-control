#!/bin/bash
cd ..

./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpBbr" --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpCubic" --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpQtOptimal" --Rt_mult=1.1 --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpQtOptimal" --Rt_mult=1.2 --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpQtOptimal" --Rt_mult=1.3 --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpQtOptimal" --Rt_mult=1.4 --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpQtOptimal" --Rt_mult=1.5 --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"
./ns3 run "examples/tcp/tcp_qt_optimal_sims --transport_prot="TcpQtOptimal" --Rt_mult=2 --sim_name="../../results/tcp_qt_optimal_sims_wireless" --error_p=0.0001 --num_flows=1 --btlBW="10Mbps" --btlDelay="100ms" --sim_duration=200"