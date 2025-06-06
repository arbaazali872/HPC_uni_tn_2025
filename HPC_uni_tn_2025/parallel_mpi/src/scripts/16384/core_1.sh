#!/bin/bash
#PBS -N matmul_parallel_mpi
#PBS -l select=1:ncpus=1:mem=16gb:mpiprocs=1
#PBS -l walltime=1:00:00
#PBS -q short_cpuQ

# ---- Configurable Section ----
src_file="matmul_parallel_mpi.c"
dim=16384        # Optional: only used if you want to pass matrix size dynamically
procs=1

# Extract base name
base_name=$(basename "$src_file" .c)

# Output files
#PBS -o ${base_name}_${dim}_${procs}_output.out
#PBS -e ${base_name}_${dim}_${procs}_error.log

# ---- Load Modules ----
module load mpich-3.2


# ---- Paths ----
project_root="/home/saad.hussainshafi/HPC_uni_tn_2025/HPC_uni_tn_2025"
src_dir="$project_root/parallel_mpi/src/main"
bin_dir="$project_root/parallel_mpi/bin/mpi/$dim"
log_dir="$project_root/parallel_mpi/logs/mpi/$dim"

mkdir -p "$bin_dir"
mkdir -p "$log_dir"

cd "$src_dir"

# ---- Compilation ----
exe_file="$bin_dir/${base_name}_${dim}_${procs}"

mpicc -O3 -std=c99 -DMATRIX_SIZE=$dim "$src_dir/$src_file" -o "$exe_file" -lm
if [ ! -f "$exe_file" ]; then
    echo "Compilation failed."
    exit 1
fi

# ---- Execution ----
cd "$bin_dir"
mpiexec -np $procs "$exe_file" > "$log_dir/${base_name}_${dim}_${procs}_output.log" \
                               2> "$log_dir/${base_name}_${dim}_${procs}_error.log"
