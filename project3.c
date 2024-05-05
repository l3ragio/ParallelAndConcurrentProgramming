#include <stdlib.h>
#include <stdio.h>
#include "mpi.h" 
#include <math.h>
#include <time.h>

static struct timespec start,stop;

//time measure functions imported by the teacher file
void tstart(void)
{
	clock_gettime( CLOCK_REALTIME, &start);	
}

double tstop(void)
{
	clock_gettime( CLOCK_REALTIME, &stop);
	return (double)( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/1.0e9;
}


int main(int argc, char **argv) 
{ 
 int rank,ranksent,size,tag,i,temp,len,flag,j;
 char name[20];

 

 len=20;
 MPI_Status status; 
 MPI_Init(&argc,&argv); 
 MPI_Comm_rank(MPI_COMM_WORLD,&rank); 
 MPI_Comm_size(MPI_COMM_WORLD,&size); 
 MPI_Get_processor_name(name,&len);
//set the exponent
long double x=15;

/**set the size of infinite:
 - if it's too high: the result of factorial will generates an overflow and the result of e^x will be = "inf"
 - if it's too low: the approximation will not be precise.
*/
int n=100;

//rank 0 recive all subdatas and sum them
if(rank==0){

	long double fat,sum=0,power;
	//printf("\n****insert the x:\n");
	//scanf("%llf",&x);
	tstart();

// recive all subdatas and sum
	for(i=0;i<=n;i++){
		//no restrictions on the recive, datas are readed in arrive order
		MPI_Recv(&power,1,MPI_LONG_DOUBLE ,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		printf("\nAAA++%dth SUBDATA recived: %llf from Proc: %d\n",i, power,status.MPI_SOURCE);
		//sum the data recived in the accumulator
		sum+=power;
		printf("\nAAA-----------SUM variable value is: %llf-----------\n", sum);
	}

//print results
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>RESULT: imp e^%llf =    %llf\n",x,sum);
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>RESULT: math.h e^%llf = %lf\n\n", x,exp(x));
// the time of 	
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>TIME: = %lf\n\n", tstop());
	
}
//**the weight of computation is divided on all processes but if necessary the higher number 
//of iterations will be done by the process who have the higher rank
else if(rank==size-1){
	//initialize factorial variable to 1
	long double fact=1,gg=1;
	//set the start index of the loop: the "rank-1" and "size-1" are 
	//used because the process rank 0 have to do a different work
	int start=(rank-1)*(n/(size-1));
	//set the end index of the loop
	int end=n;

	printf("\nBBB<><><><><><><><><><><><><> proc: %d, STARTS: %d, ENDS : %d, amount data: %d\n",rank,start,end,end-start+1);
	for(i=start; i<=end ;i++)
	{      
	fact=1;
	//calculating the factiorial
		for(j=i;j>0;j--)
		{
		fact*=j;	
		}
	//calculating the subdata
	gg=pow(x,i)/fact;
	//send it to the process 0
	MPI_Send(&gg,1,MPI_LONG_DOUBLE ,0,i,MPI_COMM_WORLD);
	printf("\nBBB proc: %d, %dth subdata:%llf sent!>>\n",rank,gg,i);

	}
	printf("\nBBB proc: %d, ALL SUBDATAS ARE BEEN SENDED!_X_\n",rank);
}
else{	
	//initialize factorial variable to 1
	long double fact=1,gg=1;
	//set the start index of the loop: the "rank-1" and "size-1" are 
	//used because the process rank 0 have to do a different work
	int start=(rank-1)*(n/(size-1));
	//set the end index of the loop
	int end=(rank-1)*(n/(size-1))+(n/(size-1));
	//calculating the factiorial
	printf("\nBBB<><><><><><><><><><><><><> proc: %d, START: %d, END: %d, amount data: %d\n",rank,start,end-1,end-start);
	for(i=start;i<end;i++)
	{
	fact=1;
		for(j=i;j>0;j--)
		{
		fact*=j;	
		}
	//calculating the subdata
	gg=pow(x,i)/fact;
	//send it to the process 0
	MPI_Send(&gg,1,MPI_LONG_DOUBLE ,0,i,MPI_COMM_WORLD);
	printf("\nBBB proc: %d, %dth subdata:%llf sent!>>\n",rank,gg,i);

	}	
	printf("\nBBB proc: %d, ALL SUBDATAS ARE BEEN SENDED!_X_\n",rank);
}



 MPI_Finalize();
 return 0;
}
