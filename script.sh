#!/bin/bash -l
#SBATCH -J GeneticTest
#SBATCH -N 8
#SBATCH --ntasks-per-node=8
#SBATCH --time=00:10:00 
#SBATCH -A plgmantagar2018a
#SBATCH -p plgrid-short
#SBATCH --output="output.out" 

srun /bin/hostname
 
module add plgrid/tools/openmpi/1.10.0-gnu-4.9.2-ib
 
cd $SLURM_SUBMIT_DIR
 
mpiexec ./genetic.out 1000 25 0.01 2
