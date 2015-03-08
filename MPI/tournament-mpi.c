#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

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

void tournament_barrier_init();
void tournament_barrier();

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
	
    tournament_barrier_init();

    struct timeval tv;
    struct timeval tv1;

    if(rank == 0)
		gettimeofday(&tv, NULL);
    for(i = 0; i < NUMLOOPS; i++)
    {
		tournament_barrier();
    	tournament_barrier();
		tournament_barrier();
		tournament_barrier();
		tournament_barrier();
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

void tournament_barrier_init()
{
	int k;
	int power2k = 1;
	int power2k_prev;

	MPI_Comm_size(MPI_COMM_WORLD,&P);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	logP = (int) ceil(log2(P));
    rounds = (rounds_t*) calloc(logP+1, sizeof(rounds_t));

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
				MPI_Recv(&rounds[round].recv_buf, 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD, &rounds[round].stat);
				fin = 1;
				break;
			case WINNER:
				MPI_Recv(&rounds[round].recv_buf, 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD, &rounds[round].stat);
				break;
			case CHAMPION:
				MPI_Recv(&rounds[round].recv_buf, 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD, &rounds[round].stat);
				MPI_Send(&(rounds[round].send_buf), 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD);
				fin = 1;
				break;
			case BYE:
				continue;
			case DROPOUT:
				printf("impossible!!!\n");
				continue;
		}
	}
	fin = 0;
	while(!fin)
	{
		round--;
		switch(rounds[round].role)
		{
			case WINNER:
				MPI_Send(&(rounds[round].send_buf), 1, MPI_INT, rounds[round].opponent, round, MPI_COMM_WORLD);
				continue;
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
}
