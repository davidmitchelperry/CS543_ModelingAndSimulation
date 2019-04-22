#include <iostream>
#include <pthread.h>
#include <algorithm>
#include <map>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

using namespace std;

pthread_mutex_t *cal_lock;
pthread_mutex_t *clock_lock;

class ThreadControlBlock {
	
	public:
		pthread_t id;
		pthread_cond_t *cond;
		bool is_running;

		ThreadControlBlock(pthread_t i, pthread_cond_t* c) {
			this->id = i;
			this->cond = new pthread_cond_t;
			memcpy(this->cond, c, sizeof(pthread_cond_t));
			is_running = 0;
		}
};


map<pthread_t, ThreadControlBlock*> thread_map;
pthread_mutex_t *global_sim_mutex;

void printThreadMap() {
	map<pthread_t, ThreadControlBlock*>::iterator it;
	printf("thread map conds:");
	for(it = thread_map.begin(); it != thread_map.end(); it++) {
		printf("%d\n", it->second->cond);
	}
}


class ActivationRecord {
	public:
		double time;
		ThreadControlBlock *tcb;

		ActivationRecord(double t, ThreadControlBlock* tc) {
			this->time = t;
			//this->tcb = tc;
			this->tcb = new ThreadControlBlock(*(new pthread_t), new pthread_cond_t);
			memcpy(this->tcb, tc, sizeof(ThreadControlBlock));
		}
};

bool minHeapCmp(ActivationRecord *a, ActivationRecord *b)
{
	if((a->time < b->time) == false) {
		return true;
	}
	if((b->time < a->time) == true) {
		return false;
	}
}

class MyHeap {
	public:
		vector<ActivationRecord*> ar_vector;

		MyHeap() {
			make_heap(ar_vector.begin(), ar_vector.end(), minHeapCmp);
		}

		void insert(ActivationRecord *a) {
			ar_vector.push_back(a);
			push_heap(ar_vector.begin(), ar_vector.end(), minHeapCmp);
		}

		ActivationRecord* deleteMin() {
			ActivationRecord *minResult = (ar_vector.front());
			pop_heap(ar_vector.begin(), ar_vector.end(), minHeapCmp);
			ar_vector.pop_back();
			return minResult;	
		}
		int size() {
			return ar_vector.size();
		}
		void print() {
			vector<ActivationRecord*>::iterator it;
			printf("cond variables being wated on and time\n");
			for(it = ar_vector.begin(); it != ar_vector.end(); it++) {
				printf("cond: %d, time: %f\n", (*it)->tcb->cond, (*it)->time);
			}
		}

};

MyHeap *global_calendar;
double *global_clock;

class Asim {
	public:
		double clock;
		MyHeap calendar;
		pthread_mutex_t exe_mutex;
		pthread_cond_t sim_cond;
		pthread_t sim_id;
		ThreadControlBlock *sim_tcb;

		Asim(MyHeap h) {
			global_clock = &clock;
			cal_lock = new pthread_mutex_t;
			pthread_mutex_init(cal_lock, NULL); 

			clock_lock = new pthread_mutex_t;
			pthread_mutex_init(clock_lock, NULL);
			//Get the main sim thread id
			sim_id = pthread_self();

			//Create the exe mutex
			exe_mutex = *(new pthread_mutex_t);
			global_sim_mutex = &exe_mutex;
			pthread_mutex_init(&exe_mutex, NULL);

			// Create sim's condition variable
			sim_cond = *(new pthread_cond_t);
			pthread_cond_init(&sim_cond, NULL);

			// Create TCB for simulation process
			sim_tcb = (new ThreadControlBlock(sim_id, &sim_cond));

			// Lock the execution mutex
			pthread_mutex_lock(&(exe_mutex));

			// Insert TCB into thread map
			thread_map.insert(pair<pthread_t, ThreadControlBlock*>(sim_id, sim_tcb));

			// Set clock and calendar
			clock = 0.0;
			calendar = h;
			global_calendar = &calendar;
		}

		void create(void (*func)()) {

			// Create the dispatch thread
			pthread_t *thread = new pthread_t;
		
			// Create the cond variable for new process
			pthread_cond_t *cond = new pthread_cond_t;
			pthread_cond_init(cond, NULL);

			// Create the thread
			pthread_create(thread, NULL, (&Asim::dispatch_func), (void*)func);
			//printf("after pthread_create\n");

			// Create the ThreadControlBlock	
			ThreadControlBlock *tcb = new ThreadControlBlock(*thread, cond);
		
			// Create Activation Record
			pthread_mutex_lock(clock_lock);
			ActivationRecord *ar = new ActivationRecord(clock, tcb);
			pthread_mutex_unlock(clock_lock);
		
			// Insert ActivationRecord into calendar
			pthread_mutex_lock(cal_lock);
			this->calendar.insert(ar);
			pthread_mutex_unlock(cal_lock);

			thread_map.insert(std::pair<pthread_t, ThreadControlBlock*>(*thread, tcb));
						
		}

		void hold(int time) {
			pthread_mutex_lock(cal_lock);
			if(this->calendar.size() != 0) {
				ThreadControlBlock *my_tcb = thread_map.find(pthread_self())->second;
				pthread_mutex_lock(clock_lock);
				ActivationRecord *my_ar = new ActivationRecord(clock + time, my_tcb);
				pthread_mutex_unlock(clock_lock);

				this->calendar.insert(my_ar);

				ActivationRecord *ar = this->calendar.deleteMin();
				pthread_mutex_unlock(cal_lock);
				ThreadControlBlock *tcb = ar->tcb; 

				pthread_mutex_lock(clock_lock);
				this->clock = ar->time;
				pthread_mutex_unlock(clock_lock);
				typedef map<pthread_t, ThreadControlBlock*>::iterator it_type;
				for(it_type iterator = thread_map.begin(); iterator != thread_map.end(); iterator++) {
					iterator->second->is_running = 1;
				}
				if(pthread_equal(ar->tcb->id, pthread_self()) != 0) {
				}
				else {
					pthread_cond_signal(tcb->cond);
					pthread_cond_wait(my_tcb->cond, &exe_mutex);
				}
				pthread_mutex_lock(clock_lock);
				this->clock = my_ar->time;
				pthread_mutex_unlock(clock_lock);

			}
			else {
				printf("cal size is 0\n");
				pthread_mutex_unlock(cal_lock);
				pthread_mutex_lock(clock_lock);
				this->clock += time;
				pthread_mutex_unlock(clock_lock);
			}
		}

		 static void *dispatch_func(void* func) {


			ThreadControlBlock *tcb = (thread_map.find(pthread_self())->second);

			pthread_mutex_lock(global_sim_mutex);
			((void(*)(void))func)();

			thread_map.erase(pthread_self());
	
			pthread_mutex_lock(cal_lock);
			printf("func finished running \n");
			if(global_calendar->size() == 0) {
				printf("in if\n");
				pthread_mutex_unlock(cal_lock);
				pthread_mutex_unlock((global_sim_mutex));
			}
			else {
				printf("in else\n");
				ActivationRecord *ar = global_calendar->deleteMin();
				printf("after delete\n");
				pthread_mutex_unlock(cal_lock);
				printf("after unlock\n");
				pthread_mutex_lock(clock_lock);
				printf("after lock");
				*global_clock = ar->time;
				pthread_mutex_unlock(clock_lock);
				ar->tcb->is_running = 1;
				pthread_mutex_unlock(global_sim_mutex);
				pthread_cond_signal(thread_map.find(ar->tcb->id)->second->cond);
			}
			printf("past conditions\n");
		}
};

Asim *a;

class Event {
	public:
		pthread_cond_t* cond;
		int occurred;
		int count;
		pthread_mutex_t *occurred_lock;

		Event() {
			cond = new pthread_cond_t;
			occurred = 0;
			count = 0;
			occurred_lock = new pthread_mutex_t;
			pthread_mutex_init(occurred_lock, NULL);
			pthread_cond_init(cond, NULL);
		}

		void wait() {
		
			pthread_mutex_lock(occurred_lock);
			count++;
			
			pthread_mutex_lock(cal_lock);
			if(occurred == 1) {
				pthread_mutex_unlock(cal_lock);
				occurred = 0;
				count--;
				pthread_mutex_unlock(occurred_lock);
			}
			else if(global_calendar->size() == 0) {
				pthread_mutex_unlock(cal_lock);
				pthread_mutex_unlock(occurred_lock);
				pthread_cond_wait(cond, global_sim_mutex);
				pthread_mutex_lock(occurred_lock);
				count--;
				pthread_mutex_unlock(occurred_lock);
			}
			else {
				pthread_mutex_unlock(occurred_lock);
				ActivationRecord* ar = global_calendar->deleteMin();
				pthread_mutex_unlock(cal_lock);
				pthread_cond_signal(ar->tcb->cond);

				pthread_cond_wait(cond, global_sim_mutex);
				pthread_mutex_lock(occurred_lock);
				count--;
				pthread_mutex_unlock(occurred_lock);
				
			}
				
		}

		void set() {
			pthread_mutex_lock(occurred_lock);
			if(count == 0) {
				occurred = 1;
				pthread_mutex_unlock(occurred_lock);
			}
			else {
				pthread_mutex_unlock(occurred_lock);
				pthread_mutex_unlock(global_sim_mutex);
				pthread_cond_signal(cond);
			}
		}
		void clear() {
			
			pthread_mutex_lock(occurred_lock);
			occurred = 0;
			pthread_mutex_unlock(occurred_lock);
		}

};

vector<pthread_t> *vptr;
pthread_mutex_t *mutptr;
int *bptr;
class Facility {
	public:
		pthread_mutex_t *mutex;
		vector<pthread_t> queue;
		int busy;

		Facility() {
			mutex = new pthread_mutex_t;
			pthread_mutex_init(mutex, NULL);
			busy = 0;
			vptr = &queue;
			mutptr = mutex;
			bptr = &busy;
		}
			
		void reserve() {
			pthread_mutex_lock(mutptr);
			if((*bptr)) {
				vptr->push_back(pthread_self());
				pthread_mutex_unlock(mutptr);
				pthread_mutex_lock(cal_lock);
				if(global_calendar->size() != 0) {
					ActivationRecord* ar = global_calendar->deleteMin();
					pthread_cond_signal(ar->tcb->cond);

				}
				pthread_mutex_unlock(cal_lock);
				pthread_cond_wait(thread_map.find(pthread_self())->second->cond, global_sim_mutex);
			}
			else {
				pthread_mutex_unlock(cal_lock);
				*bptr = 1;
				pthread_mutex_unlock(mutptr);
			}
		}

		void release() {
			pthread_mutex_lock(mutptr);
			if(vptr->size() != 0) {
				pthread_t t = (*vptr)[0];
			vptr->erase(vptr->begin());
				pthread_mutex_unlock(global_sim_mutex);
				pthread_mutex_lock(global_sim_mutex);
				pthread_mutex_lock(clock_lock);
				ActivationRecord* ar = new ActivationRecord(*global_clock, thread_map.find(t)->second);
				pthread_mutex_unlock(clock_lock);
				pthread_mutex_lock(cal_lock);
				global_calendar->insert(ar);
				pthread_mutex_unlock(cal_lock);
			}
			else {
				*bptr = 0;
			}
			pthread_mutex_unlock(mutptr);
		}
};

/* Start of user written simulation code */

double uniform(double *dseed) {


   double d2p31m = 2147483647,
          d2p31  = 2147483711;

      *dseed = 16807*(*dseed) - floor(16807*(*dseed)/d2p31m) * d2p31m;
      return( fabs((*dseed / d2p31)) ); 
}
      
double expon(double *dseed, float xm) {

      return( (-(xm) * log((double)uniform(dseed))) );
}
	
Event *e;
Facility* f;

Asim *s;
MyHeap *h;
int cnt = 1000;
double myseed = 1;

void cust() {
	printf("in cust\n");
	f->reserve();
		printf("cust reserved\n");
		s->hold(expon(&myseed, 1));
		printf("cust after hold\n");
	f->release();
		printf("cust after release\n");
	cnt--;
		printf("updated cnt: %d\n", cnt);
	if(cnt == 0) {
		printf("set the event\n");
		e->set();
	}
}

void sim() {
	h = new MyHeap();
	s = new Asim(*h);
	f = new Facility();
	e = new Event();

	while(cnt > 0) {
		printf("before sim create\n");
		s->create(&cust);
		printf("before sim hold\n");
		s->hold(expon(&myseed, 5));
	}

	printf("before sim wait\n");
	e->wait();
	printf("after sim wait\n");
	printf("time: %f\n", *global_clock);
}
	
int main() {

	sim();

	return 0;
}

/* End of user written simulation code */
