
# This script determines where winter crops should be grown
print("get_wintercrop")

lat=grid[,2]

# mean monthly temp
zz<- file(fn_tas_month,"rb")
tmp_mean_month <- readBin(zz, double(), n=ncell*12, size=4)
close(zz)
dim(tmp_mean_month) <- c(ncell,12)

# mean of single coldest month
cold_m_temp=array(0,ncell)
for(i in 1:ncell) cold_m_temp[i]=tmp_mean_month[i,order(tmp_mean_month[i,])][1]

### ------------------------ ###

gs_length <- function(pl,hv){
  gs=pl
  gs = hv - pl
  gs[which(gs<=0 & !is.na(gs))] <- gs[which(gs<=0 & !is.na(gs))]+365
  return(gs)
}

### ------------------------ ###

# tests if given season should be classified as winter crop
# takes a ratser file with planting dates in DoY
# this is the rule suggested by Portman et al. 2010, slightly changed in that <=7 instead of 6°C is used
# more details in "/p/projects/waterforce/jonas/R_functions/wintercrop.R"

wintercrop<-function(start,end) {

  cm=cold_m_temp # temp of coldest month 
  wc=start
  length=gs_length(start,end)

  wc[which(!is.na(start))]=0
  for(i in 1:length(wc)) {
    if(!is.na(start[i]) && start[i]>0 && !is.na(lat[i]) && !is.na(cm[i])) {
      if(lat[i]>0) {
        if(((start[i] + length[i] > 365) && (length[i] >= 150)) && (cm[i]>= -10 && cm[i]<= 7)) {
          wc[i]=1
        }
      } else {
        if(((start[i]<182) && (start[i] + length[i] > 182) && (length[i] >= 150)) && (cm[i]>= -10 && cm[i]<= 7)) {
          wc[i]=1
        }
      }
    }
  } # cell
  return(wc)
}

### ------------------------------------------------------------------------------------------------- ###

# rainfed
sdate_winter_rain=sdate_rain
hdate_winter_rain=hdate_rain

if(crop%in%verncrops_all) {
	wtype=array(1,ncell) # winter crop in all grid cells
} else {
	wtype=wintercrop(sdate_rain,hdate_rain)
}

sdate_winter_rain[which(wtype==0)]<-0
sdate_rain[which(wtype==1)]<-0
hdate_winter_rain[which(wtype==0)]<-0
hdate_rain[which(wtype==1)]<-0

# irrigated
sdate_winter_irr=sdate_irr
hdate_winter_irr=hdate_irr

if(crop%in%verncrops_all) {
	wtype=array(1,ncell)
} else {
	wtype=wintercrop(sdate_irr,hdate_irr)
}

sdate_winter_irr[which(wtype==0)]<-0
sdate_irr[which(wtype==1)]<-0
hdate_winter_irr[which(wtype==0)]<-0
hdate_irr[which(wtype==1)]<-0

### ------------------------------------------------------------------------------------------------- ###

rm(lat,tmp_mean_month,zz,cold_m_temp,wtype)
