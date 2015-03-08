#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

int P;
int logP;
int rank;
int *partner;

void dissemination_barrier_init();
void dissemination_barrier();

int main(int argc, char *argv[])
{
	int ret_val;
	int i;
	int NUMLOOPS = 10000;

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

	dissemination_barrier_init();

    struct timeval tv, tv1;
    if(rank == 0)
        gettimeofday(&tv, NULL);
    for(i = 0; i < NUMLOOPS; i++)
    {
	    dissemination_barrier();
        dissemination_barrier();
        dissemination_barrier();
        dissemination_barrier();
        dissemination_barrier();
    }
    if (rank == 0)
    {
        gettimeofday(&tv1, NULL);
        double total_time = (double) (tv1.tv_usec - tv.tv_usec) + (double) (tv1.tv_sec - tv.tv_sec)*1000000;
        printf("\nSUMMARY for dissemination barrier:\nNumber of processes: %d\nTotal run-time for %d "
            "loops with 5 barriers per loop: %fs\n"
            "The average time per barrier: %fus\n",
            P, NUMLOOPS, total_time/1000000, (double)(total_time/(NUMLOOPS*5)));
    }
    MPI_Finalize();
	return 0;
}

void dissemination_barrier_init()
{
    int i;
    int power2i;

    MPI_Comm_size(MPI_COMM_WORLD,&P);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    logP = (int) ceil(log2(P));
    partner = (int*) calloc(logP, sizeof(int));
    power2i = 1;

    for(i = 0; i < logP; i++)
    {
        partner[i] = (rank + power2i) % P;
        power2i = power2i * 2;
    }
}

void dissemination_barrier()
{
	int i;
	int send_buf;
	int recv_buf;
	MPI_Status stat;

	for(i = 0; i < logP; i++)
	{
		MPI_Send(&send_buf, 1, MPI_INT, partner[i], i, MPI_COMM_WORLD);
		MPI_Recv(&recv_buf, 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &stat);
	}
}
