#!/bin/bash -l
#SBATCH -N 50
#SBATCH --ntasks-per-node=1
#SBATCH --time=01:00:00 
#SBATCH -A plgmantagar2018a
#SBATCH -p plgrid
#SBATCH --output="50.out" 

srun /bin/hostname
 
module add plgrid/tools/openmpi/1.10.0-gnu-4.9.2-ib
 
cd $SLURM_SUBMIT_DIR

mpiexec ./genetic.out topology topologies/50
