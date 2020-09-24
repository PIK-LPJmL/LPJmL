require(ncdf4)
require(fields)

readmap.nc <- function(filename,var="",lo="lon",la="lat",starttime=1){
  nc <- nc_open(filename)
  if(var=="") var <- names(nc$var)[1]
  lon <- ncvar_get(nc,lo)
  if(min(lon)>=0){
    cat("WARNING! Longitude does not contain negative values, shifting >180 by 360\n")
    lon[lon>180] <- lon[lon>180]-360
  }
  lat <- ncvar_get(nc,la)
  if(lat[1] > lat[length(lat)]){
    cat("WARNING, inverting latitudes\n")
    
  }
  if(starttime==1)
    buf <- ncvar_get(nc,var)
  else
    buf <-ncvar_get(nc,var,start=c(1,1,starttime))
  nc_close(nc)
  if(length(dim(buf))==2)
    buf <- buf[order(lon),order(lat)]
  else if(length(dim(buf))==3)
    buf <- buf[order(lon),order(lat),]
  else if(length(dim(buf))>3)
    cat("WARNING, cannot adjust lon/lat setting for 4-dim array\n")
  buf
}

#### aggregation masks
fn <- "/p/projects/macmit/data/GGCMI/AgMIP.input/phase3/landuse/winter_spring_wheat_separation/winter_and_spring_wheat_areas_phase3.nc4"
swh_area_ir <- readmap.nc(fn,var="swh_area_ir")
swh_area_rf <- readmap.nc(fn,var="swh_area_rf")
wwh_area_ir <- readmap.nc(fn,var="wwh_area_ir")
wwh_area_rf <- readmap.nc(fn,var="wwh_area_rf")

ri1_frac_ir <- readmap.nc("/p/projects/macmit/data/GGCMI/AgMIP.input/phase3/crop_calendar/ri1_ir_ggcmi_crop_calendar_phase3_v1.01.nc4",var="fraction_of_harvested_area")
ri1_frac_rf <- readmap.nc("/p/projects/macmit/data/GGCMI/AgMIP.input/phase3/crop_calendar/ri1_rf_ggcmi_crop_calendar_phase3_v1.01.nc4",var="fraction_of_harvested_area")
ri2_frac_ir <- readmap.nc("/p/projects/macmit/data/GGCMI/AgMIP.input/phase3/crop_calendar/ri2_ir_ggcmi_crop_calendar_phase3_v1.01.nc4",var="fraction_of_harvested_area")
ri2_frac_rf <- readmap.nc("/p/projects/macmit/data/GGCMI/AgMIP.input/phase3/crop_calendar/ri2_rf_ggcmi_crop_calendar_phase3_v1.01.nc4",var="fraction_of_harvested_area")

# set NA land pixels to zero
swh_area_rf[!is.finite(swh_area_rf) & is.finite(ri1_frac_rf)] <- 0
swh_area_ir[!is.finite(swh_area_ir) & is.finite(ri1_frac_rf)] <- 0
wwh_area_rf[!is.finite(wwh_area_rf) & is.finite(ri1_frac_rf)] <- 0
wwh_area_ir[!is.finite(wwh_area_ir) & is.finite(ri1_frac_rf)] <- 0

climate=c("gswp3-w5e5")
start.year <- 1980
end.year <- 2010
timestep <- "annual"
irrigs <- c("noirr","firr")
res <- 0.5
raster_lons=seq(-180+(res/2),180-(res/2),0.5)
raster_lats=seq(-90+(res/2),90-(res/2),0.5)
mv <- 1e20

out.path <- "/p/projects/macmit/data/GGCMI/phase3_eval_final"
# NC file dimensions
dim_lon <- ncdim_def("lon","degrees_east",raster_lons)
#change order of latitudes
dim_lat <- ncdim_def("lat","degrees_north",rev(raster_lats))
dim_time <- ncdim_def("time",paste("growing seasons since ",start.year,"-01-01 00:00:00",sep=""),c(start.year:end.year)-start.year+1,calendar = "standard")

for(ir in 1:2){
  if(1==ir){
    ar1 <- ri1_frac_rf
    ar2 <- ri2_frac_rf
  } else {
    ar1 <- ri1_frac_ir
    ar2 <- ri2_frac_ir
  }
  sumar <- ar1+ar2
  write_var<-"yield_ric"
  dir.create(paste0(out.path,"/gswp3-w5e5/obsclim/ric/"),recursive=TRUE,showWarnings=F)
  fno <- paste(out.path,"/gswp3-w5e5/obsclim/ric/lpjml_",climate,"_hist_fullharm_",irrigs[ir],"_yield_ric_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
  fn1 <- paste(out.path,"/gswp3-w5e5/obsclim/ri1/lpjml_",climate,"_hist_fullharm_",irrigs[ir],"_yield_ri1_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
  fn2 <- paste(out.path,"/gswp3-w5e5/obsclim/ri2/lpjml_",climate,"_hist_fullharm_",irrigs[ir],"_yield_ri2_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
  in1 <- readmap.nc(fn1)
  in2 <- readmap.nc(fn2)
  
  out <- in1
  for(i in 1:dim(out)[3]) out[,,i] <- (in1[,,i]* ar1 + in2[,,i]*ar2)/sumar 
  ncv <- ncvar_def(write_var,"t ha-1 yr-1",list(dim_lon,dim_lat,dim_time),mv,longname=paste(irrigs[ir],"rice","rice yield"),compression=9)
  
  # create files
  ncf <- nc_create(fno,list(ncv))
  # commenting
  ncatt_put(ncf,varid=0,"title","LPJmL simulations for Ag-GRID GGCMI Phase 3 project")
  ncatt_put(ncf,varid=0,"comment1","Potsdam Institute for Climate Impact Research, Columbia University")
  ncatt_put(ncf,varid=0,"comment2","jonas.jaegermeyr@columbia.edu, cmueller@pik-potsdam.de, minoli@pik-potsdam.de")
  ncvar_put(ncf,ncv,out[,360:1,],start=c(1,1,1),count=c(-1,-1,-1))
  
  nc_close(ncf)
  
  
  # wheat
  if(1==ir){
    ar1 <- swh_area_rf
    ar2 <- wwh_area_rf
  } else {
    ar1 <- swh_area_ir
    ar2 <- wwh_area_ir
  }
  sumar <- ar1+ar2
  write_var<-"yield_whe"
  dir.create(paste0(out.path,"/gswp3-w5e5/obsclim/whe/"),recursive=TRUE,showWarnings=F)
  fno <- paste(out.path,"/gswp3-w5e5/obsclim/whe/lpjml_",climate,"_hist_fullharm_",irrigs[ir],"_yield_whe_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
  fn1 <- paste(out.path,"/gswp3-w5e5/obsclim/swh/lpjml_",climate,"_hist_fullharm_",irrigs[ir],"_yield_swh_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
  fn2 <- paste(out.path,"/gswp3-w5e5/obsclim/wwh/lpjml_",climate,"_hist_fullharm_",irrigs[ir],"_yield_wwh_",timestep,"_",start.year,"_",end.year,".nc4",sep="")
  in1 <- readmap.nc(fn1)
  in2 <- readmap.nc(fn2)
  
  out <- in1
  for(i in 1:dim(out)[3]) out[,,i] <- (in1[,,i]* ar1 + in2[,,i]*ar2)/sumar 
  ncv <- ncvar_def(write_var,"t ha-1 yr-1",list(dim_lon,dim_lat,dim_time),mv,longname=paste(irrigs[ir],"wheat","wheat yield"),compression=9)
  
  # create files
  ncf <- nc_create(fno,list(ncv))
  # commenting
  ncatt_put(ncf,varid=0,"title","LPJmL simulations for Ag-GRID GGCMI Phase 3 project")
  ncatt_put(ncf,varid=0,"comment1","Potsdam Institute for Climate Impact Research, Columbia University")
  ncatt_put(ncf,varid=0,"comment2","jonas.jaegermeyr@columbia.edu, cmueller@pik-potsdam.de, minoli@pik-potsdam.de")
  ncvar_put(ncf,ncv,out[,360:1,],start=c(1,1,1),count=c(-1,-1,-1))
  
  nc_close(ncf)
  
}


