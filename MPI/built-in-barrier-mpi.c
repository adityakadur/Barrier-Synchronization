#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

int P;
int rank;

int main(int argc, char *argv[])
{
	int ret_val;
	int i;
	int NUMLOOPS = 1000;
	
	ret_val = MPI_Init(&argc, &argv);
	if (ret_val != MPI_SUCCESS)
	{
		printf("Failure initializing MPI\n");
		MPI_Abort(MPI_COMM_WORLD, ret_val);
	}

	if(argc == 2)
    {
        if (sscanf (argv[1], "%d", &NUMLOOPS)!=1) printf ("N - not an integer\n");
	}
	MPI_Comm_size(MPI_COMM_WORLD,&P);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    struct timeval tv;
    struct timeval tv1;

    if(rank == 0)
		gettimeofday(&tv, NULL);
    for(i = 0; i < NUMLOOPS; i++)
    {
		MPI_barrier(MPI_COMM_WORLD);
    	MPI_barrier(MPI_COMM_WORLD);
    	MPI_barrier(MPI_COMM_WORLD);
    	MPI_barrier(MPI_COMM_WORLD);
    	MPI_barrier(MPI_COMM_WORLD);
    }
    if (rank == 0)
    {
		gettimeofday(&tv1, NULL);
		double total_time = (double) (tv1.tv_usec - tv.tv_usec) + (double) (tv1.tv_sec - tv.tv_sec)*1000000;
		printf("\nSUMMARY for Tournament barrier:\nNumber of processes: %d\nTotal run-time for %d "
        	    "loops with 5 barriers per loop: %fs\n"
            	"The average time per barrier: %fus\n",
            	P, NUMLOOPS, total_time/1000000, (double)(total_time/(NUMLOOPS*5)));
    }
    MPI_Finalize();
}