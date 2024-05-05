#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int main() {

	int i;
	int n = 100;
	int V[n];

	//fill the array with the numbers from 0 to 99
	for (i = 1; i < n; i++) {
		V[i] = i;
	}
	//V[0] will contains the actual prime number
	V[0] = 2;
	printf("elemento \n");


//******************************************************IT DOESN'T USE OMP


//if the array doesn't store any other multiples of the current prime number(pn*) the while finish
	while (V[0]*2<n) {
		//since the first multiple of the current pn,insert 0 in the array jumping pn positions for iteration
		for (i = V[0] * 2 ; i < n; i = i + V[0]) {
			V[i] = 0;
		}
		//set the index to point to the next generic number
		i = V[0]+1;
		//when the while finishes i is pointing to the next pn
		while (V[i] == 0) {
			i++;
		}
		//assign the new pn at V[0]
		V[0] = V[i];
	}

	for (i = 1; i < n; i++) {
		printf("elemento %d : %d\n",i,V[i]);
	}
	//************************************************************************
	printf("\n\n\n");
	for (i = 1; i < n; i++) {
		V[i] = i;
	}

	V[0] = 2;

	//---------------------------------------------- IT USE OMP

	//if the array doesn't store any other multiples of the current prime number(pn*) the while finish
		while (V[0]*2<n) {
			//since the first multiple of the current pn,insert 0 in the array jumping pn positions for iteration
#pragma omp parallel for shared(V) private(i)
			for (i = V[0] * 2 ; i < n; i = i + V[0]) {
				V[i] = 0;
			}
			//set the index to point to the next generic number
			i = V[0]+1;
			//when the while finishes i is pointing to the next pn
			while (V[i] == 0) {
				i++;
			}
			//assign the new pn at V[0]
			V[0] = V[i];
		}
//-------------------------------------------------
	for (i = 1; i < n; i++) {
		printf("elemento %d : %d\n",i,V[i]);
	}

return 0;
}
