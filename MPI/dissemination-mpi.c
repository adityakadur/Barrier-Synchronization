#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#ifdef DEBUG
#define PRINT_DEBUG(fmt, args...) printf(fmt, ## args)
#else
#define PRINT_DEBUG(M, args...)
#endif

int P;
int logP;
int rank;
int *partner;

void dissemination_barrier();

int main(int argc, char *argv[])
{
	int ret_val;
	unsigned long power2i;
	int i;

	ret_val = MPI_Init(&argc, &argv);
	if (ret_val != MPI_SUCCESS)
	{
		printf("Failure initializing MPI\n");
		MPI_Abort(MPI_COMM_WORLD, ret_val);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&P);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    logP = (int) ceil(log2(P));
    partner = (int*) calloc(logP, sizeof(int));

    power2i = 1;
    for(i = 0; i < logP; i++)
    {
    	partner[i] = (rank + power2i) % P;
    	PRINT_DEBUG("%d: Partner in round %d is %d\n", rank, i, partner[i]);
    	power2i = power2i * 2;
    }
    dissemination_barrier();

    int val = 0;
    struct timeval tv;
    unsigned long timestamp;
    for(i = 0; i < 1000; i++)
    {
    	val = val+1;
    	dissemination_barrier();   	
    	gettimeofday(&tv, NULL);
    	timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
    	printf("Result %d After the barrier %d value %d at %lu\n",rank, i, val, timestamp);
    	dissemination_barrier();
    }
    MPI_Finalize();
	return 0;
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
		PRINT_DEBUG("%d: In round %d, sent message to %d\n", rank, i, partner[i]);
		MPI_Recv(&recv_buf, 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &stat);
		PRINT_DEBUG("%d: In round %d, received message from %d\n", rank, i, stat.MPI_SOURCE);
	}
}