OpenMPI installation instructions:

http://www.sysads.co.uk/2014/05/install-open-mpi-1-8-ubuntu-14-04-13-10/
(Can download latest stable release from open-mpi.org)

When I restarted I had to install:
sudo apt-get install libopenmpi-dev
sudo apt-get install openmpi-bin

OpenMPI compilation instructions:
mpicc tournament-mpi.c -lm   (-lm is for math.h)

To run an OpenMPI program:
mpirun -np 4 a.out




