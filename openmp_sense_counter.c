#include <stdio.h>
#include <omp.h>
#include <sys/time.h>

#define P 10

int count = P;
int sense = 1;
int old_count;

void barrier(int *my_sense){
        *my_sense = 1-*my_sense;
        #pragma omp critical
        {
            old_count = count;
            count -= 1;
        }
        if (old_count == 1)
        {
            count = P;
            sense = 1-sense;
        }
        else while(sense != *my_sense);
        
    }
int main()
{
    printf("Central counter barrier with sense-reversal\n");
    int N = 100;
    double total_time;
    struct timeval tv1, tv2;
    int my_sense = 1;
    int j;
    
    #pragma omp parallel num_threads(P) shared(count,sense,tv1,tv2,N,total_time) firstprivate(my_sense, old_count,j)
    {
        gettimeofday(&tv1, NULL);
        for (j=0;j<N;j++){
            
            barrier(&my_sense);
            barrier(&my_sense);
            barrier(&my_sense);
        }
        gettimeofday(&tv2, NULL);
    }
    total_time = (double) (tv2.tv_usec - tv1.tv_usec) + (double) (tv2.tv_sec - tv1.tv_sec)*1000000;
    printf("\nSUMMARY:\nNumber of threads = %d\nTotal run-time for %d "
            "loops with 3 barriers per loop: %f secs\n"
            "The average time per barrier: %f us\n",
            P, N, total_time/1000000, (double)(total_time/(N*3)));
}

