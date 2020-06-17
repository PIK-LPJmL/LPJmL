print("calculate vernalization requirements")

### --------------------------------------------------------------------------------------------------- ###
# croppars

max.vern.days <- as.numeric(croppars[rownames(croppars)=="max.vern.days",names(croppars)==crop]) # maximal vernalization days
max.vern.days.month=max.vern.days/5 # Vsat_max [days per month]

Tv2 <- as.numeric(croppars[rownames(croppars)=="vern.temp.opt.min",names(croppars)==crop]) # temp.vern.min.optimum
Tv3 <- as.numeric(croppars[rownames(croppars)=="vern.temp.opt.max",names(croppars)==crop]) # temp.vern.max.optimum

### --------------------------------------------------------------------------------------------------- ###
# load mean month temperature

zz<- file(fn_tas_month,"rb")
tmp_mean_month <- readBin(zz, double(), n=ncell*12, size=4)
close(zz)
dim(tmp_mean_month) <- c(ncell,12)

### --------------------------------------------------------------------------------------------------- ###
# calculate vernalization days within 5 coldest months
irrtyp=c("rf","ir")

for(typ in irrtyp) {

    if(typ=="rf") {
        sd=sdate_winter_rain
    } else {
        sd=sdate_winter_irr
    }

    if(sum(sd)>0) {

		vern.days.required=array(0,ncell) # required vernalized days during 5 coldest month
		for (i in 1:ncell) {
			active_cell=sd[i]
			if(active_cell>0 && sum(is.na(tmp_mean_month[i,]))==0){

	            # find indices of 5 coldest months
	            temp=array(0,c(12,2))
	            temp[,1]<-tmp_mean_month[i,]
	            temp[,2]<-c(1:12)
	            sort_temp=temp[order(temp[,1]),]
	            coldest_months=sort_temp[1:5,2]

				days_required=0
				for(m in 1:5) { # sum up effective vernailization
					if(tmp_mean_month[i,coldest_months[m]]<=Tv2) {
						days=max.vern.days.month
					} else if(tmp_mean_month[i,coldest_months[m]]>=Tv3) {
						days=0
					} else if(tmp_mean_month[i,coldest_months[m]]>Tv2 & tmp_mean_month[i,coldest_months[m]]<Tv3) {
						days=max.vern.days.month*(1-(tmp_mean_month[i,coldest_months[m]]-Tv2)/(Tv3-Tv2))
					}
					days_required=days_required+days
				}

				vern.days.required[i]<- round(days_required)
				rm(days_required,days)
			}
		} # ncell

		if(typ=="rf") {
			vern.days.required_rain=vern.days.required
		} else {
			vern.days.required_irr=vern.days.required
		}

	rm(vern.days.required)

	} # if sum(sd)>0

} # irrtyp

rm(typ,irrtyp,tmp_mean_month,sd,Tv2,Tv3,max.vern.days,max.vern.days.month)
