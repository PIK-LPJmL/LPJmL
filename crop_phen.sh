#!/bin/bash

# spinup_runs=('01 08 15 22')
spinup_runs=('')

# historical_runs=('01 08 15 22')
historical_runs=('')

# future_runs=('02 03 04 05 06 07 09 10 11 12 13 14 16 17 18 19 20 21 23 24 25 26 27 28 29 30 31 32 33 34 35 36')
future_runs=('02 03 04 05 06 07 09 10 11 12 13 14 16 17 18 19 20 21 23 24 25 26 27 28')
# future_runs=('29 30 31 32 33 34 35 36') # Runs with fixed sdate or hdate

# runids=('01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36')


# DO SPINUP RUNS
#------------------------------#
for j in $spinup_runs;
do

lpjsubmit -class standby -group macmit -wtime 04:00:00 -blocking 16 256 -DRUN_ID_${j} lpjml_crop_phen.js

echo "submitted run id: ${j}"

done
wait



# DO HISTORICAL RUNS
#------------------------------#
for i in $historical_runs;
do

lpjsubmit -class standby -group macmit -wtime 05:00:00 -o lpjml.${i}.%j.out -e lpjml.${i}.%j.err -blocking 16 256 -DRUN_ID_${i} -DFROM_RESTART lpjml_crop_phen.js

echo "submitted run id: ${i}"

done
wait


# DO FUTURE RUNS
#------------------------------#
for ii in $future_runs;
do

lpjsubmit -class standby -group macmit -wtime 02:00:00 -o lpjml.${ii}.%j.out -e lpjml.${ii}.%j.err -blocking 16 256 -DRUN_ID_${ii} -DFROM_RESTART lpjml_crop_phen.js

echo "submitted run id: ${ii}"

done







