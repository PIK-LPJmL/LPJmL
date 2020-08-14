#!/bin/sh

#scriptdir=/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/SCRIPTS/Compute_GPs_PHUs/calc_PHU
scriptdir=/home/minoli/LPJmL_GitLab/macmit_intensification/calc_PHU
climates=( "HadGEM2" "GFDL" "IPSL" "MIROC5" )
scens=(1986 2060 2080)

for ((c=0;c<4;c+=1)) # climate
do
for ((s=2;s<3;s+=1)) # scen
do

  cat <<EOF >$scriptdir/tmp/calculate_PHU_requirements_fix_hdate_${climates[c]}_${scens[s]}.jcf
#!/bin/bash
#SBATCH --qos=standby
#SBATCH --export=ALL
#SBATCH --mail-user=minoli@pik-potsdam.de
#SBATCH --mail-type=end
#SBATCH --time=120
#SBATCH --tasks-per-node=16
#SBATCH --output=$scriptdir/tmp/calculate_PHU_requirements_fix_hdate_${climates[c]}_${scens[s]}.out
#SBATCH --error=$scriptdir/tmp/calculate_PHU_requirements_fix_hdate${climates[c]}_${scens[s]}.err
#SBATCH --account=macmit
#SBATCH --job-name=PHU

Rscript $scriptdir/calculate_phu_input_for_lpjml_fix_hdate.R ${c}_${s}
EOF

  sbatch $scriptdir/tmp/calculate_PHU_requirements_fix_hdate_${climates[c]}_${scens[s]}.jcf ;

done
done
