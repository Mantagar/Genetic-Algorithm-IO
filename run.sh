function createScript()
{
  cat template.sh | sed -e 's/NODES/'$1'/g' | sed -e 's/MODE/'$2'/g' | sed -e 's/CPUS/'$3'/g' | sed -e 's/MUTATION_PROB/'$4'/g' > script.sh
}

#module load plgrid/tools/python/3.6.5
#for cpus in 120 240 360 480 600 720 840 960 1080 1200
#do
#  python3.6 topologies/topology.py $cpus > "topologies/$cpus"
#done


#12 cpu per node
for mode in 'full' 'topology' 'ring' 'torus'; do
  for cpus in 120 240 360 480 600 720 840 960 1080 1200; do
    for prob in '0.0001' '0.0003' '0.001' '0.003' '0.01'; do
      createScript $((cpus/12)) $mode $cpus $prob
      sbatch script.sh
    done
  done
done

rm script.sh
