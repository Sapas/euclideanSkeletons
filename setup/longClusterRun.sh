#!/bin/bash
#SBATCH --job-name=2DsequentialKrig
#SBATCH --time=48:00:00
#SBATCH --mem=2G
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --array=1-964

# Load the required modules
module load gcccore/10.2.0
module load cmake/3.18.4
module load eigen/3.3.8
# Move into folder and run
cd ../cpp_code
# Call program with varying array (code knows how to deal with it)
# This one has 832 jobs
#./main modelPerformanceSetup ${SLURM_ARRAY_TASK_ID}
# This one has 848 jobs
#./main toalReproductionSetup ${SLURM_ARRAY_TASK_ID}
# This one has 926 jobs
#./main modelAccuracyBudget5d ${SLURM_ARRAY_TASK_ID}
# This is file with 86400 jobs
# Splitting it up into batches of 175 jobs with new call
# Will require 494 calls
#./main COCOinstanceFeatures ${SLURM_ARRAY_TASK_ID} 175
# This one has 964 jobs
#./main COCOinterestingFunctionsd1d5 ${SLURM_ARRAY_TASK_ID}
# This one has 944 jobs
#./main COCOinterestingFunctionsd10 ${SLURM_ARRAY_TASK_ID}
# This one has 964 jobs
./main COCOinterestingFunctionsd1d5budget10 ${SLURM_ARRAY_TASK_ID}



# Well I just deleted a bunch of run scripts, so these should be useless. Keeping them here for now just in case
#./main 2DsequentialKrigSetup ${SLURM_ARRAY_TASK_ID}
#./main toalModelAccuracySetup ${SLURM_ARRAY_TASK_ID}
# This one has 1260 jobs
#./main toalReproductionSetup ${SLURM_ARRAY_TASK_ID}
# This one has 70 jobs
#./main 2DsequentialKrigLitSetup ${SLURM_ARRAY_TASK_ID}
# This one has 3255 Jobs
# ./main toalReproductionCoKrigExtendedSetup ${SLURM_ARRAY_TASK_ID}
# This one has 210 jobs
#./main toalReproductionCoKrigPaciorekSetup ${SLURM_ARRAY_TASK_ID}
# This one has 525 jobs
#./main toalReproductionCoKrigTridExtendedSetup ${SLURM_ARRAY_TASK_ID}
# This one has 832 jobs
#./main modelPerformanceSetup ${SLURM_ARRAY_TASK_ID}
# This one has 848 jobs
#./main customSetup ${SLURM_ARRAY_TASK_ID}
# This one has 926 jobs

