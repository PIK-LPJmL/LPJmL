
rm(list=ls(all=TRUE))
gc()

require(ggplot2)
require(fields)
require(maptools)
args <- commandArgs(TRUE)

### ---------------------------------------------------------------------------------------------- ###

#wd="/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/SCRIPTS/Compute_GPs_PHUs/"
wd="/home/minoli/LPJmL_GitLab/macmit_intensification/"

path.in <- paste0(wd,"calc_PHU/src/")
path.tmp <- paste0(wd,"calc_PHU/tmp/")
path.out <- paste0("/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/")

### ---------------------------------------------------------------------------------------------- ###

crops=c("TEMPERATE_CEREALS","RICE","MAIZE","TROPICAL_CEREALS",
        "PULSES","TEMPERATE_ROOTS","TROPICAL_ROOTS","SUNFLOWER",
        "SOYBEAN","GROUNDNUT","RAPESEED","SUGARCANE")
verncrops <- c("TEMPERATE_CEREALS","RAPESEED")
verncrops_all <- c() #c("TEMPERATE_CEREALS","RAPESEED")

### ---------------------------------------------------------------------------------------------- ###

clim_arg=as.numeric(strsplit(args,"_")[[1]][1])+1
period_arg=as.numeric(strsplit(args,"_")[[1]][2])+1
#clim_arg=2
#period_arg=3
  
startyear=c(1986,2060,2080)[period_arg]
endyear=c(2005,2079,2099)[period_arg]
simulation_period=c(startyear,endyear)
nyears=length(simulation_period[1]:simulation_period[2])

# climate dataset
climate_data=c("HadGEM2-ES","GFDL-ESM2M","IPSL-CM5A-LR","MIROC5")[clim_arg] # dailyclimate inputs #temp_dataclimate_data

periods=c("historical","rcp60")

if (climate_data!="WFDEI") {

  if(startyear<2010) {
    period=periods[1]
    climate_start=1861
    climate_end=2005
  } else {
    period=periods[2]
    climate_start=2006
    climate_end=2099
  }
  
  path.tas=paste0("/p/projects/ikiimp/ISIMIP2b/input_CLM2/",climate_data,"/",period,"/tas_",climate_data,"_",period,"_",climate_start,"-",climate_end,".clm") # header 43,145 years, 365 bands, size 2
  
  
  path.sowing=paste0("/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/sdate_",climate_data,"_fix_sdate",".bin")
  path.harvest=paste0("/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/hdate_",climate_data,"_adapt_hdate",".bin")
  
} else {
  
  period=periods[1]
  climate_start=1979
  climate_end=2012
  
  path.tas=paste0("/p/projects/lpjml/input/historical/GGCMI_phase1/",climate_data,"/","tas_",tolower(climate_data),"_",climate_start,"-",climate_end,".clm2")
  
  path.sowing=paste0("/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/input/GGCMI_CTWN_planting_v1.25.clm2")
  path.harvest=paste0("/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/input/GGCMI_CTWN_harvest_v1.25.clm2")
  
} #climate_data

#climbuf=c(simulation_period[1]-30,simulation_period[1]) # for exponentially weighted tas mean

#crop_calendar=paste0(climate_data,"_",period,"_",startyear,"_",endyear)
crop_calendar=paste0(period,"_",startyear,"_",endyear)

### ---------------------------------------------------------------------------------------------- ###

ncell <- 67420 # number of cells
grid <- t(array(readBin(paste0(path.in,"grid.bin"),what=integer(),size=2,n=ncell*2),dim=c(2,ncell))/100) # lon = grid[,1], lat = grid[,2]
croppars <- read.csv(file=paste0(path.in,"crop_pars_crop_phen.csv"),header=TRUE,sep=",",row.names=1,as.is=T)


### ------------------------------------------------------------------------------------------------- ###
# files average temperature

fn_tas_month=paste0(path.in,"tas_average_monthly_",period,"_",simulation_period[1],"_",simulation_period[2],"_",climate_data,".clm")
fn_tas_day=paste0(path.in,"tas_average_daily_",period,"_",simulation_period[1],"_",simulation_period[2],"_",climate_data,".clm")


### ------------------------------------------------------------------------------------------------- ###
# main
### ------------------------------------------------------------------------------------------------- ###

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
