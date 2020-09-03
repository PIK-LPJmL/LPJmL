
rm(list=ls(all=TRUE))
gc()
shellarg <- commandArgs(TRUE)
#shellarg <- "yield"
# ----------------------------------------- #

require(raster)
require(fields)
require(maps)
require(ncdf4)
require(foreach)
registerDoSEQ() # tells foreach to use sequential mode

# ----------------------------------------- #

NODATA <- 1e20
ncell <- 67420
sim.path <- "/p/projects/macmit/users/jaegermeyr/GGCMI_phase3/phase3a"
out.path <- "/p/projects/macmit/users/jaegermeyr/GGCMI_phase3/processed/phase3a"
#.path <- "/p/projects/macmit/users/cmueller/GGCMI_phase3/processed/phase3a"

# ----------------------------------------- #

for_eval <- TRUE
climate=c("gswp3-w5e5")

if(for_eval){
  start_years=c(1980)
  end_years=c(2010)
} else {
  start_years=c(1901)
  end_years=c(2016)
}
  first_years=c(1901)
  last_years=c(2016)

socioecon=c("histsoc","2015soc")[1]
co2=c("default","2015co2")[1]

simulated_crops <- c("wwh","swh","mai","ri1","ri2","soy","mil","sor","pea","sgb","cas","rap","sun","nut","sgc")
ncrops=length(simulated_crops)

irrigs <- c("noirr","firr")
bands <- c(1,2,3,4,5,6)
crops <- c("wwh","swh","mai","ri1","ri2","soy")
cropf <- c("winter_wheat","spring_wheat","maize","rice1","rice2","soy")

all_variables <- c("yield","pirnreq","plantday","plantyear","matyday","harvyear","soilmoist1m")
var_sel<- which(all_variables==shellarg) # indices of variables to be processed
variables <- c("yield","pirnreq","plantday","plantyear","matyday","harvyear","soilmoist1m")[var_sel]
units <- c("t ha-1 yr-1","mm yr-1","day of year","year","days since planting","year","mm")[var_sel]
longnames <- c("crop yields","potential irrigation requirements","actual planting date","planting year","days from planting to maturity","harvest year","soil water content")[var_sel]

hlimit=TRUE # sets yields to zero if achieved husum < 90% prescribed husum 

# ----------------------------------------- #
# functions
# ----------------------------------------- #

read.LPJmL.grid <- function(filename,npix,header,swap=F){
  ff <- file(filename,"rb")
  seek(ff,where=header,origin="start")
  if(swap){
    x <- readBin(ff,integer(),size=2,n=npix*2,endian="swap")/100
  } else {
    x <- readBin(ff,integer(),size=2,n=npix*2)/100
  }
  lon <- x[(1:npix)*2-1]
  lat <- x[(1:npix)*2]
  # ilon/ilat are row and column numbers, starting at 179.75W and 89.75S as 1/1
  ilon <- as.integer((lon+180)/0.5 + 1.01)
  ilat <- as.integer((lat+90)/0.5 + 1.01)
  area <- (111e3*0.5)*(111e3*0.5)*cos(lat/180*pi)/10000
  close(ff)
  data.frame(lon,lat,ilon,ilat,area)
}

read.LPJmL.array <- function(filename,fyear,syear,eyear,nbands,crop,npix){
  ff <- file(filename,"rb")
  data <- array(NA,dim=c(npix,(eyear-syear+1)))
  for(y in syear:eyear){
    seek(ff,where=((y-fyear)*nbands+(crop-1))*4*npix,origin="start")
    data[,(y-syear+1)] <- readBin(ff,double(),size=4,n=npix)
  }
  close(ff)
  data
}

read.LPJmL.array.int <- function(filename,fyear,syear,eyear,nbands,crop,npix){
  ff <- file(filename,"rb")
  data <- array(NA,dim=c(npix,(eyear-syear+1)))
  for(y in syear:eyear){
    seek(ff,where=((y-fyear)*nbands+(crop-1))*2*npix,origin="start")
    data[,(y-syear+1)] <- readBin(ff,integer(),size=2,n=npix)
  }
  close(ff)
  data
}

read.LPJmL.array.monthly <- function(filename,fyear,syear,eyear,nbands,crop,npix){
  ff <- file(filename,"rb")
  data <- array(NA,dim=c(npix,12*(eyear-syear+1)))
  seek(ff,where=(syear-fyear)*12*nbands*4*npix,origin="start")
  for(y in syear:eyear){
    read <- readBin(ff,double(),size=4,n=npix*nbands*12)
	dim(read)=c(npix,nbands,12)
	read=read[,crop,]
	data[,c(1:12)+((y-syear)*12)]=read
  }
  close(ff)
  rm(read)
  data
}

discard_first <- function(data,select,monthly=FALSE){
  if(monthly==TRUE) {
    data[select,c(1:(dim(data)[2]-12))] <- data[select,c(13:dim(data)[2])]
    data[select,c((dim(data)[2]-11):dim(data)[2])] <- 0
    data
  } else {
    data[select,c(1:(dim(data)[2]-1))] <- data[select,c(2:dim(data)[2])]
    data[select,dim(data)[2]] <- 0
    data
  }
}

# ----------------------------------------- #
# main
# ----------------------------------------- #

# load grid
grid <- read.LPJmL.grid("/p/projects/lpjml/input/ISIMIP3/grid.bin",ncell,43,swap=F)
res=0.5
raster_lons=seq(-180+(res/2),180-(res/2),0.5)
raster_lats=seq(-90+(res/2),90-(res/2),0.5)
nlat=length(raster_lats)
nlon=length(raster_lons)

# load phu requirement input
file="/p/projects/lpjml/input/crop_calendar/phu_gswp3-w5e5_1979-2010_ggcmi_phase3_v1.01_67420.clm"
ff <- file(file,"rb")
seek(ff,where=43,origin="start")
hu_ref <- readBin(ff,integer(),size=2,n=ncell*ncrops*2)
dim(hu_ref)<- c(ncrops*2,ncell)
hu_ref=t(hu_ref)
close(ff)

# ---------------------- #
# setup loop
# ---------------------- #

for(cl in 1:length(climate)) {

    print(" # ------------------------------------------- # ")
    print(paste(" # doing ", climate[cl]))
    print(" # ------------------------------------------- # ")

    start.year=start_years[cl]
    first.year=first_years[cl]
    end.year=end_years[cl]
    last.year=last_years[cl]
    nyear=length(start.year:end.year)

    # input
    data.path=paste(sim.path,"/lpjml_phase3_",climate[cl],"_",socioecon,"_",co2,"_",first.year,"_",last.year,sep="")

    # ------------------- #
    # irrig/rainfed loop
    # ------------------- #

    for(ir in 1:2) { # 1: "noirr" 2: "firr"

      if(!("pirnreq"%in%variables && ir==1)) {

        parloop <- foreach(cr=c(1:length(crops)), .errorhandling='pass', .verbose=FALSE, .export='message') %dopar% {

          print(paste("...",irrigs[ir],crops[cr]))

          # ---------------------------- #
          # reading lpjml bin data
          # ---------------------------- #

          syr <- read.LPJmL.array.int(paste(data.path,"/syear.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
          syr2 <- read.LPJmL.array.int(paste(data.path,"/syear2.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)

          var=syr
          var2=syr2

          hu <- read.LPJmL.array(paste(data.path,"/husum.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
          hu2 <- read.LPJmL.array(paste(data.path,"/husum2.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)

          if("yield"%in%variables) {
            var <- read.LPJmL.array(paste(data.path,"/pft_harvestc.bin",sep=""),first.year,start.year,end.year,38,if(ir>1) bands[cr]+19 else bands[cr],ncell)/0.45/100
            var2 <- read.LPJmL.array(paste(data.path,"/pft_harvestc2.bin",sep=""),first.year,start.year,end.year,38,if(ir>1) bands[cr]+19 else bands[cr],ncell)/0.45/100
          }
          if("plantyear"%in%variables) {
            var <- read.LPJmL.array.int(paste(data.path,"/syear.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
            var2 <- read.LPJmL.array.int(paste(data.path,"/syear2.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
          }
          if("matyday"%in%variables) {
            var <- read.LPJmL.array(paste(data.path,"/growing_period.bin",sep=""),first.year,start.year,end.year,34,if(ir>1) bands[cr]+17 else bands[cr],ncell)
            var2 <- read.LPJmL.array(paste(data.path,"/growing_period2.bin",sep=""),first.year,start.year,end.year,34,if(ir>1) bands[cr]+17 else bands[cr],ncell)
          }
          if("pirnreq"%in%variables){
            var <- read.LPJmL.array(paste(data.path,"/cft_airrig.bin",sep=""),first.year,start.year,end.year,38,if(ir>1) bands[cr]+19 else bands[cr],ncell)
            var2 <- read.LPJmL.array(paste(data.path,"/cft_airrig2.bin",sep=""),first.year,start.year,end.year,38,if(ir>1) bands[cr]+19 else bands[cr],ncell)
          }
          if("plantday"%in%variables) {
            var <- read.LPJmL.array.int(paste(data.path,"/sdate.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
            var2 <- read.LPJmL.array.int(paste(data.path,"/sdate2.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
          }
          if("harvyear"%in%variables) {      

            # calculate harvest year from syear, sdate, and growing period
            sd <- read.LPJmL.array.int(paste(data.path,"/sdate.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
            sd2 <- read.LPJmL.array.int(paste(data.path,"/sdate2.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)
            gp <- read.LPJmL.array(paste(data.path,"/growing_period.bin",sep=""),first.year,start.year,end.year,34,if(ir>1) bands[cr]+17 else bands[cr],ncell)
            gp2 <- read.LPJmL.array(paste(data.path,"/growing_period2.bin",sep=""),first.year,start.year,end.year,34,if(ir>1) bands[cr]+17 else bands[cr],ncell)

            hd=sd+gp
            hd2=sd2+gp2

            var=syr
            var2=syr2
            for(y in 1:dim(var)[2]) {
              var[which(hd[,y]>365),y]=syr[which(hd[,y]>365),y]+1
              var[which(hd[,y]>730),y]=syr[which(hd[,y]>730),y]+2
              var2[which(hd2[,y]>365),y]=syr2[which(hd2[,y]>365),y]+1
              var2[which(hd2[,y]>730),y]=syr2[which(hd2[,y]>730),y]+2
            }
            rm(sd,sd2,gp,gp2,hd,hd2)
          }

          if("soilmoist1m"%in%variables) smo <- read.LPJmL.array.monthly(paste(data.path,"/cft_mrootmoist.bin",sep=""),first.year,start.year,end.year,30,if(ir>1) bands[cr]+15 else bands[cr],ncell)

          # ------------------------------------------ #
          # fixing double harvests ####
          # ------------------------------------------ #

          testn <- which(rowSums(var2)>0)
          testn2 <- which(rowSums(syr2)>0) # just double checking
          if(length(testn)>0 & length(testn2)>0) if(length(which(testn%in%testn2))!=length(testn)) print(paste("WARNING! different set of double harvests in YLD2\n",paste(testn,collapse=" "),"\nand MDY\n",paste(testn2,collapse=" "),"\n"))
          rm(testn2)

          # loop across cells with double harvest
          for(nfix in testn){

            # extracting vectors of pixels with double harvests
            b.syr <- v.syr <- syr[nfix,]
            b.syr2 <- v.syr2 <- syr2[nfix,]
            b.hu <- v.hu <- hu[nfix,]
            b.hu2 <- v.hu2 <- hu2[nfix,]

            b.var <- v.var <- var[nfix,]
            b.var2 <- v.var2 <- var2[nfix,]

            index <- syr[nfix,]-start.year+1
            index2 <- syr2[nfix,]-start.year+1
            index[index<0] <- 0
            index2[index2<0] <- 0
            yearswithsowing <- array(0,length(index))

            # fixing case by case
            # setting first harvests to right year

            for(yf in 1:length(index)){

              if(index[yf]!=0){
                ind <- index[yf]

                v.var[ind] <- b.var[yf]
                v.syr[ind] <- b.syr[yf]
                v.hu[ind] <- b.hu[yf]
                yearswithsowing[ind] <- 1
              } # index

              if(index2[yf]!=0){
                ind <- index2[yf]
                v.var[ind] <- b.var2[yf]
                v.var2[yf] <- 0
                v.syr[ind] <- b.syr2[yf]
                v.syr2[yf] <- 0
                v.hu[ind] <- b.hu2[yf]
                v.hu2[yf] <- 0
                yearswithsowing[ind] <- 1
              } # index2
            } # yf in index

            # testing if all cases have been fixed #
            yfix2 <- which(v.var2>0)
            if(length(yfix2)>0) if(yfix2[1]!=length(v.var2)) print(paste("WARNING, still double harvests left:",paste(yfix2,collapse=" "),"\n"))
            yfix2 <- which(yearswithsowing==0)
            if(length(yfix2)>0) if(!(length(yfix2)==1 & yearswithsowing[length(yearswithsowing)]==0)) print(paste("WARNING, still years without left:",paste(yfix2,collapse=" "),"\n"))
            if(length(yfix2)>1){
              print(paste(paste(nfix,collapse=" "),"\n",paste(syr[nfix,],collapse=" "),"\n",paste(syr2[nfix,],collapse=" "),"\n",paste(yearswithsowing,collapse=" "),"\n",paste(v.var,collapse=" "),"\n",paste(v.var2,collapse=" "),"\n"))
            }

            # feeding back vectors with corrected double harvests
            v.var -> var[nfix,]
            v.var2 -> var2[nfix,]
            v.syr -> syr[nfix,]
            v.syr2 -> syr2[nfix,]
            v.hu -> hu[nfix,]
            v.hu2 -> hu2[nfix,]

          } # nfix loop

          # -------------------------------------------------- #
          # shift time series to have complete growing seasons #
          # -------------------------------------------------- #

          # discarding first years' values if syear is before first year, but only of if all syear2 outputs are zero, i.e. the whole time series is shifted by one year
          discard <- which(syr[,1]<start.year)

          print(paste("discarding first year values in",length(discard),"cases\n"))

          var <- discard_first(var,discard)
          syr <- discard_first(syr,discard)
          hu <- discard_first(hu,discard)
          if("soilmoist1m"%in%variables) smo <- discard_first(smo,discard,TRUE)

          # ----------------------------------------------------- #
          # dump harvest if less than 90% of heat units achieved  #
          # ----------------------------------------------------- #

          if(hlimit==TRUE && ("yield"%in%variables || "matyday"%in%variables)) {

            band_id=ifelse(ir>1,bands[cr]+15,bands[cr])
            for(y in 1:nyear) {

              dump=ifelse(hu[,y]<0.90*hu_ref[,band_id],1,0)

              if("yield"%in%variables) var[which(dump==1),y]=0 # delete yields if less than 90% of husum is reached
              if("matyday"%in%variables) var[which(dump==1),y]=var[which(dump==1),y]*-1 # document the deletion of yield by setting matyday to negatve values

            }
          }

          # ------------------------------------------ #
          # write outputs to .nc4 file
          # ------------------------------------------ #

          for(va in 1:length(variables)){

  	        # 3a file name convention: lpjml_gswp3-w5e5_obsclim_histsoc_default_plantday-wwh-firr_global_annual_1901_2016.nc
  	        # folder structure: AgMIP.output/<modelname>/phase3a/<climate_forcing>/obsclim/<crop>

    		    # output dir
            outdir=paste(out.path,"/",climate[cl],"/obsclim/",crops[cr],"/",sep="")

    		    if(!file.exists(outdir)) dir.create(outdir,recursive=TRUE)

    		    # output file
            timestep=ifelse(variables[va]=="soilmoist1m","monthly","annual")
            if(for_eval){
              cro <- if(crops[cr] %in% c("wwh","swh")) "whe" else if(crops[cr] %in% c("ri1","ri2")) "ric" else crops[cr]
              write_var=paste0(variables[va],"_",cro)
              fn <- paste(outdir,"lpjml_",climate[cl],"_hist_fullharm_",irrigs[ir],"_",variables[va],"_",crops[cr],"_",timestep,"_",start.year[cl],"_",end.year[cl],".nc4",sep="")
              #fn <- paste(outdir,"lpjml_",climate[cl],"_obsclim_",socioecon,"_",co2,"_",write_var,"_global_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
            } else {
              write_var=paste0(variables[va],"-",crops[cr],"-",irrigs[ir])
              fn <- paste(outdir,"lpjml_",climate[cl],"_obsclim_",socioecon,"_",co2,"_",write_var,"_global_",timestep,"_",start.year,"_",end.year,".nc",sep="")
            }
            unlink(fn)

            # NetCDF generation ####
            mv <- NODATA

            # NC file dimensions
            dim_lon <- ncdim_def("lon","degrees_east",raster_lons)
            #change order of latitudes
            dim_lat <- ncdim_def("lat","degrees_north",rev(raster_lats))
            if(variables[va]=="soilmoist1m") {
            	dim_time <- ncdim_def("time",paste("months since ",start.year,"-01-01",sep=""),1:(nyear*12),calendar = "standard")
            } else {
              if(for_eval){
                dim_time <- ncdim_def("time",paste("growing seasons since ",start.year,"-01-01 00:00:00",sep=""),c(start.year:end.year)-start.year+1,calendar = "standard")
              } else {
                dim_time <- ncdim_def("time",paste("growing seasons since ",start.year,"-01-01",sep=""),c(start.year:end.year)-start.year+1,calendar = "standard")
              }
            }

            # define variable
            ncv <- ncvar_def(write_var,units[va],list(dim_lon,dim_lat,dim_time),mv,longname=paste(irrigs[ir],cropf[cr],longnames[va]),compression=9)

            # create files
            ncf <- nc_create(fn,list(ncv))

            # commenting
            ncatt_put(ncf,varid=0,"title","LPJmL simulations for Ag-GRID GGCMI Phase 3 project")
            ncatt_put(ncf,varid=0,"comment1","Potsdam Insittute for Climate Impact Reserach, Columbia University")
            ncatt_put(ncf,varid=0,"comment2","jonas.jaegermeyr@columbia.edu, cmueller@pik-potsdam.de, minoli@pik-potsdam.de")
            ncatt_put(ncf,varid=0,"comment3",fn)

            # preparing data for NC files
            # having some 3D matrix: lon, lat, time
            if(variables[va]=="soilmoist1m") {
              mapo <- array(NA,dim=c(nlon*nlat,nyear*12))
            } else {
              mapo <- array(NA,dim=c(nlon*nlat,nyear))
            }
            # loop through pixels, fill matrix
            buf <- var
            if(variables[va]=="soilmoist1m") buf <- smo

            lpjraster <- raster(ncols=720, nrows=360)
            lpj_raster_index <- cellFromXY(lpjraster,grid[,c(1,2)])
            mapo[lpj_raster_index,] <- buf

            if(variables[va]=="soilmoist1m") {
              dim(mapo) <- c(nlon,nlat,nyear*12)
            } else {
              dim(mapo) <- c(nlon,nlat,nyear)
            }
            
            ncvar_put(ncf,ncv,mapo,start=c(1,1,1),count=c(-1,-1,-1))

            nc_close(ncf)

          } # variables loop

          # --------------------------------- #

          if(length(warnings())>0) print(warnings())

        } # overall crop loop
      } # if not pirnreq && ir==1
  } # irrig loop

  # --------------------------------- #

} # climate

# --------------------------------- #

print("done")

