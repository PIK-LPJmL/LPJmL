print("calc phu wintercrop")

### --------------------------------------------------------------------------------------------------- ###
# croppars

max.vern.days <- as.numeric(croppars[rownames(croppars)=="max.vern.days",names(croppars)==crop]) # maximal vernalization days
max.vern.days.month=max.vern.days/5 # Vsat_max [days per month]

tv1 <- as.numeric(croppars[rownames(croppars)=="vern.temp.min",names(croppars)==crop]) # temp.vern.min.optimum
tv2 <- as.numeric(croppars[rownames(croppars)=="vern.temp.opt.min",names(croppars)==crop]) # temp.vern.min.optimum
tv3 <- as.numeric(croppars[rownames(croppars)=="vern.temp.opt.max",names(croppars)==crop]) # temp.vern.max.optimum
tv4 <- as.numeric(croppars[rownames(croppars)=="vern.temp.max",names(croppars)==crop]) # temp.vern.max.optimum

### --------------------------------------------------------------------------------------------------- ###
# load mean day temperature

zz<- file(fn_tas_day,"rb")
tmp_mean_day <- readBin(zz, double(), n=ncell*365, size=4)
close(zz)
dim(tmp_mean_day) <- c(ncell,365)

### --------------------------------------------------------------------------------------------------- ###

ndays<- 730
irrtyp=c("rf","ir")

for (typ in irrtyp) {

    if(typ=="rf") {
        sd=sdate_winter_rain
        hd=hdate_winter_rain
        mdt=tmp_mean_day
        mdt[sd==0,]<-0
        if(sum(sd)>0) vd=vern.days.required_rain
    } else {
        sd=sdate_winter_irr
        hd=hdate_winter_irr
        mdt=tmp_mean_day
        mdt[sd==0,]<-0
        if(sum(sd)>0) vd=vern.days.required_irr
    }

    if(sum(sd)>0) {
        ### --------------------------------------------------------------------------------------------------- ###
        ### Vernalization factor

        endday.vern<- array(0,ncell) # array with adapted dates of end of vernalization period
        vern_factor <- array(1.0,c(ncell,365))

        for(i in 1:ncell) {
            if(sd[i]>0 && vd[i]>0 && sum(is.na(mdt[i,]))==0) {

                # calculation of vernalization effectiveness
                veff=array(0,365) # 1 means full vernalization day

                for(k in 1:365) {
                    if(mdt[i,k]>=tv1 && mdt[i,k]<tv2) veff[k]<-(mdt[i,k]-tv1)/(tv2-tv1)
                    else if(mdt[i,k]>=tv2 && mdt[i,k]<=tv3) veff[k]<-1
                    else if(mdt[i,k]>tv3 && mdt[i,k]<tv4) veff[k]<-(tv4-mdt[i,k])/(tv4-tv3)
                    else if(mdt[i,k]>=tv4) veff[k]<-0
                    else if(mdt[i,k]<tv1) veff[k]<-0
                    else {
                        print(paste("Stop! no veff associated on day:",k,"cell:",i))
                        break
                    }
                }
                veff[veff>1]<-1
                veff[veff<0]<-0

                veff<-c(veff,veff)

                # Day when vernalization requirements (Vreq) are met
                vdsum=0
                k=sd[i]
                while(vdsum<vd[i]) {
                    vdsum <- vdsum + veff[k]
                    if(k>hd[i] || vdsum>=vd[i]) break
                    k=k+1
                }
                if(vdsum>=vd[i]) {
                    endday.vern[i]<- k
                } else {
                    endday.vern[i]<- -10
                }

                ### --------------------------------------------------------------------------------------------------- ###

                if(endday.vern[i]>0) {
                    vdsum=0

                    for(k in sd[i]:endday.vern[i]) {
  
                        vdsum <- vdsum + veff[k]

                        # if VDD < 20% Vreq (Vb)
                        if(vdsum<(vd[i]*0.2)) {
                            vern_factor[i,ifelse(k>365,k-365,k)] <- 0.0 # factor = 0 until 20% vernalization requirements reached
                        } else {
                            vern_factor[i,ifelse(k>365,k-365,k)] <- max(0,min(1,(vdsum - (vd[i]*0.2)) / (vd[i] - (vd[i]*0.2))))
                        }
                    } # day loop
                } # if

                rm(vdsum,k,veff)

            } # if sd >0 & vd >0

            if((sd[i] > 0) && (vd[i] == 0)) {   # in cases of no vernalization requirement (winter crop in warm regions or spring crop)
                endday.vern[i]<- sd[i]
            }

        } # cell loop
    } # if sum(sd)>0
    
    husum.vd=husum=array(0,ncell) # husum over MIRCA growing season with photoperiod and vernalization

    if(sum(sd)>0) {

        for(i in 1:ncell) {
            if(sd[i]>0 && (endday.vern[i] != -10) && sum(is.na(mdt[i,]))==0) {

                # select days not in growing period
                if(hd[i]<=365) days_no_gp=c(1:(sd[i]-1),hd[i]:365)
                if(hd[i]>365) days_no_gp=c((hd[i]-365):(sd[i]-1))

                teff<- mdt[i,]-basetemp
                teff[teff<0]<-0 # effective temperature
                teff.vd <- teff * vern_factor[i,] # effective temperature x photoperiod factor
                teff.vd[days_no_gp]<-0

                husum.vd[i] <- sum(teff.vd)
                rm(teff,days_no_gp)
            } # if
        } # cell

        rm(endday.vern,vern_factor)
    } # if sum(sd)>0

    filename <- paste0("PHU_",climate_data,"_",crop_calendar,"_",crop,"_winter_crop_",typ,".txt")
    write.table(husum.vd,file=paste(path.tmp,filename,sep=""),quote = FALSE,col.names = FALSE,row.names = FALSE)

    rm(husum,husum.vd,filename)

} # irr/rain loop

rm(tmp_mean_day,mdt,sd,hd,typ,tv2,tv3,ndays,irrtyp,max.vern.days)
