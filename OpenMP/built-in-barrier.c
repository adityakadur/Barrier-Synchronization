#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int P, NUMLOOPS;

int main(int argc, char* argv[])
{
    if (argc==3){
        if (sscanf (argv[1], "%d", &P)!=1) printf ("P - not an integer\n");
        if (sscanf (argv[2], "%d", &NUMLOOPS)!=1) printf ("N - not an integer\n");
    }
    else {P = 4; NUMLOOPS = 1000;}
    printf("\nBuilt-in OpenMP barrier\n"
            "--------------------------------------------\n"
            "Number of threads = %d\n", P);
    double total_time;
    struct timeval tv1, tv2;
    int j;
    #pragma omp parallel num_threads(P) shared(tv1,tv2) firstprivate(j)
    {
        gettimeofday(&tv1, NULL);
        for (j=0;j<NUMLOOPS;j++){
            #pragma omp barrier
            #pragma omp barrier
            #pragma omp barrier
            #pragma omp barrier
            #pragma omp barrier
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
