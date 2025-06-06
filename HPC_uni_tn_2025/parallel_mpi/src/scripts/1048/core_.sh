#!/bin/bash
#PBS -N matmul_openmp_job
#PBS -l select=1:ncpus=8:mem=2gb -l place=pack:excl
#PBS -l walltime=5:00:00
#PBS -q short_cpuQ

# ---- Configurable section ----

src_file="matmul_8000_openmp.c"
dim=256

# Extract base name without extension
base_name=$(basename "$src_file" .c)

# Output log files
#PBS -o ${base_name}_${dim}_output.out
#PBS -e ${base_name}_${dim}_error.log

# ---- Load modules ----

module load mpich-3.2
module load openmpi

# ---- Paths ----

project_root="/home/saad.hussainshafi/HPC_uni_tn_2025/HPC_uni_tn_2025"
bin_dir="$project_root/parallel_mpi/bin/openmp/$dim"
log_dir="$project_root/parallel_mpi/logs/openmp/$dim"


mkdir -p "$bin_dir"
mkdir -p "$log_dir"

export OMP_NUM_THREADS=8
echo "Using OMP_NUM_THREADS=$OMP_NUM_THREADS"

# ---- Compilation ----

cd "$PBS_O_WORKDIR"
exe_file="$bin_dir/$base_name"

gcc -O3 -fopenmp -std=c99 "$src_file" -o "$exe_file" -lm
if [ ! -f "$exe_file" ];_]()
