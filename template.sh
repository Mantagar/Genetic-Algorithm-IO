#!/bin/bash -l
#SBATCH -N NODES
#SBATCH --ntasks-per-node=12
#SBATCH --time=01:00:00 
#SBATCH -A io2018
#SBATCH -p plgrid
#SBATCH --output="TOPOLOGY.out"

srun /bin/hostname
 
module add plgrid/tools/openmpi/1.10.1-gcc-4.9.2
 
cd $SLURM_SUBMIT_DIR

#mpiexec ./genetic.out full
#mpiexec ./genetic.out topology topologies/TOPOLOGY
#mpiexec ./genetic.out ring
mpiexec ./genetic.out torus TORUS
