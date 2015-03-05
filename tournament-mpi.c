#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
	int buf;
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
    printf("Num of tasks is %d; logP is %d\n", P, logP);

    rounds = (rounds_t*) malloc(sizeof(rounds_t));
    power2k = 1;

    // Assign roles and opponents to each level of this process
    for(k = 0; k < logP; k++)
    {
    	rounds[k].buf = 1;
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
    				}	

    			}
    		}
    		if(rank % power2k == power2k_prev)
    		{
    			rounds[k].role = LOSER;
    			rounds[k].opponent = rank - power2k_prev; 
    		}
    		if(rank == 0 && power2k >= P)
    		{
    			rounds[k].role = CHAMPION;
    			rounds[k].opponent = rank + power2k_prev;
    		}
    	}
    	power2k_prev = power2k;
    	power2k = power2k * 2;
    }
    for(i = 1; i < 10000; i++)
    {
    	tournament_barrier();
    }
}

void tournament_barrier()
{
	int *send_buf = (int*) malloc(sizeof(int));
	int round = 1;
	while(1)
	{
		switch(rounds[round].role)
		{
			case LOSER:
				MPI_Isend(&(rounds[round].buf), 1, MPI_INT, rounds[round].opponent, MPI_ANY_TAG, MPI_COMM_WORLD, &rounds[round].req );
				MPI_Irecv(&rounds[round].buf, 1, MPI_INT, rounds[round].opponent, MPI_ANY_TAG, MPI_COMM_WORLD, &rounds[round].req);
				break;
			case WINNER:
				MPI_Irecv(&rounds[round].buf, 1, MPI_INT, rounds[round].opponent, MPI_ANY_TAG, MPI_COMM_WORLD, &rounds[round].req);
				break;
			case CHAMPION:
				MPI_Irecv(&rounds[round].buf, 1, MPI_INT, rounds[round].opponent, MPI_ANY_TAG, MPI_COMM_WORLD, &rounds[round].req);
				MPI_Isend(&(rounds[round].buf), 1, MPI_INT, rounds[round].opponent, MPI_ANY_TAG, MPI_COMM_WORLD, &rounds[round].req );
				break;
			case BYE:
				continue;
			case DROPOUT:
				continue;
		}
		round++;
	}
	while(1)
	{
		round--;
		switch(rounds[round].role)
		{
			case WINNER:
				MPI_Isend(&(rounds[round].buf), 1, MPI_INT, rounds[round].opponent, MPI_ANY_TAG, MPI_COMM_WORLD, &rounds[round].req );
			case DROPOUT:
				break;
			case LOSER:			// impossible
				continue;
			case BYE:			// do nothing
			 	continue;
			case CHAMPION:		// impossible
				continue;
		}
	}
}
