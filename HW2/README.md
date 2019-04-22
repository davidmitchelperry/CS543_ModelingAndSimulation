Author: David Perry

Files Included:

Building:

	All folders contain a make file. Simply Cd to the correct directory
	and run make

report.pdf:
	My report in pdf form

prob1/prob1.c

	Simulates N M/M/1 queue and calculates run time
	

prob2/preempt/prob2.c:

	Simulates M/M/1 queue with preemption


prob2/non_preempt/prob2.c:
	
	Simulates M/M/1 queue without preemption


prob3/card_piling.c:

	Solved the card piling problem using a genetic algorithm.

Misc:

	I solved both prob 2 with modifications to my prob 1 solution. Therefore, the ability to do
	the preemption/non-preemption simulation with more than 1 queue. However, I never tested
	this functionality and it is likely the stats it produces are incorrect. However, if N set
	to 1 everything should function correctly.

	The largest number of customers served my program can handle is a little over 1000. This
	is due to a limitation of my queue structure implementation.
	


