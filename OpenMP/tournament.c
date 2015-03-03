#include <stdio.h>
#include <math.h>
#include<stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define DROPOUT 0
#define LOSER 1
#define WINNER 2
#define BYE 3
#define CHAMPION 4

#define P 2
#define NUMLOOPS 100

typedef struct _record{
    unsigned int flag;
    unsigned int role;
    unsigned int opp;
}record;

int count = P;
int sense = 1;
record* rounds;

void barrier_init(){
    rounds = malloc(P*(int)(log2(2*P-1)+1)*sizeof(record));
    
}

void barrier(int *my_sense, int *old_count){
        *my_sense = 1-*my_sense;
        #pragma omp critical
        {
            *old_count = count;
            count -= 1;
        }
        if (*old_count == 1)
        {
            count = P;
            sense = 1-sense;
        }
        else while(sense != *my_sense);
        
    }
int main()
{
    barrier_init();
    printf("%d, %d \n",P*(int)(log2(2*P-1)+1), P*(int)(log2(2*P-1)+1)*sizeof(record));
    return 0;
}

