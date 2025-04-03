/**************************************************************************************/
/**                                                                                \n**/
/**                 c  l  i  m   b  u  f  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define k (1.0/12.0)
#define kk 0.05
#define readreal2(file,name,val,swap) if(readreal(file,name,val,swap)) return TRUE
#define readrealarray2(file,name,val,size,swap) if(readrealarray(file,name,val,size,swap)) return TRUE

Bool new_climbuf(Climbuf *climbuf, /**< pointer to climate buffer */
                 int ncft          /**< number of crop pfts */
                )                  /** \return TRUE on error */
{
  int d,m,c;
  climbuf->max=newbuffer(CLIMBUFSIZE);
  if(climbuf->max==NULL)
  {
    climbuf->min=NULL;
    return TRUE;
  }
  climbuf->min=newbuffer(CLIMBUFSIZE);
  if(climbuf->min==NULL)
    return TRUE;
  climbuf->V_req=newvec(Real,ncft);
  if(climbuf->V_req==NULL)
    return TRUE;
  climbuf->V_req_a=newvec(Real,ncft);
  if(climbuf->V_req_a==NULL)
    return TRUE;
  for(c=0;c<ncft;c++)
  {
   climbuf->V_req[c]=-9999;
   climbuf->V_req_a[c]=0;
  }
  climbuf->atemp_mean=0;
  climbuf->aetp_mean=0;
  climbuf->atemp=0;
  climbuf->atemp_mean20=-9999;
  climbuf->atemp_mean20_fix=0;
  climbuf->dval_prec[0]=0;
  climbuf->mprec=0;
  climbuf->aprec=0;
  climbuf->mpet=0;
  for(d=0;d<NDAYS;d++)
    climbuf->temp[d]=0;
  for(d=0;d<NDAYS;d++)
    climbuf->prec[d]=0;
  for(m=0;m<NMONTH;m++)
    climbuf->mpet20[m]=climbuf->mprec20[m]=climbuf->mtemp20[m]=-9999;
  return FALSE;
} /* of 'new_climbuf' */

void init_climbuf(Climbuf *climbuf, /**< pointer to climate buffer */
                  int ncft          /**< number of crop pfts */
                 )
{
  int c;
  climbuf->temp_min=HUGE_VAL;
  climbuf->temp_max=-HUGE_VAL;
  climbuf->gdd5=0;
  climbuf->mprec=0;
  climbuf->mpet=0;
  for(c=0;c<ncft;c++)
    climbuf->V_req_a[c]=0;
} /* of 'init_climbuf' */

void getmintemp20_n(const Climbuf *climbuf, /**< pointer to climate buffer */
                    Real min[],             /**< get n coldest months temperarures */
                    int n                   /**< size of array */
                   )
{
  /* calculates n coldest month and returns their values */
  Real temp[NMONTH],swp;
  int m,i,index;
  for(m=0;m<NMONTH;m++)
    temp[m]=climbuf->mtemp20[m];
  for(i=0;i<n;i++)
  {
    min[i]=temp[i];
    index=i;
    for(m=i+1;m<NMONTH;m++)
      if(min[i]>temp[m])
      {
        min[i]=temp[m];
        index=m;
      }
    swp=temp[i];
    temp[i]=temp[index];
    temp[index]=swp;
  }
} /* of 'getmintemp20_n' */

void daily_climbuf(Climbuf *climbuf, /**< pointer to climate buffer */
                   Real temp,        /**< daily temperature (deg C) */
                   Real prec         /**< daily precipitation (mm) */
                  )
{
  int d;
  updategdd5(climbuf,temp);
  for(d=1;d<NDAYS;d++)
    climbuf->temp[d-1]=climbuf->temp[d];
  climbuf->temp[NDAYS-1]=temp;
  for(d=1;d<NDAYS;d++)
    climbuf->prec[d-1]=climbuf->prec[d];
  climbuf->prec[NDAYS-1]=prec;
} /* of  'daily_climbuf' */

Real getavgprec(const Climbuf *climbuf)
{
  Real avg_prec=0;
  int d;
  for(d=0;d<(NDAYS/3);d++)
   avg_prec+=climbuf->prec[d];
  return avg_prec/(NDAYS/3);
} /* of 'getavgprec' */

void monthly_climbuf(Climbuf *climbuf, /**< pointer to climate buffer */
                     Real mtemp,       /**< monthly average temperature (deg C) */
                     Real mprec,       /**< monthly precipitation (mm) */
                     Real mpet,        /**< monthly potential evapotranspiration (mm) */
                     int month         /**< month of year (0..11) */
                    )
{
  if(climbuf->temp_min>mtemp)
    climbuf->temp_min=mtemp;
  if(climbuf->temp_max<mtemp)
    climbuf->temp_max=mtemp;
  climbuf->atemp_mean=(1-k)*climbuf->atemp_mean+k*mtemp;
  climbuf->mprec20[month]= (climbuf->mprec20[month]<-9998) ? mprec : (1-kk)*climbuf->mprec20[month]+kk*mprec;
  climbuf->mpet20[month]= (climbuf->mpet20[month]<-9998) ? mpet : (1-kk)*climbuf->mpet20[month]+kk*mpet;
  climbuf->mtemp20[month]= (climbuf->mtemp20[month]<-9998) ? mtemp : (1-kk)*climbuf->mtemp20[month]+kk*mtemp;
  climbuf->atemp+=mtemp*k;
} /* of 'monthly_climbuf' */

void annual_climbuf(Climbuf *climbuf,    /**< pointer to climate buffer */
                    Real aetp,           /**< annual evapotranspiration (mm) */
                    int ncft,            /**< number of crop pfts */
                    Bool update_v_req    /**< update V_req vector (TRUE/FALSE) */
                   )
{
  int m;
  int cft;
  updatebuffer(climbuf->min,climbuf->temp_min);
  updatebuffer(climbuf->max,climbuf->temp_max);
  climbuf->atemp_mean20 = (climbuf->atemp_mean20<-9998) ? climbuf->atemp : (1-kk)*climbuf->atemp_mean20+kk*climbuf->atemp;
  climbuf->aetp_mean=(1-kk)*climbuf->aetp_mean+kk*aetp;
  climbuf->atemp=0;
  climbuf->mtemp_min20 = getbufferavg(climbuf->min);
  climbuf->aprec=0;
  for(m=0;m<NMONTH;m++)
    climbuf->aprec+=climbuf->mprec20[m];
  if(update_v_req)
  {
    for(cft=0;cft<ncft;cft++)
      climbuf->V_req[cft]= (climbuf->V_req[cft]< -9998) ? climbuf->V_req_a[cft] : (1-kk)*climbuf->V_req[cft]+kk*climbuf->V_req_a[cft];
  }
} /* of 'annual_climbuf' */

Bool fwriteclimbuf(FILE *file,             /**< pointer to restart file */
                   const char *name,       /**< name of object */
                   const Climbuf *climbuf, /**< pointer to climate buffer written */
                   int ncft                /**< number of crop pfts */
                  )                        /** \return TRUE on error */
{
  writestruct(file,name);
  writereal(file,"temp_max",climbuf->temp_max);
  writereal(file,"temp_min",climbuf->temp_min);
  writereal(file,"atemp_mean",climbuf->atemp_mean);
  writereal(file,"aetp_mean",climbuf->aetp_mean);
  writereal(file,"atemp_mean20",climbuf->atemp_mean20);
  writereal(file,"atemp_mean20_fix",climbuf->atemp_mean20_fix);
  writereal(file,"gdd5",climbuf->gdd5);
  writereal(file,"dval_prec",climbuf->dval_prec[0]);
  writerealarray(file,"temp",climbuf->temp,NDAYS);
  writerealarray(file,"prec",climbuf->prec,NDAYS);
  writerealarray(file,"mpet20",climbuf->mpet20,NMONTH);
  writerealarray(file,"mprec20",climbuf->mprec20,NMONTH);
  writerealarray(file,"mtemp20",climbuf->mtemp20,NMONTH);
  writerealarray(file,"V_req",climbuf->V_req,ncft);
  writerealarray(file,"V_req_a",climbuf->V_req_a,ncft);
  fwritebuffer(file,"min",climbuf->min);
  fwritebuffer(file,"max",climbuf->max);
  return writeendstruct(file);
} /* of 'fwriteclimbuf' */

Bool freadclimbuf(FILE *file,       /**< pointer to restart file */
                  const char *name, /**< name of object */
                  Climbuf *climbuf, /**< pointer to climate buffer read */
                  int ncft,         /**< number of crop pfts */
                  Bool swap         /**< byte order has to be swapped (TRUE/FALSE) */
                 )                  /** \return TRUE on error */
{
  int m;
  if(readstruct(file,name,swap))
    return TRUE;
  readreal2(file,"temp_max",&climbuf->temp_max,swap);
  readreal2(file,"temp_min",&climbuf->temp_min,swap);
  readreal2(file,"atemp_mean",&climbuf->atemp_mean,swap);
  readreal2(file,"aetp_mean",&climbuf->aetp_mean,swap);
  readreal2(file,"atemp_mean20",&climbuf->atemp_mean20,swap);
  readreal2(file,"atemp_mean20_fix",&climbuf->atemp_mean20_fix,swap);
  readreal2(file,"gdd5",&climbuf->gdd5,swap);
  readreal2(file,"dval_prec",climbuf->dval_prec,swap);
  readrealarray2(file,"temp",climbuf->temp,NDAYS,swap);
  readrealarray2(file,"prec",climbuf->prec,NDAYS,swap);
  readrealarray2(file,"mpet20",climbuf->mpet20,NMONTH,swap);
  readrealarray2(file,"mprec20",climbuf->mprec20,NMONTH,swap);
  readrealarray2(file,"mtemp20",climbuf->mtemp20,NMONTH,swap);
  climbuf->V_req=newvec(Real,ncft);
  if(climbuf->V_req==NULL)
    return TRUE;
  readrealarray2(file,"V_req",climbuf->V_req,ncft,swap);
  climbuf->V_req_a=newvec(Real,ncft);
  if(climbuf->V_req_a==NULL)
    return TRUE;
  readrealarray2(file,"V_req_a",climbuf->V_req_a,ncft,swap);
  climbuf->min=freadbuffer(file,"min",swap);
  if(climbuf->min==NULL)
  {
    climbuf->max=NULL;
    return TRUE;
  }
  climbuf->max=freadbuffer(file,"max",swap);
  climbuf->mtemp_min20 = getbufferavg(climbuf->min);
  climbuf->atemp=0;
  climbuf->aprec=0;
  for(m=0;m<NMONTH;m++)
    climbuf->aprec+=climbuf->mprec20[m];
  if(climbuf->max==NULL)
    return TRUE;
  return readendstruct(file);
} /* of 'freadclimbuf' */

void freeclimbuf(Climbuf *climbuf /**< pointer to binary file */
                )
{
  freebuffer(climbuf->max);
  freebuffer(climbuf->min);
  free(climbuf->V_req);
  free(climbuf->V_req_a);
} /* of 'freeclimbuf' */
