#!/bin/bash

#spinup_runs=('01 03 04 05 06')      # create restarts
spinup_runs=('')

#hist_runs=('01 02 03 04 05 06 07 08 09 10')
hist_runs=('')

#future_runs=('11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26')
future_runs=('11')

# runids=('01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26')
# runids=('01 02')             # WFDI
# runids=('03 07 11 15 19 23') # HadGEM2-ES
# runids=('04 08 12 16 20 24') # GFDL-ESM2M
# runids=('05 09 13 17 21 25') # IPSL-CM5A-LR 
# runids=('06 10 14 18 22 26') # MIROC5 



# DO SPINUP RUNS
#------------------------------#
for j in $spinup_runs;
do

lpjsubmit -group macmit -wtime 04:00:00 -blocking 16 512 -DRUN_ID_${j} lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -wtime 00:10:00 1 -DRUN_ID_${j} lpjml_macmit_intensification.js

echo "submitted run id: ${j}"

done
wait



# DO HISTORICAL RUNS
#------------------------------#
for i in $hist_runs;
do

lpjsubmit -class standby -group macmit -wtime 12:00:00 -o lpjml.${i}.%j.out -e lpjml.${i}.%j.err -blocking 16 512 -DRUN_ID_${i} -DFROM_RESTART lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -wtime 00:10:00 1 -DRUN_ID_${i} -DFROM_RESTART lpjml_macmit_intensification.js

echo "submitted run id: ${i}"

done
wait


# DO FUTURE RUNS
#------------------------------#
for ii in $future_runs;
do

lpjsubmit -class standby -group macmit -wtime 04:00:00 -o lpjml.${ii}.%j.out -e lpjml.${ii}.%j.err -blocking 16 512 -DRUN_ID_${ii} -DFROM_RESTART lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -wtime 00:10:00 1 -DRUN_ID_${ii} -DFROM_RESTART lpjml_macmit_intensification.js

echo "submitted run id: ${ii}"

done






# WFDI        #######
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_01 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_02 lpjml_macmit_intensification.js

# HadGEM2-ES  #######
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_03 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_07 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_11 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_15 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_19 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_23 lpjml_macmit_intensification.js

# GFDL-ESM2M  #######
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_04 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_08 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_12 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_16 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_20 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_24 lpjml_macmit_intensification.js

# IPSL-CM5A-LR #######
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_05 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_09 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_13 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_17 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_21 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_25 lpjml_macmit_intensification.js

# MIROC5      #######
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_06 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_10 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_14 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_18 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_22 lpjml_macmit_intensification.js
#lpjsubmit -class standby -group macmit -blocking 16 512 -DRUN_ID_26 lpjml_macmit_intensification.js

