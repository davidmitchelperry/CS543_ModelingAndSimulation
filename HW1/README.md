Author: David Perry

Files Included:

Building:

	All folders contain a make file. Simply Cd to the correct directory
	and run make

prob1/prob1_dist_dir/prob1_dist.c:
	
	Calculates the distribution of exponentially random generated numbers

prob1/prob1_expon_dir/prob1_expon.c:

	Calculates average of expon RNG

prob1/prob1_uniform_dir/prob1_uniform.c:

	Does the x^2 test on numbers from uniform RN

prob2/prob2.c:
	
	does M/M/1 queue simulation

prob3/prob3.c:
	
	Does M/M/1 simulation with modification

	This program takes commands from the command line.
	The first parameter is the seed and the second is
	the K value.

Assumptions:

	In both prob2.c and prob3.c I implement a queue using a statically
	sized array. As of now I have used the value nmax * 2 since the queue
	should not be able to grow to that size. 

