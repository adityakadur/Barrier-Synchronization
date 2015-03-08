#! /bin/bash
gcc -o counter counter.c -fopenmp
gcc -o tournament tournament.c -lm -fopenmp
