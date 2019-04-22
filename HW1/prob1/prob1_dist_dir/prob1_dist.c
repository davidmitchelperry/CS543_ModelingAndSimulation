
//Function returns a single U(0,1) variate
// Ignore compilation warnings

#include <stdio.h>
#include <math.h>

#define n 10000 
#define m 200 

double uniform(double *dseed);
double expon(double *dseed, float xm);

int main() {

	double seed = 1234567;
	double randNums[n];
	double binWidth;
	int bins[m];
	int i,j;
	double expectedBinCt = n / m;
	double Y;
	float mean = 10;

	binWidth = .1;

	// Generate random numbers
	for(i = 0; i < n; i++) {
		randNums[i] = expon(&seed, mean);
	}

	// Initialize bins to 0
	for(i = 0; i < m; i++) {
		bins[i] = 0;
	}
	
	// Count how many numbers land in each bin
	for(i = 0; i < n; i++) {
		for(j = 0; j < m; j++) {
			if(randNums[i] < ((j+1) * binWidth)) {
				bins[j] += 1;
				break;
			}
		}
	}

	// Calculate the observed probability 
	for(i = 0; i < m; i++) {
		randNums[i] = (float)bins[i] / (float)n;
	}

	//print them out
	for(i = 0; i < m; i++) {
		printf("%.10f\n", randNums[i]);
	}

		
	return 0;

}


double uniform(double *dseed) {


   double d2p31m = 2147483647,
          d2p31  = 2147483711;

      *dseed = 16807*(*dseed) - floor(16807*(*dseed)/d2p31m) * d2p31m;
      return( fabs((*dseed / d2p31)) ); 
}
      
double expon(double *dseed, float xm) {

      return( (-(xm) * log((double)uniform(dseed))) );
}


