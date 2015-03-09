#PBS -q cs6210
#PBS -l nodes=1:fourcore
#PBS -l walltime=00:20:00
#PBS -N AK-OpenMP
./openmp/counter 2 1000000 > op-omp
./openmp/tournament 2 1000000 >> op-omp
./openmp/counter 3 1000000 >> op-omp
./openmp/tournament 3 1000000 >> op-omp
./openmp/counter 4 1000000 >> op-omp
./openmp/tournament 4 1000000 >> op-omp
./openmp/counter 5 1000000 >> op-omp
./openmp/tournament 5 1000000 >> op-omp
./openmp/counter 6 1000000 >> op-omp
./openmp/tournament 6 1000000 >> op-omp
./openmp/counter 7 1000000 >> op-omp
./openmp/tournament 7 1000000 >> op-omp
./openmp/counter 8 1000000 >> op-omp
./openmp/tournament 8 1000000 >> op-omp
