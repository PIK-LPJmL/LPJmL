/**************************************************************************************/
/**                                                                                \n**/
/**      r  e  a  d  i  n  t  d  a  t  a  _  n  e  t  c  d  f  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads data in NetCDF format                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>
#endif

Bool readintdata_netcdf(const Climatefile *file,int data[],const Cell grid[],
                        int year,const Config *config)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int cell,rc;
  int *f;
  short *s;
  size_t offsets[4];
  size_t counts[4];
  String line;
  offsets[0]=year-file->firstyear;
  offsets[1]=offsets[2]=offsets[3]=0;
  counts[0]=1;
  counts[1]=file->nlat;
  counts[2]=file->nlon;
  counts[3]=file->var_len;
  switch(file->datatype)
  {
    case LPJ_INT:
      f=newvec(int,file->nlon*file->nlat*file->var_len);
      if(f==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_int(file->ncid,file->varid,offsets,counts,f)))
          fprintf(stderr,"ERROR421: Cannot read int data: %s.\n",
                  nc_strerror(rc)); 
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(f);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(f,file->nlon*file->nlat*file->var_len,MPI_INT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(file->offset)
          offsets[1]=file->offset-(int)((grid[cell].coord.lat-file->lat_min)/file->lat_res+0.5);
        else
          offsets[1]=(int)((grid[cell].coord.lat-file->lat_min)/file->lat_res+0.5);
        if(file->is360 && grid[cell].coord.lon<0)
          offsets[2]=(int)((360+grid[cell].coord.lon-file->lon_min)/file->lon_res+0.5);
        else
          offsets[2]=(int)((grid[cell].coord.lon-file->lon_min)/file->lon_res+0.5);
        if(offsets[1]>=file->nlat || offsets[2]>=file->nlon)
        {
          fprintf(stderr,"ERROR422: Invalid coordinate for cell %d (%s) in data file.\n",
                  cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
          free(f);
          return TRUE;
        }

        if(!grid[cell].skip && f[file->nlon*offsets[1]+offsets[2]]==file->missing_value.i)
        {
          fprintf(stderr,"ERROR423: Missing value for cell=%d (%s).\n",
                  cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
          free(f);
          return TRUE;
        }
        data[cell]=f[file->nlon*offsets[1]+offsets[2]];
      }
      free(f);
      break;
    case LPJ_SHORT:
      s=newvec(short,file->nlon*file->nlat*file->var_len);
      if(s==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_short(file->ncid,file->varid,offsets,counts,s)))
          fprintf(stderr,"ERROR421: Cannot read short data: %s.\n",
                  nc_strerror(rc));
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(s);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(s,file->nlon*file->nlat*file->var_len,MPI_SHORT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(file->offset)
          offsets[1]=file->offset-(int)((grid[cell].coord.lat-file->lat_min)/file->lat_res+0.5);
        else
          offsets[1]=(int)((grid[cell].coord.lat-file->lat_min)/file->lat_res+0.5);
        if(file->is360 && grid[cell].coord.lon<0)
          offsets[2]=(int)((360+grid[cell].coord.lon-file->lon_min)/file->lon_res+0.5);
        else
          offsets[2]=(int)((grid[cell].coord.lon-file->lon_min)/file->lon_res+0.5);
        if(offsets[1]>=file->nlat || offsets[2]>=file->nlon)
        {
          fprintf(stderr,"ERROR422: Invalid coordinate for cell %d (%s) in data file.\n",
                  cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
          free(s);
          return TRUE;
        }

        if(!grid[cell].skip && s[file->nlon*offsets[1]+offsets[2]]==file->missing_value.s)
        {
          fprintf(stderr,"ERROR423: Missing value for cell=%d (%s).\n",
                  cell+config->startgrid,sprintcoord(line,&grid[cell].coord));
          free(s);
          return TRUE;
        }
        data[cell]=s[file->nlon*offsets[1]+offsets[2]];
      }
      free(s);
      break;
    default:
      if(isroot(*config))
        fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      return TRUE;
  } /* of switch(file->type) */
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'readintdata_netcdf' */
