
### ------------------------------------------------------------------------------ ###

README: how to run LPJmL_ggcmi_phase3

### ------------------------------------------------------------------------------ ###


1. Prepare sowing and maturity dates input in 30 band .bin format based on new CFT list, use R script 'convert_crop_calendar_into_bin_for_LPJmL.R' (request from jonasjae@pik-potsdam.de)

2. Calculate PHU requirements based on crop calendar and climate inputs:
 	- go to 'LPJROOT/calc_PHU/' and specify details in 'calculate_phu_input_for_lpjml.R' and 'calculate_phu_input_for_lpjml.sh'
	- 'calculate_phu_input_for_lpjml.R' contains a replica of the phenology module in LPJmL and calculates the average required heat units based on the climate input and sowing and maturity dates provided
	- make sure the inputs are the same as in the planned LPJmL runs and that the simulation periods are correctly specified
	- use 'calculate_phu_input_for_lpjml.sh' to submit the R script as cluster job (meaningful if done as a loop over various GCMs, RCP, etc.)

<<<<<<< HEAD
- compile code:
	- configure makefile with './configure.sh -DCROPSHEATFROST -DDOUBLE_HARVEST'
=======
3. Compile code:
	- configure makefile with: $ ./configure.sh -DDOUBLE_HARVEST
>>>>>>> 5a6c32743a60ec713a7f76a05245392645b0571a
	- compile

4. Create LPJmL restart file:
	- create output directory outside of LPJROOT
	- specify 'LPJmL_phase3a_submit' and 'LPJmL_phase3b_submit' shell scripts, including climate inputs, respecive start and end dates, respective PHU inputs, etc.
	- set 'sim="CREATE_RESTART" ' to create a restart file
	- define 'restartsetup="nv"' if running without river routing, otherwise define 'restartsetup="nv_rr"'

5. Run LPJmL_ggcmi_phase3: 
	- specify 'LPJmL_phase3a_submit' and 'LPJmL_phase3b_submit' shell scripts for runs starting from restart
	- set 'sim="FROM_RESTART" ' to read from restart file
	- specifiy CO2 scenario 'co2=("default" "2015co2")'
	- specify socio-economic scenario 'socioecon=("histsoc" "2015soc")'
	- check 'sdate_fixyear' value, i.e. the year after which vernalization requirements are held constant

6. Post-processing to correct for double harvest and create required output formats:
	- use 'ggcmi_phase3_postprocessing.R' (request from jonasjae@pik-potsdam.de)