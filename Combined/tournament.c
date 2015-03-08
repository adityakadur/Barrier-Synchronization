#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define DROPOUT 0
#define LOSER 1
#define WINNER 2
#define BYE 3
#define CHAMPION 4

typedef struct _record{
    int flag;
    int role;
    int opp;
}record;

int P, NUMLOOPS;
int sense = 1;
record* rounds;

void barrier_init(){
    int size = P*(int)(log2(2*P-1)+1);
    rounds = malloc(size * sizeof(record));
    
    int i;
    for (i=0; i<size; i++){
        rounds[i].flag = 0;
        rounds[i].role = -1;
        rounds[i].opp = -1;
        int j, k;
        j = i%P;
        k = i/P;
        if(k == 0)
            rounds[i].role = DROPOUT;
        else {
            if (j%(1<<k) == 0){
                if (((j + (1<<(k-1))) < P) && ((1<<k) < P)){
                    rounds[i].role = WINNER;
                    rounds[i].opp = (i+(1<<(k-1)))%P;
                }
                else if ((j + (1<<(k-1))) >= P)
                    rounds[i].role = BYE;
            }
            if ( (j%(1<<k)) == (1<<(k-1))){
                rounds[i].role = LOSER;
                rounds[i].opp = (i-(1<<(k-1)))%P;
            }
            if (j==0  &&  (1<<k) >= P){
                rounds[i].role = CHAMPION;
                rounds[i].opp = (i+(1<<(k-1)))%P;
            }
        }
    }
    
 /*   printf("Pno . Role \n");
 *       for(i=0; i<size; i++){
 *               printf("%d.%d.%d    ",i%P,rounds[i].role, rounds[i].opp);
 *                       if(i%P==P-1)
 *                                   printf("\n");
 *                                       }
 *                                           */
}

void barrier(int *sense){
    int vpid =  omp_get_thread_num();
    int i,fin = 0;
    int round = 1;
    while(!fin){
        i = round*P+vpid;
        switch(rounds[i].role){
            case LOSER:
                rounds[round*P+rounds[i].opp].flag = *sense;
                while(rounds[i].flag != *sense);
                fin = 1;
                break;
            case WINNER:
                while(rounds[i].flag != *sense);
            case BYE:
                break;
            case CHAMPION:
                while(rounds[i].flag != *sense);
                rounds[round*P+rounds[i].opp].flag = *sense;
                fin = 1;
                break;
        }
        round++;
    }
    round--;
    fin = 0;
    while(!fin){
        round--;
        i = round*P+vpid;
        switch(rounds[i].role){
            case LOSER:
                printf("1 IMPOSSIBLE!!!!!\n");
                break;
            case WINNER:
                rounds[round*P+rounds[i].opp].flag = *sense;
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

int main(int argc, char* argv[])
{
    if (argc==3){
        if (sscanf (argv[1], "%d", &P)!=1) printf ("P - not an integer\n");
        if (sscanf (argv[2], "%d", &NUMLOOPS)!=1) printf ("N - not an integer\n");
    }
    else {P = 4; NUMLOOPS = 1000;}
    barrier_init();
    printf("\nTournament barrier with sense-reversal\n"
            "--------------------------------------------\n"
            "Number of threads = %d\n", P);
    double total_time;
    struct timeval tv1, tv2;
    int my_sense = 1;
    int j;
    #pragma omp parallel num_threads(P) shared(rounds,tv1,tv2) firstprivate(my_sense, j)
    {
        gettimeofday(&tv1, NULL);
        for (j=0;j<NUMLOOPS;j++){
            barrier(&my_sense);
            barrier(&my_sense);
            barrier(&my_sense);
            barrier(&my_sense);
            barrier(&my_sense);
        }
        gettimeofday(&tv2, NULL);
    }
    
    total_time = (double) (tv2.tv_usec - tv1.tv_usec) + (double) (tv2.tv_sec - tv1.tv_sec)*1000000;
    printf("\nSUMMARY:\nTotal run-time for %d "
            "loops with 5 barriers per loop: %fs\n"
            "The average time per barrier: %fus\n",
            NUMLOOPS, total_time/1000000, (double)(total_time/(NUMLOOPS*5)));
    return 0;
}
