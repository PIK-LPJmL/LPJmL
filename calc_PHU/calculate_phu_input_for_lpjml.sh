#!/bin/sh

#workdir=/p/projects/macmit/users/jaegermeyr/LPJmL-PHU/calc_PHU

climates=( "agmerra" "gswp3-w5e5" )
myemail="jonasjae@pik-potsdam.de"

for ((c=0;c<1;c+=1)) # climate
do

  cat <<EOF >tmp/calculate_PHU_requirements_${climates[c]}.jcf
#!/bin/bash
#SBATCH --qos=short
#SBATCH --export=ALL
#SBATCH --mail-user=${myemail}
#SBATCH --mail-type=end
#SBATCH --time=120
#SBATCH --tasks-per-node=16
#SBATCH --output=tmp/calculate_PHU_requirements_${climates[c]}.out
#SBATCH --error=tmp/calculate_PHU_requirements_${climates[c]}.err
#SBATCH --account=macmit
#SBATCH --job-name=PHU

Rscript calculate_PHU_requirements.R ${climates[c]}
EOF

  sbatch tmp/calculate_PHU_requirements_${climates[c]}.jcf ;

done
