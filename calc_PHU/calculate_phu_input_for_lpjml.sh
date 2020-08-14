#!/bin/sh

#scriptdir=/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/SCRIPTS/Compute_GPs_PHUs/calc_PHU
scriptdir=/home/minoli/LPJmL_GitLab/macmit_intensification/calc_PHU
climates=( "HadGEM2" "GFDL" "IPSL" "MIROC5" )
scens=(1986 2060 2080)

climates=( "agmerra" "wfdei" "gswp3-w5e5" "gfdl-esm4" "ipsl-cm6a-lr" "mpi-esm1-2-hr" "mri-esm2-0" "ukesm1-0-ll")
myemail="jonasjae@pik-potsdam.de"

for ((c=4;c<5;c+=1)) # climate
do

  cat <<EOF >tmp/calculate_phu_input_for_lpjml_${climates[c]}.jcf
#!/bin/bash
#SBATCH --partition=priority
#SBATCH --qos=standby
#SBATCH --export=ALL
#SBATCH --mail-user=minoli@pik-potsdam.de
#SBATCH --mail-type=end
#SBATCH --time=120
#SBATCH --tasks-per-node=16
#SBATCH --output=tmp/calculate_phu_input_for_lpjml_${climates[c]}.out
#SBATCH --error=tmp/calculate_phu_input_for_lpjml_${climates[c]}.err
#SBATCH --account=macmit
#SBATCH --job-name=PHU

Rscript calculate_phu_input_for_lpjml.R ${climates[c]}
EOF

  sbatch tmp/calculate_phu_input_for_lpjml_${climates[c]}.jcf ;

done
done
