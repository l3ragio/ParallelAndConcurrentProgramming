#include <stdlib.h>
#include <stdio.h>
#include "mpi.h" 
#include <math.h>




int main(int argc, char **argv) 
{ 
 int rank,ranksent,size,tag,temp,len,j,i;
 char name[20];
 int n;


 len=20;
 MPI_Status status; 
 MPI_Init(&argc,&argv); 
 MPI_Comm_rank(MPI_COMM_WORLD,&rank); 
 MPI_Comm_size(MPI_COMM_WORLD,&size); 
 MPI_Get_processor_name(name,&len);


	if(rank==0){
	//set the n value
	printf("\nAAA****insert the n:\n");
	scanf("%d",&n);
	long double sum=0,subdata;

		//send the N to all the processes before the start of the computation 
		for(i=1;i<size;i++){
		MPI_Ssend(&n,1,MPI_INT ,i,i,MPI_COMM_WORLD);
		}

		// recive all subdatas and sum them
		for(i=1;i<=n;i++){
			//Recive the subdata
			MPI_Recv(&subdata,1,MPI_LONG_DOUBLE ,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			printf("\nAAA++%dth SUBDATA recived: %llf from Proc: %d\n",status.MPI_TAG, subdata,status.MPI_SOURCE);
			//sum it to the accumulator
			sum+=subdata;
			printf("\nAAA-----------SUM variable value is: %llf-----------\n", sum);
		}

	//calculate the difference with log(n)
	sum-=log(n);
	//print results
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>RESULT1:  =    %llf\n",sum);
	}

	/* **the weight of computation is divided on all processes but if necessary the higher 
	number of iterations will be done by the process who has the higher rank*/
	//last process
	else if(rank==size-1){
	long double subd=1;


	//recive the N value
	MPI_Recv(&n,1,MPI_INT ,0,rank,MPI_COMM_WORLD,&status);
	printf("\nBBB Process: %d, recives %d from %d \n",rank,n,status.MPI_SOURCE);
	//set start and end of partial work: the "rank-1" and "size-1" are 
	//used because the process rank 0 have to do a different work
	int start=(rank-1)*(n/(size-1))+1;
	int end=n;

	printf("\nBBB<><><><> proc: %d, START: %d, END: %d, amount data to compute: %d\n",rank,start,end, end-start+1);
		//calculate and send all values
		for(i=start; i<=end ;i++)
		{      
		//calculate the 1/k;
		subd=1/(long double)i;
		//send it to process 0
		MPI_Send(&subd,1,MPI_LONG_DOUBLE ,0,(int)i,MPI_COMM_WORLD);
		printf("\nBBB proc: %d, %dth subdata:%llf sent!>>\n",rank,i,subd);

		}
	printf("\nBBB proc: %d, ALL MY SUBDATAS ARE BEEN SENDED!_X_\n",rank);
	}
	//all other processes
	else{	

	long double subd=1;
 

	//recive the N value
	MPI_Recv(&n,1,MPI_INT ,0,rank,MPI_COMM_WORLD,&status);
	printf("\nBBB Process: %d, recives %d from %d\n",rank,n,status.MPI_SOURCE);
	//set start and end of partial work:  the "rank-1" and "size-1" are 
	//used because the process rank 0 have to do a different work
	int start=(rank-1)*(n/(size-1))+1;
	int end=start+(n/(size-1));
	printf("\nBBB<><><><> proc: %d, START: %d, END: %d, amount data to compute: %d\n",rank,start,end-1, end-start);
		//calculate and send all values
		for(i=start;i<end;i++)
		{
		//calculate the 1/k;
		subd=1/(long double)i;
		//send it to process 0
		MPI_Send(&subd,1,MPI_LONG_DOUBLE ,0,(int)i,MPI_COMM_WORLD);
		printf("\nBBB proc: %d, %dth subdata:%llf sent!>>\n",rank,i,subd);

		}
	printf("\nBBB proc: %d, ALL MY SUBDATAS ARE BEEN SENDED!_X_\n",rank);
	}



 MPI_Finalize();
 return 0;
}
