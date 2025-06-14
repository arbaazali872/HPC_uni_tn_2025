#!/bin/bash
#PBS -N matmul_openmp_job
#PBS -l select=1:ncpus=16:mem=16gb
#PBS -l walltime=1:00:00
#PBS -q short_cpuQ
#PBS -o job_output.out
#PBS -e job_error.err

# Load OpenMP and math libraries (adjust as per cluster's module system)
module load mpich-3.2 # Load the required module
module load openmpi  # Or whatever your system uses

# Set number of OpenMP threads
export OMP_NUM_THREADS=8
echo "Using OMP_NUM_THREADS=$OMP_NUM_THREADS"

# Move to the directory where the job was submitted
cd $PBS_O_WORKDIR

# Compile the OpenMP matrix multiplication code
mpicc -O3 -fopenmp -std=c99 "$src_file" -o "$exe_file" -lm
# Run the executable
./matmul_openmpi

# we have three main things from here: output, logs, fold