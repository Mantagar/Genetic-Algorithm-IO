#!/bin/bash -l
#SBATCH -N NODES
#SBATCH --ntasks-per-node=12
#SBATCH --time=01:00:00 
#SBATCH -A io2018
#SBATCH -p plgrid
#SBATCH --output="logs/MODE_CPUS_MUTATION_PROB.out"

srun /bin/hostname
 
module add plgrid/tools/openmpi/1.10.1-gcc-4.9.2
 
cd $SLURM_SUBMIT_DIR

case MODE in
  full)
    mpiexec ./genetic.out MUTATION_PROB full
    ;;
  topology)
    mpiexec ./genetic.out MUTATION_PROB topology topologies/CPUS
    ;;
  ring)
    mpiexec ./genetic.out MUTATION_PROB ring
    ;;
  torus)
    mpiexec ./genetic.out MUTATION_PROB torus 12
    ;;
esac
