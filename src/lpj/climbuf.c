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

Bool new_climbuf(Climbuf *climbuf /**< pointer to climate buffer */
                )                 /** \return TRUE on error */
{
  int d,m;
  climbuf->max=newbuffer(CLIMBUFSIZE);
  if(climbuf->max==NULL)
  {
    climbuf->min=NULL;
    return TRUE;
  }
  climbuf->min=newbuffer(CLIMBUFSIZE);
  if(climbuf->min==NULL)
    return TRUE;
  climbuf->atemp_mean=0;
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

void init_climbuf(Climbuf *climbuf /**< pointer to climate buffer */
                 )
{
  climbuf->temp_min=HUGE_VAL;
  climbuf->temp_max=-HUGE_VAL;
  climbuf->gdd5=0;
  climbuf->mprec=0;
  climbuf->mpet=0;
} /* of 'init_climbuf' */

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
}

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

void annual_climbuf(Climbuf *climbuf /**< pointer to climate buffer */
                   )
{
  int m;
  updatebuffer(climbuf->min,climbuf->temp_min);
  updatebuffer(climbuf->max,climbuf->temp_max);
  climbuf->atemp_mean20 = (climbuf->atemp_mean20<-9998) ? climbuf->atemp : (1-kk)*climbuf->atemp_mean20+kk*climbuf->atemp;
  climbuf->atemp=0;
  climbuf->mtemp_min20 = getbufferavg(climbuf->min);
  climbuf->aprec=0;
  for(m=0;m<NMONTH;m++)
    climbuf->aprec+=climbuf->mprec20[m];
} /* of 'annual_climbuf' */

Bool fwriteclimbuf(FILE *file, /**< pointer to binary file */
                   const Climbuf *climbuf /**< pointer to climate buffer written */
                  )                       /** \return TRUE on error */
{
  fwrite(&climbuf->temp_max,sizeof(Real),1,file);
  fwrite(&climbuf->temp_min,sizeof(Real),1,file);
  fwrite(&climbuf->atemp_mean,sizeof(Real),1,file);
  fwrite(&climbuf->atemp_mean20,sizeof(Real),1,file);
  fwrite(&climbuf->atemp_mean20_fix,sizeof(Real),1,file);
  fwrite(&climbuf->gdd5,sizeof(Real),1,file);
  fwrite(climbuf->dval_prec,sizeof(Real),1,file);
  fwrite(climbuf->temp,sizeof(Real),NDAYS,file);
  fwrite(climbuf->prec,sizeof(Real),NDAYS,file);
  fwrite(climbuf->mpet20,sizeof(Real),NMONTH,file);
  fwrite(climbuf->mprec20,sizeof(Real),NMONTH,file);
  fwrite(climbuf->mtemp20,sizeof(Real),NMONTH,file);
  fwritebuffer(file,climbuf->min);
  return fwritebuffer(file,climbuf->max);
} /* of 'fwriteclimbuf' */

Bool freadclimbuf(FILE *file,  /**< pointer to binary file */
                  Climbuf *climbuf, /**< pointer to climate buffer read */
                  Bool swap /**< byte order has to be swapped (TRUE/FALSE) */
                 ) /** \return TRUE on error */
{
  int m;
  freadreal1(&climbuf->temp_max,swap,file);
  freadreal1(&climbuf->temp_min,swap,file);
  freadreal1(&climbuf->atemp_mean,swap,file);
  freadreal1(&climbuf->atemp_mean20,swap,file);
  freadreal1(&climbuf->atemp_mean20_fix,swap,file);
  freadreal1(&climbuf->gdd5,swap,file);
  freadreal1(climbuf->dval_prec,swap,file);
  freadreal(climbuf->temp,NDAYS,swap,file);
  freadreal(climbuf->prec,NDAYS,swap,file);
  freadreal(climbuf->mpet20,NMONTH,swap,file);
  freadreal(climbuf->mprec20,NMONTH,swap,file);
  freadreal(climbuf->mtemp20,NMONTH,swap,file);
  climbuf->min=freadbuffer(file,swap);
  if(climbuf->min==NULL)
  {
    climbuf->max=NULL;
    return TRUE;
  }
  climbuf->max=freadbuffer(file,swap);
  climbuf->mtemp_min20 = getbufferavg(climbuf->min);
  climbuf->atemp=0;
  climbuf->aprec=0;
  for(m=0;m<NMONTH;m++)
    climbuf->aprec+=climbuf->mprec20[m];
  return (climbuf->max==NULL);
} /* of 'freadclimbuf' */

void freeclimbuf(Climbuf *climbuf /**< pointer to binary file */
                )
{
  freebuffer(climbuf->max);
  freebuffer(climbuf->min);
} /* of 'freeclimbuf' */
