#include <stdio.h>
#include <math.h>

double uniform (double *dseed);
double expon(double *dseed, float xm);

int main() {

	double seed = 1;
	float expectedMean = 50.0;
	int n = 100000;
	double total = 0;
	int i;
	double result;

	// Generate random numbers and add them to total
	for(i = 0; i < n; i++) {
		total += expon(&seed, expectedMean);
	}

	// calculate the average
	result = total / n;

	// Print it out
	printf("Expected mean is: %.5f\n", expectedMean);
	printf("Observed mean is: %.5f\n", result);

	return 0;
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
      
