#PBS -q cs6210
#PBS -l nodes=12:sixcore
#PBS -l walltime=00:20:00
#PBS -N AK-MPI
OMPI_MCA_mpi_yield_when_idle=0
cat $PBS_NODEFILE > tmp
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 2 ./mpi/mpi-tournament 1000000 > op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 3 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 4 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 5 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 6 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 7 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 8 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 9 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 10 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 11 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 12 ./mpi/mpi-tournament 1000000 >> op-mpi
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 16 ./mpi/mpi-tournament 1000000 >> op-mpi
