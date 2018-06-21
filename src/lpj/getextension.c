/**************************************************************************************/
/**                                                                                \n**/
/**                g  e  t  e  x  t  e  n  s  i  o  n  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets extensions of LPJ grid                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool getextension(Extension *ext,const Config *config)
{
  Coord coord;
  Coordfile file_coord;
  float lon,lat;
  int i;
  file_coord=opencoord(&config->coord_filename,isroot(*config));
  if(file_coord==NULL)
  {
    ext->lat_min=ext->lon_min=0;
    ext->lat_max=ext->lon_max=0;
    return TRUE;
  }
  getcellsizecoord(&lon,&lat,file_coord);
  ext->lon_res=lon;
  ext->lat_res=lat;
  if(config->global_netcdf)
  {
    ext->lon_min=-180+0.5*lon;
    ext->lon_max=180-0.5*lon;
    ext->lat_min=-90+0.5*lat;
    ext->lat_max=90-0.5*lat;
  }
  else
  {
    if(seekcoord(file_coord,config->firstgrid))
    {
      ext->lat_min=ext->lon_min=0;
      ext->lat_max=ext->lon_max=0;
      fprintf(stderr,
              "ERROR109: Cannot seek in coordinate file to position %d.\n",
              config->firstgrid);
      closecoord(file_coord);
      return TRUE;
    }  
    ext->lat_min=ext->lon_min=1000;
    ext->lat_max=ext->lon_max=-1000;
    for(i=0;i<config->nall;i++)
    {
      if(readcoord(file_coord,&coord,&config->resolution))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s'.\n",
                config->coord_filename.name);
        if(i==0)
          ext->lat_min=ext->lon_min=ext->lat_max=ext->lon_max=0;
        break;
      }
      if(ext->lon_min>coord.lon)
      ext->lon_min=coord.lon; 
      if(ext->lon_max<coord.lon)
        ext->lon_max=coord.lon; 
      if(ext->lat_min>coord.lat)
        ext->lat_min=coord.lat; 
      if(ext->lat_max<coord.lat)
        ext->lat_max=coord.lat;
    }
  }
  closecoord(file_coord);
  return FALSE;
} /* of 'getextension' */
