/* Original Heap structure found online at the following link:
 * 	www.cs.ucf.edu/~dmarino/ucf/cop3502/sampleprogs/heap.c
 * 
 * Heap Author: Arup Guha
 * Heap Code Date: 2/27/08
*/

/* Original Queue Structure found online at the follwoing link:
 * 	http://www3.cs.stonybrook.edu/~skiena/392/programs/queue.c
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define nmax 2000
#define N 8

struct event {
	double time;
	char type;
	int currentSys;
	int custNumber;
	double orig_arrival_time;
};

struct heapStruct {

    struct event* heaparray;
    int capacity;
    int size;
};

typedef struct {
        struct event q[(nmax * 100)+1];		/* body of queue */
        int first;                      /* position of first element */
        int last;                       /* position of last element */
        int count;                      /* number of queue elements */
} Queue;

double uniform (double *dseed);
double expon(double *dseed, float xm);
struct event createArrival();
struct event createExit();

struct heapStruct *initHeap();
void heapify(struct heapStruct *h);
void percolateDown(struct heapStruct *h, int index);
void percolateUp(struct heapStruct *h, int index);
void insert(struct heapStruct *h, struct event);
struct event removeMin(struct heapStruct *h);
void printHeap(struct heapStruct *h);
void swap(struct heapStruct *h, int index1, int index2);
int minimum(int a, int indexa, int b, int indexb);
void freeHeap(struct heapStruct *h);

//Queue functions
void init_queue(Queue *q);
void enqueue(Queue *q, struct event x);
struct event dequeue(Queue *q);
int empty(Queue *q);

double getAveragePeopleSize();

float service_time_mean = 1.0;
float interarrival_time_mean = 1.5;
double seed = 234567;

Queue queueArray[N];
int serverBusyArray[N];

int count;

float peopleCount[nmax * 2];
double myclock;

float averageSystemTime = 0.0;

int global_custNumber = 0;


int main() {

	int i;

	// Init queue's and server status
	for(i = 0; i < N; i++) {
		init_queue(&queueArray[i]);
		serverBusyArray[i] = 0;
	}

	//initialize the queue
	Queue myqueue;	
	init_queue(&myqueue);

	//initialize the time counter for each possible number of people
	for(i = 0; i < nmax; i++) {
		peopleCount[i] = 0;
	}

	//calculate P
	float p = service_time_mean / interarrival_time_mean;
	count = 0;
	int busy = 0;

	// Initialize heap, and schedule first arrival
	struct heapStruct *h = initHeap();
	global_custNumber++;
	struct event firstArrival = createArrival();
	insert(h, firstArrival);

	clock_t begin, end;
	double time_spent;

	begin = clock();
	// While nmax customers have not been served
	while(count < nmax) {
		//printHeap(h);
		// Extract event from heap
		heapify(h);
		struct event e = removeMin(h);
		// Appropriately find how many people are in the system
		int tempPeopleInSystem = 0;
		for(i =0; i < N; i++) {
			tempPeopleInSystem += queueArray[i].count;
			if(serverBusyArray[i] == 1) {
				tempPeopleInSystem += 1;
			}

		}
		//If an arrival at system 0 is occuring
		if(e.type == 'A' && e.currentSys != 0) {
			tempPeopleInSystem += 1;
		}
		//printf("People in System: %d\n", tempPeopleInSystem);
		//printf("\n");
		peopleCount[tempPeopleInSystem] += (e.time - myclock);
		
		// update clock	
		myclock = e.time;
		// on arrival event
		if(e.type == 'A') {
			e.orig_arrival_time = e.time;
			//printf("customer %d arrives at system: %d, at time: %f\n", e.custNumber, e.currentSys, myclock);
			// if arrival is in first system schedule a new arrival
			if(e.currentSys == 0) {
				global_custNumber++;
				struct event newArrival = createArrival();
				newArrival.time += myclock;
				insert(h, newArrival);
			}
			// If server is not busy
			if(!serverBusyArray[e.currentSys]) {
				//printf("server: %d is not busy, cust %d gets served at time %f\n", e.currentSys, e.custNumber, myclock);
				// Schedule exit event, set server to busy
				struct event newExit = createExit();
				newExit.currentSys = e.currentSys;
				newExit.time += myclock;
				newExit.custNumber = e.custNumber;
				newExit.orig_arrival_time = e.orig_arrival_time;
				insert(h, newExit);
				serverBusyArray[e.currentSys] = 1;
			}
			else {
				// Put customer in queue
				enqueue(&queueArray[e.currentSys], e);
			}
				
		}
		// On exit event
		if(e.type == 'E') {
			averageSystemTime += (e.time - e.orig_arrival_time);
			//printf("customer %d exits at system: %d at time: %f\n", e.custNumber, e.currentSys, myclock);
			// if queue is not empty
			if(!empty(&queueArray[e.currentSys])) {
				//printf("queue not empty\n");
				// Extract customer from queue
				struct event oldArrival = dequeue(&queueArray[e.currentSys]);
				
				// Schedule the exit event
				struct event newExit = createExit();
				newExit.currentSys = e.currentSys;
				newExit.time += myclock;
				newExit.custNumber = oldArrival.custNumber;
				newExit.orig_arrival_time = e.orig_arrival_time;
				newExit.orig_arrival_time = e.orig_arrival_time;
				insert(h, newExit);


				// if not at the last queue, schedule arrival at next queue
				if(e.currentSys != (N-1)) {
					struct event arriveAtNextQueue = createArrival();
					arriveAtNextQueue.custNumber = e.custNumber;
					arriveAtNextQueue.currentSys = e.currentSys + 1;
					arriveAtNextQueue.time = e.time;
					insert(h, arriveAtNextQueue);
				}
				// if at last queue, update served counter
				else {
					//printf("customer %d leaves entire system at time %f\n", e.custNumber, myclock);
					count++;
				}
				
			}
			// Queue ie empty
			else {
				//printf("queue empty\n");
				// Server is no longer busy
				serverBusyArray[e.currentSys] = 0;
				//if not at last queue, schedule next arrival
				if(e.currentSys != (N-1)) {
					struct event arriveAtNextQueue = createArrival();
					arriveAtNextQueue.currentSys = e.currentSys + 1;
					arriveAtNextQueue.time = e.time;
					arriveAtNextQueue.custNumber = e.custNumber;
					insert(h, arriveAtNextQueue);
				}
				// else, update served counter
				else {
					count++;
				}
			}
		}
	}

	// get average wait time
	averageSystemTime = averageSystemTime / nmax;

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Simulation of %d customers with %d queues took %.50f\n", nmax, N, time_spent);
	printf("With average wait time: %f\n", averageSystemTime);

	return 0;
}


//calculate average
double getAveragePeopleSize() {
	int i;
	double result = 0;
	for(i = 0; i < (nmax * 2); i++) {
		result += peopleCount[i] * i;
	}

	result = result / myclock;
	return result;
}

struct event createArrival() {
	struct event result;
	result.time = expon(&seed, interarrival_time_mean);
	result.type = 'A';
	result.currentSys = 0;
	result.custNumber = global_custNumber;
	return result;
}

struct event createExit() {
	struct event result;
	result.time = expon(&seed, service_time_mean);
	result.type = 'E';
	result.currentSys = 0;
	return result;
}

double expon(double *dseed, float xm) {

      return( (-(xm) * log((double)uniform(dseed))) );
}

double uniform(double *dseed) {


   double d2p31m = 2147483647,
          d2p31  = 2147483711;

      *dseed = 16807*(*dseed) - floor(16807*(*dseed)/d2p31m) * d2p31m;
      return( fabs((*dseed / d2p31)) ); 
}

// Initialize an empty heap with a capacity of SIZE.
struct heapStruct* initHeap() {

    struct heapStruct* h;
    
    // Allocate space for the heap and set the size for an empty heap.
    h = (struct heapStruct*)(malloc(sizeof(struct heapStruct)));
    h->capacity = (nmax * N * 2);
    h->heaparray = (struct event*)malloc(sizeof(struct event)*((nmax * N * 2) + 1));
    h->size = 0;
    return h;
}

// Frees the struct pointed to by h.
void freeHeap(struct heapStruct *h) {
     free(h->heaparray);     
     free(h);
}

// Initializes the heap using the first length number of items in the array
// values.

// h points to a heap structure that has values inside of it, but isn't yet
// organized into a heap and does exactly that.
void heapify(struct heapStruct *h) {
     
    int i;
     
    // We form a heap by just running percolateDown on the first half of the 
    // elements, in reverse order.
    for (i=h->size/2; i>0; i--) 
        percolateDown(h, i);
    
}

// Runs percolate down on the heap pointed to by h on the node stored in index.
void percolateDown(struct heapStruct *h, int index) {

    int min;
    
    // Only try to percolate down internal nodes.
    if ((2*index+1) <= h->size) {
                    
        // Find the minimum value of the two children of this node.            
        min = minimum(h->heaparray[2*index].time, 2*index, h->heaparray[2*index+1].time, 2*index+1);
        
      // If this value is less than the current value, then we need to move
      // our current value down the heap.  
        if (h->heaparray[index].time > h->heaparray[min].time) {
            swap(h, index, min);
        
            // This part is recursive and allows us to continue percolating
            // down the element in question.
            percolateDown(h, min);
        }
    }
    
    // Case where our current element has exactly one child, a left child.
    else if (h->size == 2*index) {
         
        // Here we only compare the current item to its only child.
        // Clearly, no recursive call is needed since the child of this node
        // is a leaf. 
        if (h->heaparray[index].time > h->heaparray[2*index].time) 
            swap(h, index, 2*index);
    }
  }

// Runs percolate up on the heap pointed to by h on the node stored in index.
void percolateUp(struct heapStruct *h, int index) {

    // Can only percolate up if the node isn't the root.
    if (index > 1) {
              
        // See if our current node is smaller in value than its parent.        
        if (h->heaparray[index/2].time > h->heaparray[index].time) {
            
            // Move our node up one level.
            swap(h, index, index/2);
            
            // See if it needs to be done again.
            percolateUp(h, index/2);
        }
    }
}

// Inserts value into the heap pointed to by h.
void insert(struct heapStruct *h, struct event value) {

    struct event* temp;
    struct event* throwaway;
    int i;
    
    // Our array is full, we need to allocate some new space!
    if (h->size == h->capacity) {
                
        // We will double the size of the structure.
        h->capacity *= 2;
      
        // Allocate new space for an array.
        temp = (struct event*)malloc(sizeof(struct event)*h->capacity+1);
      
        // Copy all the items over.
        for (i=1; i<=h->capacity; i++)
            temp[i] = h->heaparray[i];
            
        // Move the pointer and free the old memory.
        throwaway = h->heaparray;
        h->heaparray = temp;
        free(throwaway);
    }
      
    // Adjust all the necessary components of h, and then move the inserted
    // item into its appropriate location.  
    h->size++;
    h->heaparray[h->size] = value;
    percolateUp(h, h->size);
}

struct event removeMin(struct heapStruct *h) {

    struct event retval;
    
    // We can only remove an element, if one exists in the heap!
    if (h->size > 0) {

        // This is where the minimum is stored.
        retval = h->heaparray[1];
        
        // Copy the last value into this top slot.
        h->heaparray[1] = h->heaparray[h->size];
        
        // Our heap will have one fewer items.
        h->size--;
        
        // Need to let this value move down to its rightful spot in the heap.
        percolateDown(h, 1);
        
        // Now we can return our value.
        return retval;
    }
    
    // No value to return, indicate failure with a -1.
}

// For debugging purposes, lets us see what's in the heap.  
void printHeap(struct heapStruct *h) {
    int i;
    
    for (i=1; i<=h->size; i++)
        printf("Event Type: %c, Event Time: %.5f\n", h->heaparray[i].type, h->heaparray[i].time);
    printf("\n");
}

// Swaps the values stored in the heap pointed to by h in index1 and index2.
void swap(struct heapStruct *h, int index1, int index2) {
    struct event temp = h->heaparray[index1];
    h->heaparray[index1] = h->heaparray[index2];
    h->heaparray[index2] = temp;
}

// Returns indexa if a < b, and returns indexb otherwise.
int minimum(int a, int indexa, int b, int indexb) {

    // Return the value associated with a.    
    if (a < b)
        return indexa;
        
    // Return the value associated with b.
    else
        return indexb;
}

void init_queue(Queue *q)
{
        q->first = 0;
        q->last = (nmax * 100)-1;
        q->count = 0;
}

void enqueue(Queue *q, struct event x)
{
        if (q->count >= (nmax * 100))
		printf("Warning: queue overflow enqueue \n");
        else {
                q->last = (q->last+1) % (nmax * 100);
                q->q[ q->last ] = x;    
                q->count = q->count + 1;
        }
}

struct event dequeue(Queue *q)
{
        struct event x;

        if (q->count <= 0) printf("Warning: empty queue dequeue.\n");
        else {
                x = q->q[ q->first ];
                q->first = (q->first+1) % (nmax * 100);
                q->count = q->count - 1;
        }

        return(x);
}

int empty(Queue *q)
{
        if (q->count <= 0) return 1;
        else return 0;
}



