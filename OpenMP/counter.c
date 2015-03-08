#include <stdio.h>
#include <omp.h>
#include <sys/time.h>

int P, NUMLOOPS, count;
int sense = 1;

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
int main(int argc, char* argv[])
{
    if (argc==3){
        if (sscanf (argv[1], "%d", &P)!=1) printf ("P - not an integer\n");
        if (sscanf (argv[2], "%d", &NUMLOOPS)!=1) printf ("N - not an integer\n");
    }
    else {P = 4; NUMLOOPS = 1000;}
    count = P;

    printf("\nCentral counter barrier with sense-reversal\n"
            "--------------------------------------------\n"
            "Number of threads = %d\n", P);
    double total_time;
    struct timeval tv1, tv2;
    int my_sense = 1;
    int old_count;
    int j;
    
    #pragma omp parallel num_threads(P) shared(count, sense, tv1, tv2, total_time) firstprivate(my_sense, old_count, j)
    {
        gettimeofday(&tv1, NULL);
        for (j=0; j<NUMLOOPS; j++){
            
            barrier(&my_sense,&old_count);
            barrier(&my_sense,&old_count);
            barrier(&my_sense,&old_count);
            barrier(&my_sense,&old_count);
            barrier(&my_sense,&old_count);
        }
        gettimeofday(&tv2, NULL);
    }
    total_time = (double) (tv2.tv_usec - tv1.tv_usec) + (double) (tv2.tv_sec - tv1.tv_sec)*1000000;
    printf("\nSUMMARY:\nTotal run-time for %d "
            "loops with 5 barriers per loop: %fs\n"
            "The average time per barrier: %fus\n",
            NUMLOOPS, total_time/1000000, (double)(total_time/(NUMLOOPS*5)));
}


