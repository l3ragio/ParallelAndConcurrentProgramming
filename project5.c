#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define N 1000
#include<time.h>
void sostituzioneIndietro(int* A, int* b, int* c, int n) {

}

int main(void) {
	int j, k, n;
	double s, start;
	omp_set_num_threads(4);

	n = 6;
	//b is the "the constant term" vector
	double b[n];
	//x vector will store the unknowns values
	double x[n];
	//A is the coefficient matrix
	double A[n][n];

//random filling
	for (j = 0; j < n; j++) {
		//use sleep to get "more random" numbers
		sleep(1);
		srand(time(NULL) * j);
		//initialize b
		b[j] = rand() % 10 + 1;
		//initialize x
		x[j] = 0;
		//initialize A
		for (k = 0; k < n; k++) {
			srand(time(NULL) * k);
			A[j][k] = (k <= j) ? rand() % 10 + 1 : 0;
		}
	}

	printf("print the A | b matrix:\n");
	for (k = 0; k < n; k++) {
		for (j = 0; j < n; j++) {
			printf("[%d][%d]: %d  ", k, j,(int) A[k][j]);
		}
		printf("[%d] : %d\n", k, (int)b[k]);
	}

	start = omp_get_wtime();

//forward substitution without omp: (it works by lines)
	for (k = 0; k < n; k++) {
		// s will store temporarily the b[k] value
		s = b[k];
		// then subtract the coefficiet multiplicated with the value of the proper unknow
		for (j = 0; j < k; j++) {
			s -= A[k][j] * x[j];
		}
		//finallx divide s with the proper coefficient and assign it to the proper x
		x[k] = s / (double) A[k][k];
	}

	printf("\nexecution time without omp: %lf \n", omp_get_wtime() - start);

	printf("print the X vector calculated without omp: \n");
	for (j = 0; j < n; j++) {

		printf("%d elemento : %lf \n", j, x[j]);
	}
	printf("\n\n");



	start = omp_get_wtime();
	//forward substitution with omp: (it works by columns)
	//first: fill the x vector with division between the well-known term, and its x coefficient
	for (k = 0; k < n; k++) {
			x[k] = b[k] / A[k][k];
		}
	//the first x has been calculated yet, so k have to start from 1 (k = column index)
	for (k = 1; k < n; k++) {
#pragma omp parallel for  shared(A,x) private(j)
		//(j = line index)
		for (j = k; j < n; j++) {
			//x[j] contains, iteration by iteration, a more complete partial value
			x[j] -= A[j][k - 1] * x[k - 1] / A[j][j];
		}
	}

	printf("execution time with omp: %lf \n", omp_get_wtime() - start);

	printf("print the X vector calculated with omp: \n");
	for (j = 0; j < n; j++) {
		printf("%d elemento : %lf \n", j, x[j]);
	}

	return 0;
}

