#!/bin/sh

variables=("yield" "pirnreq" "plantday" "plantyear" "matyday" "harvyear" "soilmoist1m")
ext="ssp585_default" # c("picontrol","historical","ssp126","ssp585")

scriptdir="/p/projects/macmit/users/jaegermeyr/GGCMI_phase3/processed"

for ((v=0;v<7;v+=1))
do

  cat <<EOF >$scriptdir/tmp/lpjml_ggcmi_phase3_processing_${variables[v]}_${ext}.jcf
#!/bin/bash
#SBATCH --qos=short
#SBATCH --export=ALL
#SBATCH --mail-user=jonasjae@pik-potsdam.de
#SBATCH --mail-type=end
#SBATCH --time=360
#SBATCH --tasks-per-node=16
#SBATCH --output=$scriptdir/tmp/lpjml_ggcmi_phase3_processing_${variables[v]}_${ext}.out
#SBATCH --error=$scriptdir/tmp/lpjml_ggcmi_phase3_processing_${variables[v]}_${ext}.err
#SBATCH --account=macmit
#SBATCH --job-name=GGCMI

Rscript $scriptdir/lpjml_ggcmi_output_processing_phase3b.R ${variables[v]}
EOF

  sbatch $scriptdir/tmp/lpjml_ggcmi_phase3_processing_${variables[v]}_${ext}.jcf ;

done