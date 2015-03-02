#! /bin/bash

rm ./omp
gcc -o omp *.c -fopenmp
./omp
