#! /bin/bash
/opt/openmpi-1.4.3-gcc44/bin/mpicc -o mpi-builtin builtin.c -lm
gcc -o omp-builtin builtin-omp.c -lm -fopenmp
