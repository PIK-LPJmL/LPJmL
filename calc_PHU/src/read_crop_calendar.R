
# This script reads in the observed/prescribed sowing and harvest dates and area
print("read crop calendar")

# ------------------------------------------------------------------------------------------------------------------- #
# Read in binary input files
irrtyp=c("rainfed","irrigated")

for (typ in irrtyp) {

	sdin<- file(path.sowing,"rb")
	seek(sdin,where=43,origin="start")
	sdate <- readBin(sdin,integer(),size=2,n=ncell*length(crops)*2)
	close(sdin)
	dim(sdate)<- c(length(crops)*2,ncell)
	sdate=t(sdate)

	hdin<- file(path.harvest,"rb")
	seek(hdin,where=43,origin="start")
	hdate <- readBin(hdin,integer(),size=2,n=ncell*length(crops)*2)
	close(hdin)
	dim(hdate)<- c(length(crops)*2,ncell)
	hdate=t(hdate)

	if(typ=="rainfed") {
		sdate=sdate[,which(crops==crop)]
		hdate=hdate[,which(crops==crop)]
	} else {
		sdate=sdate[,which(crops==crop)+length(crops)]
		hdate=hdate[,which(crops==crop)+length(crops)]
	}

	# Shift spring hdates into next year
	hdate=ifelse(sdate>0 & hdate>0 & sdate>hdate,hdate + 365,hdate)

	if(typ=="rainfed") {
		sdate_rain=sdate
		hdate_rain=hdate
	} else {
		sdate_irr=sdate
		hdate_irr=hdate
	}

} # rain/irr loop

rm(typ,irrtyp,sdate,hdate)
