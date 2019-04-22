
//Function returns a single U(0,1) variate
// Ignore compilation warnings

#include <stdio.h>
#include <math.h>

#define n 400
#define m 10

double uniform(double *dseed);

int main() {

	double seed = 1234567;
	double randNums[n];
	double binWidth;
	int bins[m];
	int i,j;
	double expectedBinCt = n / m;
	double Y;

	binWidth = ((double)1)/((double)m);

	// Generate random numbers
	for(i = 0; i < n; i++) {
		randNums[i] = uniform(&seed);
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

	// Initialize Y to zero
	Y = 0;

	// Perform series calculation and store in Y
	for(i = 0; i < m; i++) {
		Y += ((pow((bins[i] - expectedBinCt), 2)) / expectedBinCt);
	}

	// Print out Y value
	printf("Y val is: %.5f\n", Y);

	if(Y > 16.919) 
		printf("reject the hypothesis\n");
	else
		printf("do not reject the hypothesis\n");
		
	return 0;

}


double uniform(double *dseed) {


   double d2p31m = 2147483647,
          d2p31  = 2147483711;

      *dseed = 16807*(*dseed) - floor(16807*(*dseed)/d2p31m) * d2p31m;
      return( fabs((*dseed / d2p31)) ); 
}
      


