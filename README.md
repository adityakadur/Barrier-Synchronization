OpenMPI installation instructions:  

http://www.sysads.co.uk/2014/05/install-open-mpi-1-8-ubuntu-14-04-13-10/  
(Can download latest stable release from open-mpi.org)  

When I restarted I had to install:  
sudo apt-get install libopenmpi-dev  
sudo apt-get install openmpi-bin  

OpenMPI compilation instructions:  
mpicc tournament-mpi.c -lm [-DDEBUG]  
mpicc dissemination-mpi.c -lm [-DDEBUG]  
  
-lm is for math.h  
-DDEBUG is for printing debug statements  

To run an OpenMPI program:  
mpirun -np 4 a.out [> output_file]  
  
The test.py checks the values and the timestamps printed after each barrier. Look at the comments at the top of the file for printf format to use.  

To run test.py:  
python test.py output_file num_processes num_iterations_of_barrier



