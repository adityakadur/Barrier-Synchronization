#! /bin/bash
/opt/openmpi-1.4.3-gcc44/bin/mpicc -o mpi-tournament tournament-mpi.c -lm
/opt/openmpi-1.4.3-gcc44/bin/mpicc -o mpi-dissemination dissemination-mpi.c -lm
