
### ----------------------------------------------------------------------------------- ###
### some useful functions for GGCMI post-processing
### ----------------------------------------------------------------------------------- ###

sort_by_harvest=function(yield,harvyear,years_sim,years_analysis) {

  harvyear[which(harvyear==0)]=NA

  years_harv_correct=c((years_analysis[1]-1):(years_analysis[length(years_analysis)]+1))
  yield=yield[,,which(years_sim%in%years_harv_correct)]
  harvyear=harvyear[,,which(years_sim%in%years_harv_correct)]

  yld_corr=array(NA,c(720,360,length(years_harv_correct)))

  for(y in 2:length(years_harv_correct)){

    year=years_harv_correct[y]
    y1=yield[,,y]
    y2=yield[,,y-1]

    yfin=yld_corr[,,y]
    yfin[which(harvyear[,,y]==year)]=y1[which(harvyear[,,y]==year)]
    yfin[which(harvyear[,,y-1]==year)]=y2[which(harvyear[,,y-1]==year)]
    yld_corr[,,y]=yfin
  }
  yld_corr=yld_corr[,,-c(1,length(years_harv_correct))]
  return(yld_corr)
  rm(years_harv_correct,year,y1,y2,yfin)
}

### ----------------------------------------------------------------------------------- ###

anomalies<-function(yield,normalized=FALSE){
  poly_fit <- lm(yield ~ poly(seq(1,length(yield)),2,raw=TRUE))
  detrended = yield - predict(poly_fit, newdata=data.frame(years = 1:length(yield)))
  ano=as.numeric(detrended-mean(detrended,na.rm=T))
  if(normalized==TRUE) {
    # divides by SD to normalize anomalies
    ano=ano/sd(ano,na.rm=T)
  }
  return(ano)
}

### ----------------------------------------------------------------------------------- ###
