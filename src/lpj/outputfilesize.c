/**************************************************************************************/
/**                                                                                \n**/
/**         o  u  t  p  u  t  f  i  l  e  s  i  z  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates sum of all output files sizes.                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

long long outputfilesize(const Config *config /**< LPJ configuration */
                        )                     /** \return size of files in bytes */
{
  long long sum,size,size_cdf;
  int i;
  Extension ext;
  Coord_netcdf coord;
  Bool iscdf;
  sum=0;
  iscdf=FALSE;
  for(i=0;i<config->n_out;i++)
  {
    size=getsize(i,config);
    size*=config->lastyear-config->outputyear+1;
    if(config->outputvars[i].filename.fmt==CDF)
    {
      if(!iscdf)
      {
         if(config->soil_filename.fmt==CDF)
         {
           coord=opencoord_netcdf(config->soil_filename.name,
                                  config->soil_filename.var,isroot(*config));
           if(coord!=NULL)
           {
             getextension_netcdf(&ext,coord);
             closecoord_netcdf(coord);
           }
           else
             ext.lon_res=ext.lat_res=0.5;
         }
         else
           getextension(&ext,config);
         size_cdf=((long long)((ext.lon_max-ext.lon_min)/ext.lon_res)+1)*
                  ((long long)((ext.lat_max-ext.lat_min)/ext.lat_res)+1);
         iscdf=TRUE;
      }
      sum+=size_cdf*(size/config->total);
    }
    else
      sum+=size;
  }
  return sum;
} /* of 'outputfilesize' */
