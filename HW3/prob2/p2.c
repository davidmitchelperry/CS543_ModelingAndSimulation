/* simulate an M/M/1 queue
  (an open queue with exponential service times and interarrival intervals)
*/

#include "csim.h"
#include <stdio.h>

#define NARS	10000	/*number of arrivals to be simulated*/

EVENT done;		/*pointer for counter */
int cnt;		/*number of active tasks*/

void init();
void production();
void order_dispatch();
void order();

int LP_READY = 1;

STORE stock;
FACILITY packingMahcine;
TABLE packingMachineTbl;
QTABLE packingMachineQtbl;
int dropped_orders = 0;


void sim(int argc, char* argv[])				/*1st process - named sim */
{
	int i;

	// use the first parameter of the program as the seed
	reseed(NIL, atoi(argv[1]));

	set_model_name("Problem 2");
	create("sim");				/*required create statement*/

	//initialize data structures
	init();

	production();
	order_dispatch();
	wait(done);				/*wait until all done*/


	printf("# of dropped orders: %d\n", dropped_orders);
	
	//report();				/*print report*/
	report_facilities();
	report_storages();
	//status_storages();
	//theory();				/*print theoretical res*/ 
	//mdlstat();
}

void order() {

	TIME t1;

	create("order");

	// Check to see if backlog is full
	if(storage_qlength(stock) < 10) {
	
		// Figure out how many widgets this order is requesting	
		float rand = uniform01();
		int widgetNum;
		if(rand <= .1) {
			widgetNum = 4;
		}
		else if(rand <= .5) {
			widgetNum = 5;
		}
		else if(rand <= .8) {
			widgetNum = 6;
		}
		else if(rand <= 1) {
			widgetNum = 7;
		}

		// Request the appropriate amount of widgets
		allocate(widgetNum, stock);

		// Use the packing machine for the correct
		// deterministic amount of time
		use(packingMahcine, 90 + (50 * widgetNum));

		// decrement the counter
		cnt--;

	}
	else {
		// Order is lost
		dropped_orders++;
	}
}

void order_dispatch() {

	create("order_dispatch");
	//While all orders are not completed hold the expntl amount of time and 
	//generate an order
	while(cnt > 0) {
		//printf("in order dispatch\n");
		hold(expntl(300));
		order();
	}
	//if you're here the simulation is over
	set(done);
}

void production() {
	
	create("production");
	//While simulation is not done hold for uniform random amount of time
	//add 1 widget to the stock, and deallocate (releases waiting orders)
	while(state(done) == NOT_OCC) {
		hold(uniform(60, 100));	
		add_store(1, stock);
		deallocate(0, stock);
	}


}

void init() {
	cnt = NARS;
	done = event("done");
	stock = storage("widget stock", 0);	
	packingMahcine = facility("packing machine");
	packingMachineTbl = table("resp tms for Packing Machine");
	packingMachineQtbl = qhistogram("num in Packing Machine queue", 101);
}

