rm(list=ls(all=TRUE))
gc()
args <- commandArgs(TRUE)

# args="gswp3-w5e5"
# args="agmerra"
# args="wfdei"

### ------------------------------------------------------------------------------------------------- ###

# specify working directory "LPJROOT/calc_PHU/"

wd="/p/projects/waterforce/jonas/diss/LPJmL/LPJmL_gitlab/LPJmL_ggcmi_phase3/calc_PHU/"

# specify output path for PHU file

path.out="/p/projects/lpjml/input/crop_calendar/"

### ------------------------------------------------------------------------------------------------- ###

crops <- c("wwh","swh","mai","ri1","ri2","soy","mil","sor","pea","sgb","cas","rap","sun","nut","sgc")

verncrops <- c("wwh","rap") # winter crops with vernalization requirements
verncrops_all <- c("wwh") # winter crops assumed in all grid cells

### ------------------------------------------------------------------------------------------------- ###

climate_forcings=c("agmerra","wfdei","gswp3-w5e5","gfdl-esm4","ipsl-cm6a-lr","mpi-esm1-2-hr","mri-esm2-0","ukesm1-0-ll") # make sure this list is identical to the shell script submitting the job
clim_arg=which(climate_forcings==args) # name of climate forcing data, passed by shell script
climate_data=climate_forcings[clim_arg] # dailyc limate inputs #temp_data climate_data

# path to climate forcing data (daily temperature)
clm_paths=c(
  paste0("/p/projects/lpjml/input/historical/GGCMI_phase1/AgMERRA/tas_agmerra_1980-2010.clm2"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/tas_wfdei_1979-2012.clm2"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/historical/ISIMIP3a/obsclim/GSWP3-W5E5/tas_gswp3-w5e5_obsclim_1901-2016.clm"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/scenarios/ISIMIP3b/historical/GFDL-ESM4/tas_gfdl-esm4_historical_1850-2014.clm"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/scenarios/ISIMIP3b/historical/IPSL-CM6A-LR/tas_ipsl-cm6a-lr_historical_1850-2014.clm"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/scenarios/ISIMIP3b/historical/MPI-ESM1-2-HR/tas_mpi-esm1-2-hr_historical_1850-2014.clm"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/scenarios/ISIMIP3b/historical/MRI-ESM2-0/tas_mri-esm2-0_historical_1850-2014.clm"), # header 43, 31 years, 365 bands, size 2
  paste0("/p/projects/lpjml/input/scenarios/ISIMIP3b/historical/UKESM1-0-LL/tas_ukesm1-0-ll_historical_1850-2014.clm") # header 43, 31 years, 365 bands, size 2
)
path.tas=clm_paths[clim_arg]

# specifiy time period for which PHUs are calculated 
startyear=c(1980,1979,1979,1979,1979,1979,1979,1979)[clim_arg] # list of start dates for the respective climate forcing data sets
endyear=c(2010,2012,2010,2010,2010,2010,2010,2010)[clim_arg] # list of end dates for the respective climate forcing data sets

# specify start and end of climate forcing data
climate_start=c(1980,1979,1901,1850,1850,1850,1850,1850)[clim_arg]
climate_end=c(2010,2012,2016,2014,2014,2014,2014,2014)[clim_arg]

### ------------------------------------------------------------------------------------------------- ###

# specify path to crop calendar in LPJmL format (.bin or .clm)
crop_calendar="ggcmi_phase3_v1.01" # name tag for output files

path.sowing="/p/projects/lpjml/input/crop_calendar/sdates_ggcmi_phase3_v1.01_67420.clm"
path.harvest="/p/projects/lpjml/input/crop_calendar/mdates_ggcmi_phase3_v1.01_67420.clm"

### ------------------------------------------------------------------------------------------------- ###

path.in <- paste0(wd,"src/")
path.tmp <- paste0(wd,"tmp/")

# specify path to paramter table, make sure this is updated with pft.js
path.par=paste0(path.in,"crop_pars_phase3.csv")

### ------------------------------------------------------------------------------------------------- ###

simulation_period=c(startyear,endyear)
nyears=length(simulation_period[1]:simulation_period[2])

fn_tas_month=paste0(path.in,"tas/tas_average_monthly_",simulation_period[1],"_",simulation_period[2],"_",climate_data,".clm")
fn_tas_day=paste0(path.in,"tas/tas_average_daily_",simulation_period[1],"_",simulation_period[2],"_",climate_data,".clm")

### ------------------------------------------------------------------------------------------------- ###
### ------------------------------------------------------------------------------------------------- ###
### ------------------------------------------------------------------------------------------------- ###
### ------------------------------------------------------------------------------------------------- ###

source("/p/projects/waterforce/jonas/R_functions/generic_functions.R")

ncell <- 67420 # number of cells
grid <- t(array(readBin(paste0(path.in,"grid.bin"),what=integer(),size=2,n=ncell*2),dim=c(2,ncell))/100) # lon = grid[,1], lat = grid[,2]

croppars <- read.csv(file=path.par,header=TRUE,sep=",",row.names=1,as.is=T)

### ------------------------------------------------------------------------------------------------- ###

print(paste("### ------------------------------------------------------------------------------------------- ###"))
print(paste0("Calculating PHU requirements for climate ",climate_data," and crop_calendar ",crop_calendar))
print(paste("### ------------------------------------------------------------------------------------------- ###"))

### ------------------------------------------------------------------------------------------------- ###
# calculate average temperature

if(!file.exists(fn_tas_day) || !file.exists(fn_tas_month)) source(paste0(path.in,"calculate_average_daily_temperature.R"))   # calculate average daily temperature 1980-2010 (this script is computationally expensive)

### ------------------------------------------------------------------------------------------------- ###

for(crop in crops) {

  print(paste0("Doing ",crop," ..."))
  if(crop %in% verncrops) verncrop=TRUE else verncrop=FALSE

  ### ------------------------------------------------------------------------------------------------- ###
  # read parameter table

  if(!crop%in%names(croppars)) print("Error: crop parameters not found!!!")
  basetemp <- as.numeric(croppars[rownames(croppars)=="basetemp",names(croppars)==crop])

  ### ------------------------------------------------------------------------------------------------- ###
  # read sowing and maturity dates

  source(paste0(path.in,"read_crop_calendar.R")) # creates sdate_rain, sdate_irr, hdate_rain, hdate_irr

  ### ------------------------------------------------------------------------------------------------- ###
  # get winter crop

  if(verncrop) source(paste0(path.in,"get_wintercrop.R")) # creates sdate_rain, sdate_irr, hdate_rain, hdate_irr

  ### ------------------------------------------------------------------------------------------------- ###
  # calculate vernalization requirements for winter crops

  if(verncrop) source(paste0(path.in,"calculate_vernalization_requirements.R"))

  ### ------------------------------------------------------------------------------------------------- ###
  # calculate HU requirements between sowing and harvest

  source(paste(path.in,"calc_phu.R",sep=""))

  if(verncrop) source(paste(path.in,"calc_phu_wintercrop.R",sep=""))

} # crop

### ------------------------------------------------------------------------------------------------- ###
print(paste("   "))
print(paste("### ------------------------------------------------------------------ ###"))
print(paste0("Finished calculating PHU requirements, creating LPJmL input file ..."))
print(paste("### ------------------------------------------------------------------ ###"))
print(paste("   "))

source(paste(path.in,"create_phu_input_file_for_lpjml.R",sep=""))

unlink(paste0(path.tmp,"PHU_",climate_data,"_",crop_calendar,"_*.txt"))

# unlink(paste0(path.tmp,"calculate_*.err"))
# unlink(paste0(path.tmp,"calculate_*.out"))
# unlink(paste0(path.tmp,"calculate_*.jcf"))

print("ready!!!")

