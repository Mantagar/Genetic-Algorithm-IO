#!/bin/bash -l
#SBATCH -N NODES
#SBATCH --ntasks-per-node=12
#SBATCH --time=01:00:00 
#SBATCH -A io2018
#SBATCH -p plgrid
#SBATCH --output="logs/MODE_CPUS.out"

srun /bin/hostname
 
module add plgrid/tools/openmpi/1.10.1-gcc-4.9.2
 
cd $SLURM_SUBMIT_DIR

case MODE in
  full)
    mpiexec ./genetic.out full
    ;;
  topology)
    mpiexec ./genetic.out topology topologies/CPUS
    ;;
  ring)
    mpiexec ./genetic.out ring
    ;;
  torus)
    mpiexec ./genetic.out torus 12
    ;;
esac
