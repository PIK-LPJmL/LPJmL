
### ------------------------------------------------------------------------------ ###

README: how to run LPJmL_ggcmi_phase3

### ------------------------------------------------------------------------------ ###


- prepare sowing and maturity dates input in 24 band .bin format, use R script 'convert_crop_calendar_into_bin_for_LPJmL.R'

- calculate PHU requirements based on crop calendar and climate inputs:
 	- go to 'LPJROOT/calc_PHU/' and specify details in 'calculate_phu_input_for_lpjml.R' and 'calculate_phu_input_for_lpjml.sh'
	- 'calculate_phu_input_for_lpjml.R' contains a replica of the phenology module in LPJmL and calculates the average required heat units based on the climate input and sowing and maturity dates provided
	- make sure the inputs are the same as in the planned LPJmL runs and that the simulation periods are correctly specified
	- use 'calculate_phu_input_for_lpjml.sh' to submit the R script as cluster job

- compile code:
	- configure makefile with './configure.sh -DDOUBLE_HARVEST'
	- compile

- create LPJmL restart file:
	- make output directory outside of LPJROOT
	- specify 'LPJmL_phase3a_submit' and 'LPJmL_phase3b_submit' shell scripts
	- set 'sim="CREATE_RESTART" ' to create a restart file
	- define 'restartsetup="nv"' if run is without river routing, otherwise 'restartsetup="nv_rr"'

- run LPJmL_ggcmi_phase3: 
	- specify 'LPJmL_phase3a_submit' and 'LPJmL_phase3b_submit' shell scripts
	- set 'sim="FROM_RESTART" ' to read from restart file

- post-processing to correct for double harvest and create required output formats:
	- use 'ggcmi_phase3_postprocessing.R'