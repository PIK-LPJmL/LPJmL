print("calc phu")

### --------------------------------------------------------------------------------------------------- ###
# load mean day temperature

zz<- file(fn_tas_day,"rb")
tmp_mean_day <- readBin(zz, double(), n=ncell*365, size=4)
close(zz)
dim(tmp_mean_day) <- c(ncell,365)

### --------------------------------------------------------------------------------------------------- ###
# cell loop

ndays<- 730
irrtyp=c("rf","ir")

for (typ in irrtyp) {

    if(typ=="rf") {
        sd=sdate_rain
        hd=hdate_rain
        mdt=tmp_mean_day
        mdt[sd==0,]<-0
    } else {
        sd=sdate_irr
        hd=hdate_irr
        mdt=tmp_mean_day
        mdt[sd==0,]<-0
    }

    ### --------------------------------------------------------------------------------------------------- ###
    # Determine anthesis day and then calculate available husum

    husum=array(0,ncell) # husum during the observed growing season (without effects of photoperiod)

    if(sum(sd)>0) {

        count=0
        count_missing=0
        for(i in 1:ncell) {
            if(sd[i]>0 && sum(is.na(mdt[i,]))==0) {

                count=count+1

                # select days not in growing period
                if(hd[i]<=365) days_no_gp=c(1:(sd[i]-1),hd[i]:365)
                if(hd[i]>365) days_no_gp=c((hd[i]-365):(sd[i]-1))

                teff<- mdt[i,]-basetemp
                teff[teff<0]<-0 # effective temperature
                teff[days_no_gp]<-0

                husum[i] <- sum(teff)

                rm(teff,days_no_gp)
            } # sdate > 0 loop
        } # cell loop
    } # sum(sd)>0

    filename <- paste0("PHU_",climate_data,"_",crop_calendar,"_",crop,"_",typ,".txt")
    write.table(husum,file=paste0(path.tmp,filename),quote = FALSE,col.names = FALSE,row.names = FALSE)

} # irr/rain loop

### --------------------------------------------------------------------------------------------------- ###

rm(typ,irrtyp,sd,hd,ndays,tmp_mean_day,mdt,husum)
