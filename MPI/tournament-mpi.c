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

typedef enum {
	WINNER,
	LOSER,
	BYE,
	CHAMPION,
	DROPOUT
} role_t;

typedef struct {
	role_t role;
	int opponent;
	int send_buf;
	int recv_buf;
	MPI_Status stat;
} rounds_t;

int P;
int logP;
int rank;
rounds_t *rounds;

void tournament_barrier();

int main(int argc, char *argv[])
{
	int ret_val;
	unsigned long power2k, power2k_prev;
	int k;
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
    rounds = (rounds_t*) calloc(logP+1, sizeof(rounds_t));
    power2k = 1;

    // Assign roles and opponents to each level of this process
    for(k = 0; k <= logP; k++)
    {
    	rounds[k].send_buf = 1;
    	if(k == 0)
    		rounds[k].role = DROPOUT;
    	else
    	{
    		if(rank % power2k == 0)
    		{
    			if(rank + power2k_prev >= P)
    				rounds[k].role = BYE;
    			else
    			{
    				if(power2k < P)
    				{
    					rounds[k].role = WINNER;
    					rounds[k].opponent = rank + power2k_prev;
    					PRINT_DEBUG("My(%d) opponent at level %d is %d\n", rank, k, rounds[k].opponent);
    				}	

    			}
    		}
    		if(rank % power2k == power2k_prev)
    		{
    			rounds[k].role = LOSER;
    			rounds[k].opponent = rank - power2k_prev; 
    			PRINT_DEBUG("My(%d) opponent at level %d is %d\n", rank, k, rounds[k].opponent);
    		}
    		if(rank == 0 && power2k >= P)
    		{
    			rounds[k].role = CHAMPION;
    			rounds[k].opponent = rank + power2k_prev;
    			PRINT_DEBUG("I (%d) am the champion at level %d", rank, k);
    			PRINT_DEBUG("My(%d) opponent at level %d is %d\n", rank, k, rounds[k].opponent);
    		}
    	}
    	power2k_prev = power2k;
    	power2k = power2k * 2;
    }
    
    int val = 0;
    struct timeval tv;
    unsigned long timestamp;
    for(i = 0; i < 1000; i++)
    {
    	val = val+1;
    	//printf("%d: Entering barrier %d\n", rank, i);
    	tournament_barrier();   	
    	gettimeofday(&tv, NULL);
    	timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
    	printf("Result %d After the barrier %d value %d at %lu\n",rank, i, val, timestamp );
    	tournament_barrier();
    }
    MPI_Finalize();
}

void tournament_barrier()
{
	int round = 0;
	int fin = 0;
	while(!fin)
	{
		round++;
		switch(rounds[round].role)
		{
			case LOSER:
				MPI_Send(&(rounds[round].send_buf), 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD);
				PRINT_DEBUG("%d: Loser at level %d sent tag %d to %d\n", rank, round, round, rounds[round].opponent);
				MPI_Recv(&rounds[round].recv_buf, 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD, &rounds[round].stat);
				PRINT_DEBUG("%d: Loser at level %d received tag %d from %d\n", rank, round, rounds[round].stat.MPI_TAG, rounds[round].stat.MPI_SOURCE);
				fin = 1;
				break;
			case WINNER:
				MPI_Recv(&rounds[round].recv_buf, 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD, &rounds[round].stat);
				PRINT_DEBUG("%d: Winner at level %d received tag %d from %d\n", rank, round, rounds[round].stat.MPI_TAG, rounds[round].stat.MPI_SOURCE);
				break;
			case CHAMPION:
				MPI_Recv(&rounds[round].recv_buf, 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD, &rounds[round].stat);
				PRINT_DEBUG("%d: Champion at level %d received tag %d from %d\n", rank, round, rounds[round].stat.MPI_TAG, rounds[round].stat.MPI_SOURCE);
				MPI_Send(&(rounds[round].send_buf), 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD);
				PRINT_DEBUG("%d: Champion at level %d sent tag %d to %d\n", rank, round, round, rounds[round].opponent);
				fin = 1;
				break;
			case BYE:
				continue;
			case DROPOUT:
				printf("impossible!!!\n");
				continue;
		}
	}
	//PRINT_DEBUG("Reached the top in %d. Moving down\n", rank);
	fin = 0;
	while(!fin)
	{
		round--;
		switch(rounds[round].role)
		{
			case WINNER:
				MPI_Send(&(rounds[round].send_buf), 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD);
				PRINT_DEBUG("%d: Winner at level %d sent tag %d to %d\n", rank, round, round, rounds[round].opponent);
				continue;
			case DROPOUT:
				PRINT_DEBUG("%d: At level %d, DROPOUT exiting the loop\n", rank, round);
				fin = 1;
				break;
			case LOSER:			// impossible
				printf("impossible!!!\n");
				continue;
			case BYE:			// do nothing
			 	continue;
			case CHAMPION:		// impossible
				printf("impossible!!!\n");
				continue;
		}
	}
	//PRINT_DEBUG("End of barrier in %d\n", rank );
}
