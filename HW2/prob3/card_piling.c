#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double evaluate(int n);
double uniform(double *dseed);
void display(int tournaments, int n);
void run();
void init_pop();

int gene[30][10];
int POP = 30;
int LEN = 10;
double MUT = .1;
double REC = .5;
double END = 100000;
double SUMTARG = 36;
double PRODTARG = 360;

//double seed = 1;
double seed = 123456;

int best;
double leastError = 100;
int bestTournNum = -1;

int main() {
	
	run();
	display(bestTournNum, best);
	return 0;
}

double uniform(double *dseed) {

   double d2p31m = 2147483647,
          d2p31  = 2147483711;

      *dseed = 16807*(*dseed) - floor(16807*(*dseed)/d2p31m) * d2p31m;
      return( fabs((*dseed / d2p31)) ); 
}

void run() {
	int a, b, Winner, Loser;
	init_pop();
	int tournamentNo;

	for(tournamentNo = 0; tournamentNo < END; tournamentNo++) {
		a = (int)(POP * uniform(&seed));
		b = (int)(POP * uniform(&seed));
		if(evaluate(a) < evaluate(b)) {
			Winner = a;
			Loser = b;
		}
		else {
			Winner = b;
			Loser = a;
		}

		int i;
		for(i = 0; i < LEN; i++) {
			if(uniform(&seed) < REC)
				gene[Loser][i] = gene[Winner][i];

			if(uniform(&seed) < MUT)
				gene[Loser][i] = 1 - gene[Loser][i];

			if(evaluate(Loser) <  leastError) {
				leastError = evaluate(Loser);
				best = Loser;
				bestTournNum = tournamentNo;
				//display(tournamentNo, Loser);
			}
			
		}
	}
}

void display(int tournaments, int n) {
	printf("\r\n==============================\r\n");
	printf("After %d tournaments, Solution sum pile (should be 36) cards are : \n", tournaments);
	int i;
	for(i = 0; i < LEN; i++) {
		if(gene[n][i] == 0) {
			printf("%d\n", i+1);
		}
	}

	printf("\r\nAnd product pile (should be 360) cards are : \n");

	for(i = 0; i < LEN; i++) {
		if(gene[n][i] == 1) {
			printf("%d\n", i + 1);

		}
	}
	printf("\n");

}

void init_pop() {
	int i, j;
	for(i = 0; i < POP; i++) {
		for(j = 0; j < LEN; j++) {
			if(uniform(&seed) < .5) {
				gene[i][j] = 0;
			}
			else {
				gene[i][j] = 1;
			}
		}
	}
}
	

double evaluate(int n) {
	
	int sum =0, prod = 1;
	double scaled_sum_error, scaled_prod_error, combined_error;
	int i;

	for(i = 0; i < LEN; i++) {
		if(gene[n][i] == 0) {
			sum += (1 + i);
		}
		else {
			prod *= (1 + i);
		}
	}

	scaled_sum_error = (sum - SUMTARG) / SUMTARG;
	scaled_prod_error = (prod - PRODTARG) / PRODTARG;
	combined_error = fabs(scaled_sum_error) + fabs(scaled_prod_error);
	//printf("%f\n", combined_error);

	//printf("sum error: %f, prod error: %f\n", scaled_sum_error, scaled_prod_error);
	//printf("combined : %f\n", combined_error);

	return combined_error;
}
