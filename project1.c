#include <stdlib.h>
#include <stdio.h>
#include "mpi.h" 
#include <math.h>

typedef struct {
	int dest;
	int loc;
	int dir;
	int rank;
} Data;

void bubble_sort(Data *V, int n, int dir) 
{
	int c, d; 
	Data t;
	if (dir==10){
		for (c=0;c<(n-1);c++) 
		{
			for(d=0;d<n-c-1;d++) 
			{
				if (V[d].dest>V[d + 1].dest) 
				{
				t.dest=V[d].dest;
				t.loc=V[d].loc;
				t.dir=V[d].dir;
				t.rank=V[d].rank;
				V[d].dest=V[d+1].dest;
				V[d].loc=V[d+1].loc;
				V[d].dir=V[d+1].dir;
				V[d].rank=V[d+1].rank;
				V[d+1].dest=t.dest;
				V[d+1].loc=t.loc;
				V[d+1].dir=t.dir;
				V[d+1].rank=t.rank;
				}
			}
		}
	} else 
	{
		for (c=0;c<(n-1);c++) 
		{
			for(d=0;d<n-c-1;d++) 
			{
				if (V[d].dest<V[d + 1].dest) 
				{
				t.dest=V[d].dest;
				t.loc=V[d].loc;
				t.dir=V[d].dir;
				t.rank=V[d].rank;
				V[d].dest=V[d+1].dest;
				V[d].loc=V[d+1].loc;
				V[d].dir=V[d+1].dir;
				V[d].rank=V[d+1].rank;
				V[d+1].dest=t.dest;
				V[d+1].loc=t.loc;
				V[d+1].dir=t.dir;
				V[d+1].rank=t.rank;
				}
			}
		}

	}
}

void removeAt(Data* V, int i, int n) {
	int j;
	for (j = i; j < n-1 ; j++) {
	V[j].dest = V[j + 1].dest;
	V[j].loc = V[j + 1].loc;
	V[j].dir =V[j + 1].dir;
	V[j].rank = V[j + 1].rank;
	}
	V[n - 1].dest = -1;
	V[n - 1].loc = -1;
	V[n - 1].dir = 0;
	V[n - 1].rank = -1;
}




int main(int argc, char **argv) 
{ 
 int rank,ranksent,size,count,floor,dest,direction,tag,source,i,temp,len,dxd=0, flag;
 char name[20];
 int a;
 Data tabrecord,tabrecieved;
 

 len=20;
 MPI_Status status,status1; 
 MPI_Init(&argc,&argv); 
 MPI_Comm_rank(MPI_COMM_WORLD,&rank); 
 MPI_Comm_size(MPI_COMM_WORLD,&size); 
 MPI_Get_processor_name(name,&len);

 MPI_Request request1;
//new struct initialization
 MPI_Datatype new_type;
 MPI_Datatype type[4] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT };
 int blen[4] = { 1, 1 ,1, 1};
 MPI_Aint disp[4];
 MPI_Aint base, addr;

 MPI_Get_address(&tabrecord, &base);
 MPI_Get_address(&(tabrecord.dest), &addr);
 disp[0] = addr - base;
 MPI_Get_address(&(tabrecord.loc), &addr);
 disp[1] = addr - base;
 MPI_Get_address(&(tabrecord.dir), &addr);
 disp[2] = addr - base;
 MPI_Get_address(&(tabrecord.rank), &addr);
 disp[3] = addr - base;

 MPI_Type_create_struct(4, blen, disp, type, &new_type);
 MPI_Type_commit(&new_type);
//
 
 srand(time(NULL)+ rank);




	if(rank==0)
	{
	 Data collectData[size];

	 	printf("\n||ELEVATOR starts!!!!!!!!!!!!!!||\n");
	 
		while(1)
		{
		printf("\n||*************IDLE************||\n");
		//recive the first elevator call and go to the location to take a passenger:
		
		MPI_Recv(&tabrecieved,1,new_type,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		count=0;
		//store passengers datas in a vector of structs
		collectData[0]=tabrecieved;	

		floor=collectData[0].loc;
		direction=collectData[0].dir;
		source=collectData[0].rank;
		printf("\n||CALLED BY: %d, loc: %d, dest: %d||\n",tabrecieved.rank,tabrecieved.loc,tabrecieved.dest);

			do{	  
				printf("\n|| _____GOING TO THE FLOOR: %d___||\n",tabrecieved.loc);
				printf("\n||. . . . . . . . . . . . . . .||\n");
				sleep(1);
				printf("\n||. . . . . . . . . . . . . . .||\n");
				sleep(1);
				printf("\n||___________FLOOR: %d__________||\n", floor);
				// arrive at the first step and take-on more passengers, if they have to go in the same direction
				for(i=1;i<size;i++)	
				{
					//recive all passengers who are in the same floor as the elevator and have the same direction as the passenger who calls
					MPI_Iprobe(i, floor*direction, MPI_COMM_WORLD,&flag, &status);
					if(flag){
					MPI_Recv(&tabrecieved,1,new_type,i,floor*direction,MPI_COMM_WORLD,&status);
					printf("\n||++ proc: %d, loc: %d,dest : %d ||\n",tabrecieved.rank,tabrecieved.loc,tabrecieved.dest);
					count++;
					collectData[count]=tabrecieved;
					}

				}
				//get out the passengers who have to leave the elevator
				i=0;
				while(i<=count)
				{	
					//Send a synchronus message for say at the process that he have to leave
					if(floor==collectData[i].dest)
					{
					
					printf("\n||--proc %d has to leave!!!!!!!!||\n",collectData[i].rank);
					MPI_Ssend(&tabrecieved,1,new_type,collectData[i].rank,collectData[i].dir*collectData[i].dest,MPI_COMM_WORLD);
					//remove the passenger from the list:
					removeAt(collectData,i,size);		
					count--;
					}
					else{
					i++;
					}
				}
				
				//ordinate the passengers in leave order
			    bubble_sort(collectData,count+1,direction);
				
				printf("\n||DESTINATION SCHEDULE---------||\n");
				for(i=0;i<=count;i++)
				{
				printf("\n|| -> proc %d, loc %d, dest ((%d))||\n",collectData[i].rank,collectData[i].loc,collectData[i].dest);
				}	
				printf("\n||-----------------------------||\n");
			//set new datas for condition
			direction=collectData[0].dir;
			floor=collectData[0].dest;
			source=collectData[0].rank;
			
			printf("\n||PROCESSES ON THE ELEVATOR: %d ||\n", count+1);


			}while(count!=-1);
		}
	}
	else
	{
		//ALL PROCESSES GENERATE RANDOMS DATAS, then comunicate with rank 0
		while(1)
		{
		  srand(time(NULL)+ rank);
		
			do
			{
			tabrecord.loc=(rand()%8)+1;
			tabrecord.dest=(rand()%8)+1;
			}while(tabrecord.loc==tabrecord.dest);
			tabrecord.dir=1;
			if(tabrecord.loc-tabrecord.dest<0)
			{
			tabrecord.dir=10;
			}
			tabrecord.rank=rank;

		 printf("\n\t**(process: %d, loc: %d, dest: %d it's waiting)**\n",rank,tabrecord.loc,tabrecord.dest*tabrecord.dir);
		 //Send a synchronus message to the elevator(rank 0) and Wait
		// the tag passed defines the location and the direction 
		 MPI_Ssend(&tabrecord,1,new_type,0,tabrecord.loc*tabrecord.dir,MPI_COMM_WORLD);
		 printf("\n\t++(process: %d, dest: %d enters on the elevator)++\n",rank,tabrecord.loc );
		 // Wait untill the elevator sends me a message and then leave it 
		 MPI_Recv(&tabrecieved,1,new_type,0,tabrecord.dest*tabrecord.dir,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		 printf("\n\t--(process: %d dest: %d leaves on the elevator)--\n",rank,tabrecord.dest);
		 
		}
	}
 MPI_Finalize(); 
 return(0);
}
