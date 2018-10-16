/**************************************************************************************/
/**                                                                                \n**/
/**    r  e  a  d  c  l  i  m  a  t  e  _  n  e  t  c  d  f  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads climate data in NetCDF format                               \n**/
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

static Bool myopen_netcdf(Climatefile *file,int year,const Config *config)
{
  int rc,ndims;
  char *s;
  s=malloc(strlen(file->filename)+12);
  if(s==NULL)
  {
    printallocerr("filename");
    return TRUE;
  }    
  sprintf(s,file->filename,year+file->firstyear);
  rc=nc_open(s,NC_NOWRITE,&file->ncid);
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR409: Cannot open '%s': %s.\n",s,nc_strerror(rc));
    free(s);
    return TRUE;
  }
  if(getvar_netcdf(file,s,file->var,file->units,config))
  {
    nc_close(file->ncid); 
    free(s);
    return TRUE;
  }
  if(getlatlon_netcdf(file,s,config))
  {
    free(s);
    nc_close(file->ncid);
    return TRUE;
  }
  nc_inq_varndims(file->ncid,file->varid,&ndims);
  if(ndims!=3)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s'.\n",
              ndims,s);
    nc_close(file->ncid);
    free(s);
    return TRUE;
  }
  free(s);
  return FALSE;
} /* of 'myopen_netcdf' */

static Bool openfile(Climatefile *file,int year,const Config *config)
{
  int rc;
  if(isroot(*config))
    rc=myopen_netcdf(file,year,config);
#ifdef USE_MPI
  MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
#endif
  if(rc)
    return TRUE;
#ifdef USE_MPI
  MPI_Bcast(&file->datatype,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->nlon,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->nlat,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->is360,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->lon_min,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->lat_min,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->lon_res,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->lat_res,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->slope,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->intercept,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->offset,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->missing_value,sizeof(file->missing_value),MPI_BYTE,0,
            config->comm);
#endif
  return FALSE;
} /* of 'openfile' */

#endif

Bool readclimate_netcdf(Climatefile *file,   /**< climate data file */
                        Real data[],         /**< pointer to data read in */
                        const Cell grid[],   /**< LPJ grid */
                        int year,            /**< year (0..nyear-1) */
                        const Config *config /**< LPJ configuration */
                       )                     /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i,cell,rc;
  float *f;
  double *d;
  short *s;
  int size;
  size_t offsets[3];
  size_t counts[3];
  String line;
  size=isdaily(*file) ? NDAYYEAR : NMONTH;
  if(file->oneyear)
  {
    if(openfile(file,year,config))
      return TRUE;
    offsets[0]=0;
  }
  else
  {
    offsets[0]=year*size;
    if(isdaily(*file) && file->isleap)
      offsets[0]+=nleapyears(file->firstyear,year+file->firstyear);
  }
  offsets[1]=offsets[2]=0;
  counts[0]=size;
  counts[1]=file->nlat;
  counts[2]=file->nlon;
  switch(file->datatype)
  {
    case LPJ_FLOAT:
      f=newvec(float,size*file->nlon*file->nlat);
      if(f==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_float(file->ncid,file->varid,offsets,counts,f)))
        {
          fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
                  nc_strerror(rc)); 
        }
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(f);
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(f,size*file->nlon*file->nlat,MPI_FLOAT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            if(file->oneyear)
              nc_close(file->ncid);
            return TRUE;
          }
          for(i=0;i<size;i++)
          {
            if(f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.f)
            {
              fprintf(stderr,"ERROR423: Missing value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(f);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }
            else if(isnan(f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]))
            {
              fprintf(stderr,"ERROR434: Invalid value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(f);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }

            data[cell*size+i]=file->slope*f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]+file->intercept;
          }
        }
      free(f);
      break;
    case LPJ_DOUBLE:
      d=newvec(double,size*file->nlon*file->nlat);
      if(d==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_double(file->ncid,file->varid,offsets,counts,d)))
        {
          fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
                  nc_strerror(rc));
        }
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(d);
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(d,size*file->nlon*file->nlat,MPI_DOUBLE,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            free(d);
            if(file->oneyear)
              nc_close(file->ncid);
            return TRUE;
          }
          for(i=0;i<size;i++)
          {
            if(d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.d)
            {
              fprintf(stderr,"ERROR423: Missing value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(d);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }
            else if(isnan(d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]))
            {
              fprintf(stderr,"ERROR434: Invalid value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(d);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }
            data[cell*size+i]=file->slope*d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]+file->intercept;
          }
        }
      free(d);
      break;
    case LPJ_SHORT:
      s=newvec(short,size*file->nlon*file->nlat);
      if(s==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_short(file->ncid,file->varid,offsets,counts,s)))
        {
          fprintf(stderr,"ERROR421: Cannot read short data: %s.\n",
                  nc_strerror(rc));
        }
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(s);
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(s,size*file->nlon*file->nlat,MPI_SHORT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            if(file->oneyear && isroot(*config))
              nc_close(file->ncid);
            return TRUE;
          }
          for(i=0;i<size;i++)
          {
            if(s[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.s)
            {
              fprintf(stderr,"ERROR423: Missing value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(s);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }
            data[cell*size+i]=file->slope*s[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]+file->intercept;
          }
        }
      free(s);
      break;
    default:
      if(isroot(*config))
        fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      if(file->oneyear && isroot(*config))
        nc_close(file->ncid);
      return TRUE;
  }
  if(file->oneyear && isroot(*config))
    nc_close(file->ncid);
  return FALSE;
#else
  if(isroot(*config))
    fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'readclimate_netcdf' */

Bool readintclimate_netcdf(Climatefile *file,   /* climate data file */
                           int data[],          /* pointer to data read in */
                           const Cell grid[],   /* LPJ grid */
                           int year,            /* year (0..nyear-1) */
                           const Config *config /* LPJ configuration */
                          )                     /* returns TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i,cell,rc;
  int *f;
  short *s;
  int size;
  size_t offsets[3];
  size_t counts[3];
  String line;
  size=isdaily(*file) ? NDAYYEAR : NMONTH;
  if(file->oneyear)
  {
    if(openfile(file,year,config))
      return TRUE;
    offsets[0]=0;
  }
  else
  {
    offsets[0]=year*size;
    if(isdaily(*file) && file->isleap)
      offsets[0]+=nleapyears(file->firstyear,year+file->firstyear);
  }
  offsets[1]=offsets[2]=0;
  counts[0]=size;
  counts[1]=file->nlat;
  counts[2]=file->nlon;
  switch(file->datatype)
  {
    case LPJ_INT:
      f=newvec(int,size*file->nlon*file->nlat);
      if(f==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_int(file->ncid,file->varid,offsets,counts,f)))
        {
          fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
                nc_strerror(rc)); 
        }
      }
      else 
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(f);
        if(file->oneyear)
          nc_close(file->ncid);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(f,size*file->nlon*file->nlat,MPI_INT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            if(file->oneyear && isroot(*config))
              nc_close(file->ncid);
            return TRUE;
          }
          for(i=0;i<size;i++)
          {
            if(f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.i)
            {
              fprintf(stderr,"ERROR423: Missing value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(f);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }
            data[cell*size+i]=f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]];
          }
        }
      free(f);
      break;
    case LPJ_SHORT:
      s=newvec(short,size*file->nlon*file->nlat);
      if(s==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_short(file->ncid,file->varid,offsets,counts,s)))
        {
          fprintf(stderr,"ERROR421: Cannot read short data: %s.\n",
                  nc_strerror(rc)); 
        }
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(s);
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return TRUE;
      }
#ifdef USE_MPI
      MPI_Bcast(s,size*file->nlon*file->nlat,MPI_SHORT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            if(file->oneyear && isroot(*config))
              nc_close(file->ncid);
            return TRUE;
          }
          for(i=0;i<size;i++)
          {
            if(s[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.s)
            {
              fprintf(stderr,"ERROR423: Missing value for cell=%d (%s) at %s %d.\n",
                      cell+config->startgrid,sprintcoord(line,&grid[cell].coord),isdaily(*file) ? "day" : "month",i+1);
              free(s);
              if(file->oneyear && isroot(*config))
                nc_close(file->ncid);
              return TRUE;
            }
            data[cell*size+i]=s[file->nlon*(i*file->nlat+offsets[1])+offsets[2]];
          }
        }
      free(s);
      break;
    default:
      if(isroot(*config))
        fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      if(file->oneyear && isroot(*config))
        nc_close(file->ncid);
      return TRUE;
  }
  if(file->oneyear && isroot(*config))
    nc_close(file->ncid);
  return FALSE;
#else
  if(isroot(*config))
    fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'readintclimate_netcdf' */

int checkvalidclimate_netcdf(Climatefile *file,   /* climate data file */
                             Cell grid[],         /* LPJ grid */
                             int year,            /* year (0..nyear-1) */
                             const Config *config /* LPJ configuration */
                            )  /* returns number of invalid cells or -1 */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int i,cell,rc;
  float *f;
  short *s;
  double *d;
  int size,count;
  size_t offsets[3];
  size_t counts[3];
  String line;
  size=isdaily(*file) ? NDAYYEAR : NMONTH;
  if(file->oneyear)
  {
    if(openfile(file,year,config))
      return -1;
    offsets[0]=0;
  }
  else
  {
    offsets[0]=year*size;
    if(isdaily(*file) && file->isleap)
      offsets[0]+=nleapyears(file->firstyear,year+file->firstyear);
  }
  offsets[1]=offsets[2]=0;
  counts[0]=size;
  counts[1]=file->nlat;
  counts[2]=file->nlon;
  count=0;
  switch(file->datatype)
  {
    case LPJ_FLOAT:
      f=newvec(float,size*file->nlon*file->nlat);
      if(f==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_float(file->ncid,file->varid,offsets,counts,f)))
          fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
                  nc_strerror(rc)); 
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(f);
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return -1;
      }
#ifdef USE_MPI
      MPI_Bcast(f,size*file->nlon*file->nlat,MPI_FLOAT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            if(file->oneyear && isroot(*config))
              nc_close(file->ncid);
            return -1;
          }
          for(i=0;i<size;i++)
          {
            if(f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.f)
            {
              count++;
              grid[cell].skip=TRUE;
              break;
            }
          }
        }
      free(f);
      break;
    case LPJ_DOUBLE:
      d=newvec(double,size*file->nlon*file->nlat);
      if(d==NULL)
      {
        printallocerr("data");
        rc=TRUE;
      }
      else if(isroot(*config))
      {
        if((rc=nc_get_vara_double(file->ncid,file->varid,offsets,counts,d)))
          fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
                  nc_strerror(rc));
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
      {
        free(d);
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return -1;
      }
#ifdef USE_MPI
      MPI_Bcast(d,size*file->nlon*file->nlat,MPI_DOUBLE,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            free(d);
            if(file->oneyear && isroot(*config))
              nc_close(file->ncid);
            return -1;
          }
          for(i=0;i<size;i++)
          {
            if(d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.d)
            {
              count++;
              grid[cell].skip=TRUE;
              break;
            }
          }
        }
      free(d);
      break;
    case LPJ_SHORT:
      s=newvec(short,size*file->nlon*file->nlat);
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
        if(file->oneyear && isroot(*config))
          nc_close(file->ncid);
        return -1;
      }
#ifdef USE_MPI
      MPI_Bcast(s,size*file->nlon*file->nlat,MPI_SHORT,0,config->comm);
#endif
      for(cell=0;cell<config->ngridcell;cell++)
        if(!grid[cell].skip)
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
            if(file->oneyear && isroot(*config))
              nc_close(file->ncid);
            return -1;
          }
          for(i=0;i<size;i++)
          {
            if(s[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.s)
            {
              count++;
              grid[cell].skip=TRUE;
              break;
            }
          }
        }
      free(s);
      break;
    default:
      if(isroot(*config))
        fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      if(file->oneyear && isroot(*config))
        nc_close(file->ncid);
      return -1;
  }
  if(file->oneyear && isroot(*config))
    nc_close(file->ncid);
  return count;
#else
  if(isroot(*config))
    fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return -1;
#endif
} /* of 'checkvalidclimate_netcdf' */
