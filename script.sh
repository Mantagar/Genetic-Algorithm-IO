#!/bin/bash -l
#SBATCH -J Genetic
#SBATCH -N 5
#SBATCH --ntasks-per-node=12
#SBATCH --time=10:00:00 
#SBATCH -A plgmantagar2018a
#SBATCH -p plgrid-long
#SBATCH --output="output.out" 

srun /bin/hostname
 
module add plgrid/tools/openmpi/1.10.0-gnu-4.9.2-ib
 
cd $SLURM_SUBMIT_DIR
 
mpiexec ./genetic.out 2000 25 0.01 2
