# This script calculates:
# maximum daylength at a location possibible i.e. 21 June northern hemisphere and 21 December southern hemisphere
# minimum daylength at a location possibible i.e. 21 December northern hemisphere and 21 June southern hemisphere
# differences between maximum and minimum
# optimum daylength for short day plants

print("Starting script: calculate_maximum_daylength.R")

# Assignments
res <- 0.5 # resolution in degrees
ndays<- 365 # number of days
deg2rad<- pi/180 # from degrees to radians

# Set arrays
max.daylength<- array(0,ncells) # daylength on 21 June (NH) and 21 Dec (SH)
min.daylength<- array(0,ncells) # daylength on 21 Dec (NH) and 21 June (SH)
diff.daylength<- array(0,ncells)
optimum.daylength<-array(0,ncells)

### -------------------------------------------------------------------------------------------------- ###

for(i in 1:ncells){

      if(grid[i,2] <= 0){
            day <- 355 # 21 Dec
      }
      if(grid[i,2] > 0){
            day <- 172 # 21 June
      }

      delta<- deg2rad*(-23.4*cos(2*pi*(day+10.0)/ndays))
      u <- sin(deg2rad*grid[i,2])*sin(delta)
      v <- cos(deg2rad*grid[i,2])*cos(delta)

      if(u >= v){
            max.daylength[i] <- 24
            min.daylength[i] <- 0
      }
      if(u <= -v){
            max.daylength[i] <- 24
            min.daylength[i] <- 0
      }
      if(u < v && u > -v){
            hh <- acos(-u/v)
            max.daylength[i]<- 24*hh*(1/pi)
            min.daylength[i]<- 24-(24*hh*(1/pi))
      }

      optimum.daylength[i]<- max(0,max.daylength[i] * (1-(1/(max.daylength[i] - min.daylength[i]))))
}

### -------------------------------------------------------------------------------------------------- ###
# Calculate day length
daylength.daily<-array(0,c(ncells,ndays))

for(i in 1:ncells) {
    for (k in 1:ndays) {
      delta <- deg2rad*(-23.4*cos(2*pi*(k+10.0)/365))
      u<- sin(deg2rad*grid[i,2])*sin(delta)
      v<- cos(deg2rad*grid[i,2])*cos(delta)

      if(u >= v) daylength.daily[i,k] <- 24

      if(u <= -v) daylength.daily[i,k] <- 0

      if(u < v && u > -v){
          hh  <- acos(-u/v)
          daylength.daily[i,k]<- 24*hh*(1/pi)
      }
    } # end of for each day
} # end for each cell

### -------------------------------------------------------------------------------------------------- ###
# Write binary files

max.daylength=round(max.daylength,d=2)
zz <- file(paste(path.in,"maximum_daylength.clm",sep=""),"wb")
writeBin(as.vector(max.daylength),zz,size=4)
close(zz)

min.daylength=round(min.daylength,d=2)
zz <- file(paste(path.in,"minimum_daylength.clm",sep=""),"wb")
writeBin(as.vector(min.daylength),zz,size=4)
close(zz)

difference_daylength=round(max.daylength - min.daylength,d=2)
zz <- file(paste(path.in,"difference_daylength.clm",sep=""),"wb")
writeBin(as.vector(difference_daylength),zz,size=4)
close(zz)

optimum.daylength=round(optimum.daylength,d=2)
zz <- file(paste(path.in,"optimum_photoperiod_short_day_plant.clm",sep=""),"wb")
writeBin(as.vector(optimum.daylength),zz,size=4)
close(zz)

daylength.daily=round(daylength.daily,d=2)
zz <- file(paste(path.in,"daylength_daily.clm",sep=""),"wb")
writeBin(as.vector(daylength.daily),zz,size=4)
close(zz)
