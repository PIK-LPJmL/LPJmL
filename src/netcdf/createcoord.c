/**************************************************************************************/
/**                                                                                \n**/
/**                c  r  e  a  t  e  c  o  o  r  d  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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

Coord_array *createcoord(Outputfile *output,
                         const Cell grid[],
                         const Config *config)
{
  Coord_array *array;
  Real *lon=NULL,*lat=NULL;
#ifdef USE_MPI
  Real *vec;
#endif
  Real lon_max,lat_max;
  int cell,count;
  Bool iserr;
  if(config->total==0)
  {
    if(isroot(*config))
      fputs("ERROR429: No cell with valid soil code found in create_coord().\n",stderr);
    return NULL;
  }
  iserr=FALSE;
  array=new(Coord_array);
  if(array==NULL)
  {
    printallocerr("index");    
    iserr=TRUE;
  }
  if(isroot(*config))
  {
    lat=newvec(Real,config->total);
    if(lat==NULL)
    {
      printallocerr("lat");
      iserr=TRUE;
    }
    lon=newvec(Real,config->total);
    if(lon==NULL)
    {
      printallocerr("lon");
      iserr=TRUE;
    }
  }
  else
    array->index=NULL;
  count=0;
#ifdef USE_MPI
  vec=newvec(Real,config->count);
  if(vec==NULL)
  {
    printallocerr("vec");
    iserr=TRUE;
  }
  if(iserror(iserr,config))
  {
    free(vec);
    free(array);
    if(isroot(*config))
    {
      free(lat);
      free(lon);
    }
    return NULL;
  }
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
      vec[count++]=grid[cell].coord.lon;
  MPI_Gatherv(vec,config->count,
              (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
              lon,output->counts,output->offsets,
              (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
              0,config->comm);
  count=0;
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
      vec[count++]=grid[cell].coord.lat;
  MPI_Gatherv(vec,config->count,
              (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
              lat,output->counts,output->offsets,
              (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
              0,config->comm);
  free(vec);
#else
  if(iserr)
  {
    free(array);
    free(lat);
    free(lon);
    return NULL;
  }
  for(cell=0;cell<config->ngridcell;cell++)
    if(!grid[cell].skip)
    {
      lon[count]=grid[cell].coord.lon;
      lat[count++]=grid[cell].coord.lat;
    }
#endif
  if(isroot(*config))
  {
    array->index=newvec(int,config->total);
    if(array->index==NULL)
    {
      printallocerr("index");    
      iserr=TRUE;
    }
    else
    {
      if(config->global_netcdf)
      {
        array->lon_min=-180+0.5*config->resolution.lon;
        lon_max=180-0.5*config->resolution.lon;
        array->lat_min=-90+0.5*config->resolution.lat;
        lat_max=90-0.5*config->resolution.lat;;
      }
      else
      {
        array->lon_min=array->lat_min=1000;
        lon_max=lat_max=-1000;
        for(cell=0;cell<config->total;cell++)
        {
          if(array->lon_min>lon[cell])
            array->lon_min=lon[cell]; 
          if(lon_max<lon[cell])
            lon_max=lon[cell]; 
          if(array->lat_min>lat[cell])
            array->lat_min=lat[cell]; 
          if(lat_max<lat[cell])
            lat_max=lat[cell];
        }
      }
      array->nlon=(int)((lon_max-array->lon_min)/config->resolution.lon+0.5)+1;
      array->nlat=(int)((lat_max-array->lat_min)/config->resolution.lat+0.5)+1;
      for(cell=0;cell<config->total;cell++)
      {
        array->index[cell]=(int)((lon[cell]-array->lon_min)/config->resolution.lon+0.5)+
                           (int)((lat[cell]-array->lat_min)/config->resolution.lat+0.5)*array->nlon;
#ifdef SAFE
        if(array->index[cell]<0 || array->index[cell]>=array->nlon*array->nlat)
        {
          fprintf(stderr,"ERROR433: Invalid index %d in createcoord().\n",array->index[cell]);
          free(array->index);
          break;
        }
#endif
      }
      iserr=(cell<config->total);
    }
    free(lon);
    free(lat);
  }
#ifdef USE_MPI
  MPI_Bcast(&iserr,1,MPI_INT,0,config->comm);
#endif
  if(iserr)
  {
    free(array);
    array=NULL;
  }  
  return array;
} /* of 'createcoord' */
