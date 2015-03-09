#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#ifdef DEBUG
#define PRINT_DEBUG(fmt, args...) printf(fmt, ## args)
#else
#define PRINT_DEBUG(M, args...)
#endif

#define DROPOUT 0
#define LOSER 1
#define WINNER 2
#define BYE 3
#define CHAMPION 4

int P, logP, rank, T, NUMLOOPS, sense = 1;
int *partner;

typedef struct _record{
    int flag;
    int role;
    int opp;
}record;
record* rounds;

void combined_barrier();
void dissemination_mpi_barrier_init();
void dissemination_mpi_barrier();
void combined_barrier_init();
void combined_barrier(int *sense);

int main(int argc, char *argv[])
{
	int ret_val, i, my_sense = 1;
	unsigned long power2i;
	ret_val = MPI_Init(&argc, &argv);
	if (ret_val != MPI_SUCCESS)
    {
        printf("Failure initializing MPI\n");
        MPI_Abort(MPI_COMM_WORLD, ret_val);
    }
    if (argc==3){
        if (sscanf (argv[1], "%d", &T)!=1) printf ("T - not an integer\n");
        if (sscanf (argv[2], "%d", &NUMLOOPS)!=1) printf ("N - not an integer\n");
    }
    else {T = 4; NUMLOOPS = 10000;}
	MPI_Comm_size(MPI_COMM_WORLD,&P);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    combined_barrier_init();
    struct timeval tv1, tv2;

    #pragma omp parallel num_threads(T) shared(rounds,tv1,tv2) firstprivate(my_sense, i)
    {
        if(rank == 0)
            gettimeofday(&tv1, NULL);
        for(i = 0; i < NUMLOOPS; i++)
        {
	        combined_barrier(&my_sense);
            combined_barrier(&my_sense);
            combined_barrier(&my_sense);
            combined_barrier(&my_sense);
            combined_barrier(&my_sense);
        }
    }
    if (rank == 0)
    {
        gettimeofday(&tv2, NULL);
        double total_time = (double) (tv2.tv_usec - tv1.tv_usec) + (double) (tv2.tv_sec - tv1.tv_sec)*1000000;
        printf("\nSUMMARY for combined barrier:\n"
            "Number of processes: %d\nNumber of threads: %d\n"
            "Total run-time for %d "
            "loops with 5 barriers per loop: %fs\n"
            "The average time per barrier: %fus\n",
            P, T, NUMLOOPS, total_time/1000000, (double)(total_time/(NUMLOOPS*5)));
    }
    MPI_Finalize();
	return 0;
}


void dissemination_mpi_barrier_init()
{
    logP = (int) ceil(log2(P));
    partner = (int*) calloc(logP, sizeof(int));
    int i, power2i;
    power2i = 1;
    for(i = 0; i < logP; i++)
    {
        partner[i] = (rank + power2i) % P;
        PRINT_DEBUG("%d: Partner in round %d is %d\n", rank, i, partner[i]);
        power2i = power2i * 2;
    }
}

void dissemination_mpi_barrier()
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

void combined_barrier_init(){
    int size = T*(int)(log2(2*T-1)+1);
    rounds = malloc(size * sizeof(record));
    
    int i;
    for (i=0; i<size; i++){
        rounds[i].flag = 0;
        rounds[i].role = -1;
        rounds[i].opp = -1;
        int j, k;
        j = i%T;
        k = i/T;
        if(k == 0)
            rounds[i].role = DROPOUT;
        else {
            if (j%(1<<k) == 0){
                if (((j + (1<<(k-1))) < T) && ((1<<k) < T)){
                    rounds[i].role = WINNER;
                    rounds[i].opp = (i+(1<<(k-1)))%T;
                }
                else if ((j + (1<<(k-1))) >= T)
                    rounds[i].role = BYE;
            }
            if ( (j%(1<<k)) == (1<<(k-1))){
                rounds[i].role = LOSER;
                rounds[i].opp = (i-(1<<(k-1)))%T;
            }
            if (j==0  &&  (1<<k) >= T){
                rounds[i].role = CHAMPION;
                rounds[i].opp = (i+(1<<(k-1)))%T;
            }
        }
    }
    
    /*   printf("Pno . Role \n");
 *       *       for(i=0; i<size; i++){
 *           *               printf("%d.%d.%d    ",i%T,rounds[i].role, rounds[i].opp);
 *               *                       if(i%T==T-1)
 *                   *                                   printf("\n");
 *                       *                                       }
 *                           *                                           */
    dissemination_mpi_barrier_init();
}

void combined_barrier(int *sense){
    int vpid =  omp_get_thread_num();
    int i,fin = 0;
    int round = 1;
    while(!fin){
        i = round*T+vpid;
        switch(rounds[i].role){
            case LOSER:
                rounds[round*T+rounds[i].opp].flag = *sense;
                while(rounds[i].flag != *sense);
                fin = 1;
                break;
            case WINNER:
                while(rounds[i].flag != *sense);
            case BYE:
                break;
            case CHAMPION:
                while(rounds[i].flag != *sense);
                dissemination_mpi_barrier();
                rounds[round*T+rounds[i].opp].flag = *sense;
                fin = 1;
                break;
        }
        round++;
    }
    round--;
    fin = 0;
    while(!fin){
        round--;
        i = round*T+vpid;
        switch(rounds[i].role){
            case LOSER:
                printf("1 IMPOSSIBLE!!!!!\n");
                break;
            case WINNER:
                rounds[round*T+rounds[i].opp].flag = *sense;
            case BYE:
                break;
            case CHAMPION:
                printf("2 IMPOSSIBLE!!!!!\n");
                break;
            case DROPOUT:
                fin = 1;
                break;
        }
    }
    *sense = 1-*sense;
    
}


