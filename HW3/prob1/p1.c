/* simulate an M/M/1 queue
  (an open queue with exponential service times and interarrival intervals)
*/

#include <stdio.h>
#include <stdlib.h>
#include "csim.h"

#define	SVTM	1.0	/*mean of service time distribution */
#define IATM_LP 	2.0		/*mean of inter-arrival time distribution */
#define NARS	1000	/*number of arrivals to be simulated*/

double IATM_HP;
FACILITY f;		/*pointer for facility */
EVENT done;		/*pointer for counter */
TABLE tbl_lp;		/*pointer for table */
QTABLE qtbl_lp;		/*pointer for qhistogram */
TABLE tbl_hp;		/*pointer for table */
QTABLE qtbl_hp;		/*pointer for qhistogram */
int cnt;		/*number of active tasks*/
void cust();
void dispatch();
void lp_dispatch();

int LP_READY = 1;

FACILITY fac_array[8];
QTABLE q_array[8];
TABLE t_array[8];
TABLE system_tbl;
QTABLE system_qtbl;



void sim(int argc, char* argv[])				/*1st process - named sim */
{

	IATM_HP = atof(argv[2]);	

	int i;

	// use the first parameter of the program as the seed
	reseed(NIL, (float)atoi(argv[1]));

	set_model_name("M/M/1 Queue");
	create("sim");				/*required create statement*/

	for(i = 0; i < 8; i++) {
		char f_temp[11];
		sprintf(f_temp, "facility %d", i);
		fac_array[i] = facility(f_temp);
		char t_temp[15];
		sprintf(t_temp, "resp tms fac %d", i);
		//t_array[i] = table(t_temp);
		char q_temp[15];
		sprintf(q_temp, "num in fac %d", i);
		//q_array[i] = qhistogram(q_temp, 101); 
	}
	system_tbl = table("system table");
	//system_qtbl = qhistogram("system qtable", 101);
	done = event("done");			/*initialize event*/
	
	cnt = NARS;				/*initialize cnt*/

	// Call the Functions that dispatch customers
	dispatch();

	wait(done);				/*wait until all done*/
	report();				/*print report*/
	mdlstat();
}

void dispatch() {

	create("dispatch");

	//Loop until simulation is over, hold for the IATM of HP's, and 
	//Create the customer
	while(cnt >= 0) {
		hold(expntl(IATM_HP));
		cust();
	}
}

void cust()				/*process customer*/
{
	TIME t1;
	int i;
	double time_spent_waiting = 0.0;

	create("cust");				/*required create statement*/

	//note_entry(system_qtbl);
	for(i = 0; i < 8; i++) {
		t1 = clock;				/*time of request */
		//note_entry(q_array[i]);			/*note arrival */
		//use the facility
		use(fac_array[i], expntl(SVTM));
		//record(clock-t1, t_array[i]);			/*record response time*/
		time_spent_waiting += clock - t1;
		//note_exit(q_array[i]);			/*note departure */
	}
	record(time_spent_waiting, system_tbl);
	//note_exit(system_qtbl);


	cnt--;					/*decrement cnt*/
	if(cnt == 0)
		set(done);			/*if last arrival, signal*/
}

