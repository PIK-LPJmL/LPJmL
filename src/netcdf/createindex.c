/**************************************************************************************/
/**                                                                                \n**/
/**                c  r  e  a  t  e  i  n  d  e  x  .  c                           \n**/
/**                                                                                \n**/
/**     Function creates index vector for NetCDF output                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Coord_array *createindex(const Coord *grid, /**< coordinate array of LPJ grid */
                         int ngrid,         /**< size of coordinate array */
                         Coord resolution,  /**< lon, lat resolution (deg) */
                         Bool global        /**< use global grid */
                        )                   /** \return pointer to lat, lon, index  arrays or NULL */
{
  Coord_array *array;
  Real lon_max,lat_max;
  int cell;
  array=new(Coord_array);
  if(array==NULL)
    return NULL;
  array->index=newvec(int,ngrid);
  if(array->index==NULL)
  {
    printallocerr("index");
    free(array);
    return NULL;
  }
  if(global)
  {
    array->lon_min=-180+0.5*resolution.lon;
    lon_max=180-0.5*resolution.lon;
    array->lat_min=-90+0.5*resolution.lat;
    lat_max=90-0.5*resolution.lat;
  }
  else
  {
    array->lon_min=array->lat_min=1000;
    lon_max=lat_max=-1000;
    for(cell=0;cell<ngrid;cell++)
    {
      if(array->lon_min>grid[cell].lon)
        array->lon_min=grid[cell].lon;
      if(lon_max<grid[cell].lon)
        lon_max=grid[cell].lon;
      if(array->lat_min>grid[cell].lat)
        array->lat_min=grid[cell].lat;
      if(lat_max<grid[cell].lat)
        lat_max=grid[cell].lat;
    }
  }
  array->nlon=(int)((lon_max-array->lon_min)/resolution.lon+0.5)+1;
  array->nlat=(int)((lat_max-array->lat_min)/resolution.lat+0.5)+1;
  for(cell=0;cell<ngrid;cell++)
  {
   array->index[cell]=(int)((grid[cell].lon-array->lon_min)/resolution.lon+0.5)+
                      (int)((grid[cell].lat-array->lat_min)/resolution.lat+0.5)*array->nlon;
#ifdef SAFE
   if(array->index[cell]<0 || array->index[cell]>=array->nlon*array->nlat)
     fprintf(stderr,"Invalid index %d.\n",array->index[cell]);
#endif
  }
  return array;
} /* of 'createindex' */
