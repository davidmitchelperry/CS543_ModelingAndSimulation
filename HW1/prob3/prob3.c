/* Original Heap structure found online at the following link:
 * 	www.cs.ucf.edu/~dmarino/ucf/cop3502/sampleprogs/heap.c
 * 
 * Heap Author: Arup Guha
 * Heap Code Date: 2/27/08
*/


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define SIZE 100

struct event {
	double time;
	char type;
};

struct heapStruct {

    struct event* heaparray;
    int capacity;
    int size;
};

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
double getAveragePeopleSize();
struct event createVacationEnd();

int getQueueSize();

float service_time_mean = 1;
float interarrival_time_mean = 1.1;
double seed;
float k;


#define nmax 2000 

int count;

struct event queue[nmax * 2];
int head = 0;
int tail = 0;
int queueEmpty = 1;

double peopleCount[nmax];
double myclock;

double averageWaitTime;

float vacation_time_mean; 

int main(int argc, char *argv[]) {

	if(argc != 3) {
		printf("usage: prob3 seed k\n");
		return 0;
	}

	// Get the seed and K value
	seed = (double)atoi(argv[1]);
	k = atoi(argv[2]);

	// Calculate vacation time mean
	vacation_time_mean = k * service_time_mean;

	// Initialize the array for counting lenght of time at certain size
	int i;
	for(i = 0; i < nmax; i++) {
		peopleCount[i] = 0;
	}

	// Calculate P value, initialize state vars
	float p = service_time_mean / interarrival_time_mean;
	count = 0;
	int busy = 0;
	int onVacation = 0;

	//init heap and schedule first arrival
	struct heapStruct *h = initHeap();
	struct event firstArrival = createArrival();

	insert(h, firstArrival);

	// While nmax customers have not been served
	while(count < nmax) {
		// Extract the event from the queue
		struct event e = removeMin(h);
		// Update people Count correctly
		if(busy) {
			peopleCount[getQueueSize() + 1] += (e.time - myclock);
		}
		else {
			peopleCount[getQueueSize()] += (e.time - myclock);
		}
		//update time	
		myclock = e.time;
		//on an arrival
		if(e.type == 'A') {
			// Create new arrival, and schedule it
			struct event newArrival = createArrival();
			newArrival.time += myclock;
			insert(h, newArrival);
			//if not busy or on vacation
			if(!busy && !onVacation) {
				//Create and schedule exit event
				struct event newExit = createExit();
				newExit.time += myclock;
				insert(h, newExit);
				busy = 1;
			}
			//if server is busy or on vacation
			else {
				// add customer to queue
				queue[tail] = e;
				queueEmpty = 0;
				tail++;
			}
				
		}
		// On exit event
		if(e.type == 'E') {

			// if queue is non empty
			if(!queueEmpty) {
				// figure out how long customer has waited
				averageWaitTime += (myclock - queue[head].time);
				// remove from the queue
				head++;
				if(head == tail) {
					queueEmpty = 1;
				}
				// Schedule exit event
				struct event newExit = createExit();
				newExit.time += myclock;
				insert(h, newExit);
				count++;
			}
			//if queue is empty
			else { 
			
				//update server state, coumt, and schedule the end of vacation
				busy = 0;
				count++;
				struct event newVacation = createVacationEnd();
				newVacation.time += myclock;
				insert(h, newVacation);
				onVacation = 1;

			}
		}
		// On the end of vacation
		if(e.type == 'V') {
			// If the queue is not empty
			if(!queueEmpty) {
				// figure out how long customer has waited
				averageWaitTime += (myclock - queue[head].time);
				// remove from the queue
				head++;
				if(head == tail) {
					queueEmpty = 1;
				}
			
				// Schedule next exit event
				struct event newExit = createExit();
				newExit.time += myclock;
				insert(h, newExit);
				count++;
			}
			//The queue is empty
			else {
				//update server state, schedule the end of vacation
				busy = 0;
				struct event newVacation = createVacationEnd();
				newVacation.time += myclock;
				insert(h, newVacation);
				onVacation = 1;
			}
		}
			
	}

	// Calculate average wait time
	averageWaitTime = averageWaitTime / nmax;

	//Print out results
	printf("%d customers were served\n", count);
	printf("%.5f customers in system on average\n", getAveragePeopleSize());
	printf("On average customers wait %.5f to be served\n", averageWaitTime);
	printf("last seed: %.5f\n", seed);
	printf("time to complete: %.5f\n", myclock);

	return 0;
}

int getQueueSize() {
	return tail - head;
}

double getAveragePeopleSize() {
	int i;
	double result = 0;
	for(i = 0; i < nmax; i++) {
		result += peopleCount[i] * i;
	}

	result = result / myclock;
	return result;
}

struct event createArrival() {
	struct event result;
	result.time = expon(&seed, interarrival_time_mean);
	result.type = 'A';
	return result;
}

struct event createVacationEnd() {
	struct event result;
	result.time = expon(&seed, vacation_time_mean);
	result.type = 'V';
	return result;
}

struct event createExit() {
	struct event result;
	result.time = expon(&seed, service_time_mean);
	result.type = 'E';
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
    h->capacity = SIZE;
    h->heaparray = (struct event*)malloc(sizeof(struct event)*(SIZE+1));
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
        
        // Copy the last value into this top slot.  h->heaparray[1] = h->heaparray[h->size]; 
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

// Runs a heap sort by creating a heap out of the values in the array, and then
// extracting those values one-by-one from the heap back into the array in the
// proper order.


