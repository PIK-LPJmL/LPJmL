#!/bin/sh

setups=("picontrol" "historical" "ssp126_2015co2" "ssp126_default" "ssp585_2015co2" "ssp585_default")
variables=("yield" "pirnreq" "plantday" "plantyear" "matyday" "harvyear" "soilmoist1m")

scriptdir="/p/projects/macmit/users/jaegermeyr/GGCMI_phase3/processed"

for ((s=1;s<6;s+=1))
do
for ((v=0;v<6;v+=1))
do

  cat <<EOF >$scriptdir/tmp/lpjml_ggcmi_phase3_processing_${setups[s]}_${variables[v]}.jcf
#!/bin/bash
#SBATCH --partition=priority
#SBATCH --qos=standby
#SBATCH --export=ALL
#SBATCH --mail-user=jonasjae@pik-potsdam.de
#SBATCH --mail-type=end
#SBATCH --time=360
#SBATCH --tasks-per-node=16
#SBATCH --output=$scriptdir/tmp/lpjml_ggcmi_phase3_processing_${setups[s]}_${variables[v]}.out
#SBATCH --error=$scriptdir/tmp/lpjml_ggcmi_phase3_processing_${setups[s]}_${variables[v]}.err
#SBATCH --account=macmit
#SBATCH --job-name=GGCMI

Rscript $scriptdir/lpjml_ggcmi_output_processing_phase3b.R ${s}_${v}
EOF

  sbatch $scriptdir/tmp/lpjml_ggcmi_phase3_processing_${setups[s]}_${variables[v]}.jcf ;

done
done