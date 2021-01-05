# This script calculates an exponential weighted moving average of monthly temperature for the year 2000
print("calculate average temperature")

### ---------------------------------------------------------------------------------------------- ###
# calculate arithmetic mean 1980-2009
### ---------------------------------------------------------------------------------------------- ###

print(paste0("calculating arithmetic mean daily temperature ",simulation_period[1],"-",simulation_period[2]," ..."))

zz <- file(path.tas,"rb")
seek(zz,where=43+(simulation_period[1]-climate_start)*(365*ncell)*2,origin="start")

tmp <- array(0,dim=c(365,ncell))
for (k in 1:nyears) {
      read <- readBin(zz,integer(),n=365*ncell,size=2)
      read <- array(read,dim=c(365,ncell))
      tmp <- tmp+read
}
close(zz)
tmp_mean_day=tmp/nyears # mean over nyears
tmp_mean_day <- tmp_mean_day/10 # To convert from (°Cx10) to (°C)

tmp_mean_day=t(tmp_mean_day)

tmp_mean_day=round(tmp_mean_day,d=2)

# Write binary file for faster read in
zz <- file(fn_tas_day,"wb")
writeBin(as.vector(tmp_mean_day),zz,size=4)
close(zz)

### ---------------------------------------------------------------------------------------------- ###
# Calculate exponentially weigthed annual mean temperature for year 1980
# (to mimic LPJmL calculation)
### ---------------------------------------------------------------------------------------------- ###

# print(paste0("calculating exponentially weighted mean ",climbuf[1],"-",climbuf[2]," ..."))

# alpha<- 0.05 # parameter weighting average
# years_exp=c(climbuf[1],climbuf[2])
# nyears_exp<- years_exp[2]-years_exp[1]+1

# zz <- file(path.tas,"rb")
# seek(zz,where=43+(years_exp[1]-climate_start)*(365*ncell)*2,origin="start")
# tmp <- array(0,dim=c(ncell,nyears_exp))
# for (k in 1:nyears_exp) {
#       file <- readBin(zz,integer(),n=365*ncell,size=2)
#       file <- array(file,dim=c(365,ncell))
#       atemp=apply(file,2,sum)/365 # mean annual
#       tmp[,k] <- atemp
# }
# close(zz)
# tmp <- tmp/10 # To convert from (°Cx10) to (°C)

# average_tmp<- array(0,dim=c(ncell,nyears_exp))# array mean of monthly temperatures for each year (weighted calculation)

# for (i in 1:ncell) {
#         x <- tmp[i,1] # initialize formula with data from first year
#             for (y in 1:nyears_exp) {
#                   average_tmp[i,y] <- alpha * tmp[i,y] + (1-alpha) * x
#                   x <- average_tmp[i,y] # x is the historic exponential average used in next year
#             }
# }
# average_tmp_1980=round(average_tmp[,nyears_exp],d=2)

# # Write binary file for faster read in
# zz <- file(paste0(path.in,"tas_average_exp_",period,"_",climbuf[2],"_",climate_data,".clm"),"wb")
# writeBin(as.vector(average_tmp_1980),zz,size=4)
# close(zz)

# rm(average_tmp,average_tmp_1980,tmp)

### ---------------------------------------------------------------------------------------------- ###
# save daily temperature 1980-2009
### ---------------------------------------------------------------------------------------------- ###

# # read binary file
# zz <- file(path.tas,"rb")
# seek(zz,where=43+(simulation_period[1]-climate_start)*(365*ncell)*2,origin="start")

# # Write binary file for faster read in
# wb <- file(paste0(path.in,"tas_daily_",simulation_period[1],"_",simulation_period[2],"_",climate_data,".clm"),"wb")

# for (k in 1:nyears) {
#       read <- readBin(zz,integer(),n=365*ncell,size=2)
#       tmp <- t(array(read,dim=c(365,ncell)))
#       tmp <- round(tmp/10,d=2) # To convert from (°Cx10) to (°C)
#       writeBin(as.vector(tmp),wb,size=4)
# }
# close(zz)
# close(wb)

### ---------------------------------------------------------------------------------------------- ###
# calculate average monthl tmp

tmp_mean_month=array(0,c(ncell,12))

for(i in 1:ncell) {
    tmp_mean_month[i,1]<-mean(tmp_mean_day[i,1:31],na.rm=T)
    tmp_mean_month[i,2]<-mean(tmp_mean_day[i,32:59],na.rm=T)
    tmp_mean_month[i,3]<-mean(tmp_mean_day[i,60:90],na.rm=T)
    tmp_mean_month[i,4]<-mean(tmp_mean_day[i,91:120],na.rm=T)
    tmp_mean_month[i,5]<-mean(tmp_mean_day[i,121:151],na.rm=T)
    tmp_mean_month[i,6]<-mean(tmp_mean_day[i,152:181],na.rm=T)
    tmp_mean_month[i,7]<-mean(tmp_mean_day[i,182:212],na.rm=T)
    tmp_mean_month[i,8]<-mean(tmp_mean_day[i,213:243],na.rm=T)
    tmp_mean_month[i,9]<-mean(tmp_mean_day[i,244:273],na.rm=T)
    tmp_mean_month[i,10]<-mean(tmp_mean_day[i,274:304],na.rm=T)
    tmp_mean_month[i,11]<-mean(tmp_mean_day[i,305:334],na.rm=T)
    tmp_mean_month[i,12]<-mean(tmp_mean_day[i,335:365],na.rm=T)
}

### ---------------------------------------------------------------------------------------------- ###

tmp_mean_month=round(tmp_mean_month,d=2)

# Write binary file for faster read in
zz <- file(fn_tas_month,"wb")
writeBin(as.vector(tmp_mean_month),zz,size=4)
close(zz)

rm(tmp,tmp_mean_day,tmp_mean_month,zz)

### ---------------------------------------------------------------------------------------------- ###
