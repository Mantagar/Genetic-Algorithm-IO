function createScript()
{
  cat template.sh | sed -e 's/NODES/'$1'/g' | sed -e 's/TORUS/'$2'/g' | sed -e 's/TOPOLOGY/'$3'/g' > script.sh
}

#12 cpu per node
for cpus in 120 240 360 480 600
do
  createScript $((cpus/12)) 12 $cpus
  sbatch script.sh
done

rm script.sh
