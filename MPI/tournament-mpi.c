#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG
#define PRINT_DEBUG(M, ...) printf("%s", M)
#else
#define PRINT_DEBUG(M, ...)
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
	MPI_Request req;
} rounds_t;

int P;
int rank;
int logP;
rounds_t *rounds;
int tag_up = 1;
int tag_down = 2;

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
    //printf("Num of tasks is %d; logP is %d\n", P, logP);
    //printf("My rank is %d\n", rank);

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
    					//PRINT_DEBUG("My(%d) opponent at level %d is %d\n", rank, k, rounds[k].opponent);
    				}	

    			}
    		}
    		if(rank % power2k == power2k_prev)
    		{
    			rounds[k].role = LOSER;
    			rounds[k].opponent = rank - power2k_prev; 
    			//PRINT_DEBUG("My(%d) opponent at level %d is %d\n", rank, k, rounds[k].opponent);
    		}
    		if(rank == 0 && power2k >= P)
    		{
    			rounds[k].role = CHAMPION;
    			rounds[k].opponent = rank + power2k_prev;
    			//PRINT_DEBUG("I (%d) am the champion at level %d", rank, k);
    			//PRINT_DEBUG("My(%d) opponent at level %d is %d\n", rank, k, rounds[k].opponent);
    		}
    	}
    	power2k_prev = power2k;
    	power2k = power2k * 2;
    }
    
    //tournament_barrier();
    int val = 0;
    int send_buff = 1;
    int recv_buff;
    MPI_Status stat;
    /*
    if(rank == 0)
    {
    	printf("0 is waiting to receive from 1\n");
    	MPI_Recv(&recv_buff, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &stat);
    	printf("0 received from %d\n", stat.MPI_SOURCE);
    	MPI_Recv(&recv_buff, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &stat);
    	printf("0 received from %d\n", stat.MPI_SOURCE);
    }
    if(rank > 0)
    {
    	for (i = 0; i < 10000000; i++)
    		val = 1;
    	printf("%d is sending to 0\n", rank);
    	MPI_Send(&send_buff, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    	printf("%d sent to 0\n", rank);
    }
    */
    for(i = 1; i < 1000; i++)
    {
    	val = val+1;
    	//MPI_Send(&send_buff, 1, MPI_INT, (rank+1) % P, 100, MPI_COMM_WORLD);
    	//MPI_Recv(&recv_buff, 1, MPI_INT, (rank-1)% P, 100, MPI_COMM_WORLD, &stat);
    	val = val + recv_buff;
    	//printf("%d: Entering barrier %d\n", rank, i);
    	tournament_barrier();
    	printf("Result %d: AFter the barrier %d value %d\n",rank, i, val );
    	tournament_barrier();
    	//tournament_barrier();
    }
    
    MPI_Finalize();
}

void tournament_barrier()
{
	//int *send_buf = (int*) malloc(sizeof(int));
	int round = 0;
	int fin = 0;
	while(!fin)
	{
		round++;
		//PRINT_DEBUG("At level %d, %d is %d\n",round, rank, rounds[round].role);
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
			case DROPOUT:
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
