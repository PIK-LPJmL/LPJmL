### ---------------------------------------- ###
# test

path.hu1="/p/projects/waterforce/jonas/diss/yield_variability/prescribed_husum/husum_mirca_no_photoperiod_best_season_nophoto_maturity_PGFv2.bin"
path.hu2="/p/projects/waterforce/jonas/GGCMI/seasonal_forecasting/LPJmL-PHU/input/PHU_PGFv2_1980_2010_Jaegermeyr_Frieler_2018.bin"

con=file(path.hu2,"rb")
seek(con,where=43,origin="start")
hu=readBin(con,integer(),n=24*67420,size=2)
close(con)
dim(hu)<- c(24,67420)
hu2=t(hu)

path.sd1="/p/projects/waterforce/jonas/diss/yield_variability/prescribed_sdate/hdates_mirca_2000_best_season_nophoto_maturity.bin"
path.sd2="/p/projects/waterforce/jonas/GGCMI/seasonal_forecasting/LPJmL-PHU/input/hdates_Jaegermeyr_Frieler_2018.bin"
con=file(path.sd2,"rb")
seek(con,where=43,origin="start")
sd=readBin(con,integer(),n=24*67420,size=2)
close(con)
dim(sd)<- c(24,67420)
sd2=t(sd)

path.tas1="/p/projects/waterforce/jonas/GGCMI/seasonal_forecasting/LPJmL-PHU/calc_PHU/src/tas_daily_1980_2010_PGFv2.1.clm"
path.tas2="/p/projects/waterforce/jonas/diss/yield_variability/PHU/regression_model/input_data/mean_temperature/tmp_daily_1980_2010_PGFv2.1.clm"
con<- file(path.tas1,"rb")
tas1 <- readBin(con, double(), n=67420, size=4)
close(con)
