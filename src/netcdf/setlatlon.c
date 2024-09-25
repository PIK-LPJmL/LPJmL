/**************************************************************************************/
/**                                                                                \n**/
/**                s  e  t  l  a  t  l  o  n  .  c                                 \n**/
/**                                                                                \n**/
/**     Function creates lat/lon vectors for NetCDF output                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool setlatlon(double **lat,            /**< latitude array allocated and set */
               double **lon,            /**< longitude array allocated and set */
               double **lat_bnds,       /**< latitude boundary array allocated and set */
               double **lon_bnds,       /**< longitude boundary array allocated and set */
               const Coord_array *array /**< coordinate array of LPJ grid */
              )                         /** \return TRUE on error */
{
  int i;
  *lon=newvec(double,array->nlon);
  if(*lon==NULL)
  {
    printallocerr("lon");
    return TRUE;
  }
  *lon_bnds=newvec(double,2*array->nlon);
  if(*lon_bnds==NULL)
  {
    printallocerr("lon_bnds");
    free(*lon);
    return TRUE;
  }
  *lat=newvec(double,array->nlat);
  if(*lat==NULL)
  {
    printallocerr("lat");
    free(*lon);
    return TRUE;
  }
  *lat_bnds=newvec(double,2*array->nlat);
  if(*lat_bnds==NULL)
  {
    printallocerr("lat_bnds");
    free(*lon);
    free(*lon_bnds);
    free(*lat);
    return TRUE;
  }
  for(i=0;i<array->nlon;i++)
  {
    (*lon)[i]=array->lon_min+i*array->lon_res;
    (*lon_bnds)[2*i]=array->lon_min-array->lon_res*0.5+i*array->lon_res;
    (*lon_bnds)[2*i+1]=array->lon_min+array->lon_res*0.5+i*array->lon_res;
  }
  if(array->rev_lat)
    for(i=0;i<array->nlat;i++)
    {
      (*lat)[i]=array->lat_min+(array->nlat-1-i)*array->lat_res;
      (*lat_bnds)[2*i]=array->lat_min+array->lat_res*0.5+(array->nlat-1-i)*array->lat_res;
      (*lat_bnds)[2*i+1]=array->lat_min-array->lat_res*0.5+(array->nlat-1-i)*array->lat_res;
    }
  else
    for(i=0;i<array->nlat;i++)
    {
      (*lat)[i]=array->lat_min+i*array->lat_res;
      (*lat_bnds)[2*i]=array->lat_min-array->lat_res*0.5+i*array->lat_res;
      (*lat_bnds)[2*i+1]=array->lat_min+array->lat_res*0.5+i*array->lat_res;
    }
  return FALSE;
} /* of 'setlatlon' */
