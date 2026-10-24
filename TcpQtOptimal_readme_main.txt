TcpQtOptimal development and simulations
=============

Description
-----------
TcpQtOptimal is developed in ns3 (tested in ns3.40 and ns3.41) to illustrate the use Queuing-Based Dynamical Systems Theory in end-to-end TCP congestion control. The full development can be found at https://github.com/dumisa/TowardsOptimalTcp. The development is linked to an article entitled "Towards Optimal End-to-end TCP Congestion Control Using Queuing-Based Dynamical Systems Theory".

Important files added in ns3.41
-------------------------------
Internet module:
	* tcp-ql-optimal.h and tcp-ql-optimal.cc implement TcpQtOptimal and are found at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/src/internet/model
tcp examples:
	* Simulations in the paper for single flow microscopic and macroscopic used my_sim_basic_metrics.cc at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/examples/tcp
	* Simulations in the paper for single flow for stochstic links used my_sim_StochBW.cc at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/examples/tcp
	* Simulations in the paper for multi flow faitness analysis used my_sim_fairness.cc at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/examples/tcp
Contributed modules required:
	* my-dumbbell module at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/contrib is used by some examples
	* my-dumbbell module consists of my_dumbbell.h and my_dumbbell.cc at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/contrib/my-dumbbell/model
	* my_maths_tools module may be usefull some mathematical rqeuirements - https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/contrib
bash script files:
	bash script files have been included for convinience at https://github.com/dumisa/TowardsOptimalTcp/tree/master/ns-allinone-3.41/ns-3.41/sim_scripts

Usage
-----
(1) To view simulation graphs open .png files at https://github.com/dumisa/TowardsOptimalTcp/tree/master/python_graphs
(2) Data from simulations is stored at https://github.com/dumisa/TowardsOptimalTcp/tree/master/results
(3) To modify graphs use .ipynb files (Jupyter Notebook files) at https://github.com/dumisa/TowardsOptimalTcp/tree/master/python_graphs. Ensure correct directors at https://github.com/dumisa/TowardsOptimalTcp/tree/master/results are used.
(4) To implement TcpQtOptimal:
	Option 1: clone https://github.com/dumisa/TowardsOptimalTcp using
		git@github.com:dumisa/TowardsOptimalTcp.git,
		https://github.com/dumisa/TowardsOptimalTcp.git, or
		gh repo clone dumisa/TowardsOptimalTcp 
	Option 2: prepare ns3.41 (from ns3 location) for installation and the following:
		- add tcp-ql-optimal.h and tcp-ql-optimal.cc files into ns-allinone-3.41/ns-3.41/src/internet/model
		- add my_sim_basic_metrics.cc, my_sim_StochBW.cc, and my_sim_fairness.cc files into ns-allinone-3.41/ns-3.41/examples/tcp
		- add my-dumbbell and my_maths_tools directories (modules) into ns-allinone-3.41/ns-3.41/contrib
		- install ns3.41 accordingly

License
-------

Contact
-------
ngwenyad@sentech.co.za; dumisa@crocs.co.za.

Additional Information
-----------------------
This development is for experimental use.
