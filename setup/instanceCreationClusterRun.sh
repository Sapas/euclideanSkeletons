#!/bin/bash
#SBATCH --job-name=bigRunNormalDist
#SBATCH --time=48:00:00
#SBATCH --mem=2G
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --array=2-100



# Load the required modules
module load gcc/10.2.0
module load cplex/12.8
module load cmake/3.18.4

# Call run, this one has 990 jobs
cpp_code/skeleton graphCreation $((${SLURM_ARRAY_TASK_ID} * 25)) $((${SLURM_ARRAY_TASK_ID} * 25)) 10 1 50 uniform true
cpp_code/skeleton graphCreation $((${SLURM_ARRAY_TASK_ID} * 25)) $((${SLURM_ARRAY_TASK_ID} * 25)) 10 1 50 normal true
