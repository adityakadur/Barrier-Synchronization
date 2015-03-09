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
	int length;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &length);
	printf("Rank %d, name of processor %s\n", rank, name);
    MPI_Finalize();
}

