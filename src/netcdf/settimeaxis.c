/**************************************************************************************/
/**                                                                                \n**/
/**                   s  e  t  t  i  m  e  a  x  i  s  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates time and time boundary vectors for NetCDF files           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool settimeaxis(double **time,       /**< allocated and initialized time vector */
                 double **time_bnds,  /**< allocated and initialized time bnd vector */
                 int nyear,           /**< number of years */
                 int nstep,           /**< number of samples per year (1/12/365) */
                 int timestep,        /**< time step for annual output (yrs) */
                 int outputyear,      /**< first year of output */
                 int baseyear,        /**< base year of output */
                 Bool oneyear,        /**< output is for one year only (TRUE/FALSE) */
                 Bool with_days,      /**< time axis in units of days (TRUE/FALSE) */
                 Bool absyear,        /**< absolute years instead of relative years (TRUE/FALSE) */
                 const char *filename /**< filename of NetCDF file */
                )                     /** \return TRUE on error */
{
  int i,j;
  *time=*time_bnds=NULL;
  if(nstep==1)
  {
    *time=newvec(double,nyear/timestep);
    *time_bnds=newvec(double,(nyear/timestep)*2);
  }
  else
  {
    *time=newvec(double,nyear*nstep);
    *time_bnds=newvec(double,nyear*nstep*2);
  }
  if(*time==NULL || *time_bnds==NULL)
  {
    free(*time);
    free(*time_bnds);
    printallocerr("time");
    return TRUE;
  }
  switch(nstep)
  {
    case 0:
      break;
    case 1:
      if(with_days)
      {
        for(i=0;i<nyear/timestep;i++)
        {
          (*time)[i]=(outputyear-baseyear+i*timestep)*NDAYYEAR+timestep*NDAYYEAR/2;
          (*time_bnds)[2*i]=(outputyear-baseyear+i*timestep)*NDAYYEAR;
          (*time_bnds)[2*i+1]=(outputyear-baseyear+(i+1)*timestep)*NDAYYEAR;
        }
      }
      else
      {
        if(absyear)
          for(i=0;i<nyear/timestep;i++)
          {
            (*time)[i]=outputyear+i*timestep+timestep*0.5;
            (*time_bnds)[2*i]=outputyear+i*timestep;
            (*time_bnds)[2*i+1]=outputyear+(i+1)*timestep;
          }
        else
          for(i=0;i<nyear/timestep;i++)
          {
            (*time)[i]=outputyear-baseyear+i*timestep+timestep*0.5;
            (*time_bnds)[2*i]=outputyear-baseyear+i*timestep;
            (*time_bnds)[2*i+1]=outputyear-baseyear+(i+1)*timestep;
          }
      }
      break;
    case NMONTH:
      if(with_days)
      {
        for(i=0;i<nyear;i++)
          for(j=0;j<NMONTH;j++)
            if(i==0 && j==0)
            {
              if(oneyear)
              {
                (*time)[0]=15;
                (*time_bnds)[0]=0;
                (*time_bnds)[1]=ndaymonth[0];
              }
              else
              {
                (*time)[0]=15+(outputyear-baseyear)*NDAYYEAR;
                (*time_bnds)[0]=(outputyear-baseyear)*NDAYYEAR;
                (*time_bnds)[1]=ndaymonth[j]+(outputyear-baseyear)*NDAYYEAR;
              }
            }
            else
            {
              (*time)[i*NMONTH+j]=(*time)[i*NMONTH+j-1]+ndaymonth[(j==0) ? 11 : j-1];
              (*time_bnds)[2*(i*NMONTH+j)]=(*time_bnds)[2*(i*NMONTH+j)-1];
              (*time_bnds)[2*(i*NMONTH+j)+1]=(*time_bnds)[2*(i*NMONTH+j)]+ndaymonth[j];
            }
      }
      else
      {
        if(oneyear)
          for(i=0;i<NMONTH;i++)
          {
            (*time)[i]=i+0.5;
            (*time_bnds)[2*i]=i;
            (*time_bnds)[2*i+1]=i+1;
          }
        else
          for(i=0;i<nyear*NMONTH;i++)
          {
            (*time)[i]=(*time_bnds)[2*i]=(outputyear-baseyear)*NMONTH+i+0.5;
            (*time_bnds)[2*i]=(outputyear-baseyear)*NMONTH+i;
            (*time_bnds)[2*i+1]=(outputyear-baseyear)*NMONTH+i+1;
          }
      }
      break;
    case NDAYYEAR:
      if(oneyear)
        for(i=0;i<nstep;i++)
        {
          (*time)[i]=i+0.5;
          (*time_bnds)[2*i]=i;
          (*time_bnds)[2*i+1]=i+1;
        }
      else
        for(i=0;i<nyear*nstep;i++)
        {
          (*time)[i]=(outputyear-baseyear)*NDAYYEAR+i+0.5;
          (*time_bnds)[2*i]=(outputyear-baseyear)*NDAYYEAR+i;
          (*time_bnds)[2*i+1]=(outputyear-baseyear)*NDAYYEAR+i+1;
        }
      break;
    default:
      fprintf(stderr,"ERROR425: Invalid value=%d for number of data points per year in '%s', must be 1, 12, or 365.\n",
              nstep,filename);
      free(*time);
      free(*time_bnds);
      return TRUE;
  } /* of switch(nstep) */
  return FALSE;
} /* of 'settimeaxis' */
