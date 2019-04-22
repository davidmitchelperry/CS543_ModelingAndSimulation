/* simulate an M/M/1 queue
  (an open queue with exponential service times and interarrival intervals)
*/

#include "csim.h"
#include <stdio.h>

#define	SVTM	1.0	/*mean of service time distribution */
#define IATM_HP 	5.0		/*mean of inter-arrival time distribution */
#define IATM_LP 	2.0		/*mean of inter-arrival time distribution */
#define NARS	10000	/*number of arrivals to be simulated*/

EVENT done;		/*pointer for counter */
int cnt;		/*number of active tasks*/
void init();
void AtoBArrivals();
void BtoAArrivals();
void AtoBCar();
void BtoACar();
void traffic_controller();

FACILITY BtoALight;
FACILITY AtoBLight;

EVENT AtoBGreen;
int AtoBGreen_bool = 0;
EVENT BtoAGreen;
int BtoAGreen_bool = 0;
EVENT bothRed;

float AtoBCarsPerSec = 300.0;
float BtoACarsPerSec = 400.0;

double GREENAB;
double GREENBA;

void sim(int argc, char* argv[])				/*1st process - named sim */
{
	int i;

	set_model_name("Problem 3");
	create("sim");				/*required create statement*/


	init(); //Init data structures

	// Get Green Light times from command line arguments
	GREENAB = (double)atoi(argv[1]);
	GREENBA = (double)atoi(argv[2]);

	//Start the traffic controller
	traffic_controller();

	//Generate arrivals
	AtoBArrivals();
	BtoAArrivals();

	wait(done);

	report_facilities();
}

void init() {
	cnt = NARS;
	done = event("done");
	AtoBGreen = event("AtoBGreen");
	BtoAGreen = event("BtoAGreen");
	bothRed = event("bothRed");
	AtoBLight = facility("AtoBLight");
	BtoALight = facility("BtoALight");
}

void traffic_controller() {
	create("traffic_controller");
	// Loop until simulation is over
	while(cnt > 0) {

		//Set one light to green and hold
		set(AtoBGreen);
		hold(GREENAB);
		//Unset light
		clear(AtoBGreen);
		AtoBGreen_bool = 0;

		//hold for 55 secs (BOTH RED)
		hold(55);
	
		//Signal other green light and hold	
		set(BtoAGreen);
		hold(GREENBA);
		//Unset the light
		clear(BtoAGreen);
		BtoAGreen_bool = 0;

		hold(55);
	}
}

void AtoBArrivals() {

	create("AtoBArrivals");
	//Loop until the simulation is over, hold for secs/car, and generate car
	while(cnt > 0) {
		hold(poisson((60 * 60)/AtoBCarsPerSec));
		AtoBCar(); 
	}
	set(done);
	
}

void AtoBCar() {

	create("AtoBCar");

	// Attempt to reserve the light
	reserve(AtoBLight);
		// If you are the first car to pass set bool so other cars can pass too
		if(!AtoBGreen_bool) {
			wait(AtoBGreen);
			AtoBGreen_bool = 1;
		}
		//hold for 2 seconds (cars pass every 2 secs)
		hold(2);
	//let the light go
	release(AtoBLight);

	//decrease cnt
	cnt--;
}

void BtoAArrivals() {

	create("BtoAArrivals");
	//Loop until the simulation is over, hold for secs/car, and generate car
	while(cnt > 0) {
		//printf("making BtoA car\n");
		hold(poisson((60 * 60)/BtoACarsPerSec));
		BtoACar();
	}
	set(done);
}

void BtoACar() {

	create("BtoACar");

	// Attempt to reserve the light
	reserve(BtoALight);
		// If you are the first car to pass set bool so other cars can pass too
		if(!BtoAGreen_bool) {
			wait(BtoAGreen);
			BtoAGreen_bool = 1;
		}
		//hold for 2 seconds (cars pass every 2 secs)
		hold(2);	
	//Let the light go
	release(BtoALight);

	//decrease cnt
	cnt--;
}
