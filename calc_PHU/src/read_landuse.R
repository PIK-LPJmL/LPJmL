
cftin<- file(landuse_file,"rb")
seek(cftin,where=43+(ncells*n_bands_landuse*2*(2005-1700)),origin="start")
cft <- readBin(cftin,integer(),size=2,n=ncells*n_bands_landuse)
close(cftin)
cft<- cft*0.001 # scaling factor
dim(cft)<- c(n_bands_landuse,ncells)
cft<- t(cft)
if(n_bands_landuse==64) {
  cft32=array(0,c(ncells,32))
  cft32[,c(1:16)]=cft[,c(1:16)]
  cft32[,c(17:32)]=cft[,c(17:32)]+cft[,c(33:48)]+cft[,c(49:64)]
  cft=cft32
  rm(cft32)
}