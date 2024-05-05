#include <stdio.h>
#include <stdlib.h>
#include "./time/tmeas.h"
#include <pthread.h>
#include <math.h>

#define test_errno(msg) do{if (errno) {perror(msg); exit(EXIT_FAILURE);}} while(0)

// number of intervals
int N = 1000;

#define NUM_THR 1000

// the integration left boundery point
int a = 0;
// the integration right boundery point
int b = 1;

//every yi will be stored here temporarily
double y = 0;

// mutex and conditions initialization
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t y_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t y_null = PTHREAD_COND_INITIALIZER;

//this function calculates, from the argument, the corresponding value of y
double fx(double x) {
	return 1 / 1 - pow(x, 2);
}

// new threads will execute this section of code
void thread(void* thr_data) {
	long int id = (long int) thr_data;
	int i;
	//h is the height of the trapezium
	double h = (b - a) / (double) N;

	//set the left boundery dependently by the process id
	int start = id * (N / NUM_THR) + 1;

	// to manage the not integer division of iteration the highest id will do much work then others
	// all threads but the on with the highest id does this section
	if (id != NUM_THR - 1) {
		for (i = start; i < start + (N / NUM_THR); i++) {
			//mutual exclusion
			pthread_mutex_lock(&mutex1);
			//until the main processes doesn't get the y null wait on y_null condition
			while (y != 0) {
				pthread_cond_wait(&y_null, &mutex1);
			}
			//calulate the new value of y
			y = fx(h * (double) i);
			//signal to the main thread
			pthread_cond_signal(&y_available);

			printf("id: %ld, iteration :%d, y= %lf\n", id, i, y);

			pthread_mutex_unlock(&mutex1);

		}
	} else
	//the thread with the highest id does this section
	{
		for (i = start; i < N; i++) {
			//mutual exclusion
			pthread_mutex_lock(&mutex1);
			//until the main processes doesn't get the y null wait on y_null condition
			while (y != 0) {
				pthread_cond_wait(&y_null, &mutex1);
			}
			//calulate the new value of y
			y = fx(h * i);
			//signal to the main thread
			pthread_cond_signal(&y_available);

			printf("PROCESS id: %ld, iteration :%d, PRODUCES y= %lf\n", id, i,y);
			pthread_mutex_unlock(&mutex1);

		}
	}
}

int main() {
	double h = (b - a) / (double) N;
	long int i;
	long int thr;
	//the accomulator of the sum of yi, will contain the result
	double I;

	pthread_t thread_array[NUM_THR];
	tstart();

	//generates all threads
	for (i = 0; i < NUM_THR; i++) {
		// 'pthread_create' returns 0 if there was an error
		thr = pthread_create(&thread_array[i], NULL, thread, (void *) i);
		if (thr) {
			fprintf( stderr, "Error: return code from pthread_create() - %ld",
					thr);
			exit(-1);
		}
	}

	//read y n-1 times
	for (i = 0; i < N - 1; i++) {
		//mutual exclusion
		pthread_mutex_lock(&mutex1);
		//until one of the threads doesn't get the new value of y wait on y_availalbe condition
		while (y == 0) {
			pthread_cond_wait(&y_available, &mutex1);
		}
		printf("MAIN PROCESS) iteration: %ld, Y READED:= %lf\n", i, y);
		//add the new y to the accumulator
		I += y;
		// set y to null
		y = 0;
		//signal to the next thread
		pthread_cond_signal(&y_null);
		pthread_mutex_unlock(&mutex1);
	}

// add f(a)+f(b)/2
	I += fx(h * a) / 2;
	I += fx(h * b) / 2;
// multiply for the height
	I *= h;
// multiply for 4 as required
	I *= 4;

	printf("\n\n ****MAIN PROCESS) RESULT OF THE COMPUTATION: %lf \n", I);

	//print the execution time
	printf("\n\n -----MAIN PROCESS) EXECUTION TIME: %lf \n", tstop());



	return 0;
}
