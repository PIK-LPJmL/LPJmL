
### ---------------------------------------------------------------------------------- ###
print("create phu input for lpjml")

husum_irr=husum_rain=array(0,c(ncell,length(crops)))

for (cr in 1:length(crops)) {
	crop=crops[cr]

	# ------------------------------------------------------------------------------------------------------------------- #

	filename <- paste0(path.tmp,"PHU_",climate_data,"_",crop_calendar,"_",crop,"_ir.txt")
	if(file.exists(filename)) {
		hu_irr<- read.table(file=filename,header=FALSE,nrows=ncell)[,1]
	} else {
		print("ERROR! File not found!")
	}
	filename <- paste0(path.tmp,"PHU_",climate_data,"_",crop_calendar,"_",crop,"_rf.txt")
	if(file.exists(filename)) {
		hu_rain<- read.table(file=filename,header=FALSE,nrows=ncell)[,1]
	} else {
		print("ERROR! File not found!")
	}

	if(crop %in% verncrops) {
		filename <- paste0(path.tmp,"PHU_",climate_data,"_",crop_calendar,"_",crop,"_winter_crop_ir.txt")
		if(file.exists(filename)) {
			hu_wi_irr<- read.table(file=filename,header=FALSE,nrows=ncell)[,1]
		} else {
			print("ERROR! File not found!")
		}

		filename <- paste0(path.tmp,"PHU_",climate_data,"_",crop_calendar,"_",crop,"_winter_crop_rf.txt")
		if(file.exists(filename)) {
			hu_wi_rain<- read.table(file=filename,header=FALSE,nrows=ncell)[,1]
		} else {
			print("ERROR! File not found!")
		}

		hu_wi_irr=hu_wi_irr*-1 # store information on winter type in negative values
		husum_irr[,cr]=hu_wi_irr+hu_irr

		hu_wi_rain=hu_wi_rain*-1
		husum_rain[,cr]=hu_wi_rain+hu_rain

	} else {

		husum_irr[,cr]=hu_irr
		husum_rain[,cr]=hu_rain

	}

} # crop loop

husum_vector=c(t(cbind(husum_rain,husum_irr)))

# scale to store in integer
husum_vector=round(husum_vector)

### ---------------------------------------------------------------------------------- ###
# prepare header

header.s <- charToRaw("LPJmLHU") # new header string with 7 bytes (old 10 bytes), to read use: rawToChar()
header.int=array(0,7)
header.int[1]<- 2   	# header version: 1 for CLM, 2 for CLM2
header.int[2]<- 1   	# order
header.int[3]<- 2000   	# first year
header.int[4]<- 1     	# nyears
header.int[5]<- 0     	# first cell
header.int[6]<- ncell  	# ncell
header.int[7]<- length(crops)*2		# nbands
header.float <- c(0.5,1) # cell size and scaling factor, new in new header

### ---------------------------------------------------------------------------------- ###
# write to file

con=file(paste0(path.out,"phu_",climate_data,"_",crop_calendar,"_",ncell,".clm",sep=""),"wb")
writeBin(as.raw(header.s),con,size=1)
writeBin(as.integer(header.int),con,size=4)
writeBin(as.double(header.float),con,size=4)
writeBin(as.integer(husum_vector),con,size=2)
close(con)
