/**************************************************************************************/
/**                                                                                \n**/
/**          o  p  e  n  f  i  l  e  _  n  e  t  c  d  f  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens data file in NetCDF format                                  \n**/
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

#define error(var,rc) if(rc) {if(isroot(*config))fprintf(stderr,"ERROR403: Cannot read '%s' in '%s': %s.\n",var,filename,nc_strerror(rc)); free_netcdf(file->ncid); return TRUE;}

Bool openfile_netcdf(Climatefile *file,    /* data file */
                     const char *filename, /* filename */
                     const char *var,      /* variable name or NULL */
                     const char *units,    /* units or NULL */
                     const Config *config  /* LPJ configuration */
                    )                      /* returns TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  char *s;
  int rc,time_id,var_id,*time,ndims,*dimids;
  size_t len;
  Bool isopen;
  if(filename==NULL)
    return TRUE;
  /* open NetCDF file for input */
  rc=open_netcdf(filename,&file->ncid,&isopen);
  if(rc)
  {
    fprintf(stderr,"ERROR409: Cannot open '%s': %s.\n",filename,nc_strerror(rc));
    return TRUE;
  }
  rc=nc_inq_varid(file->ncid,TIME_NAME,&var_id);
  error("time",rc);
  rc=nc_inq_varndims(file->ncid,var_id,&ndims);
  error("time dim",rc);
  if(ndims!=1)
  {
    fprintf(stderr,"ERROR408: Invalid number of dimensions %d for time in '%s'.\n",ndims,filename);
    free_netcdf(file->ncid);
    return TRUE;
  }
  nc_inq_vardimid(file->ncid,var_id,&time_id);
  if(!nc_inq_attlen(file->ncid, var_id, "units", &len))
  {
    s=malloc(len+1);
    if(s==NULL)
    {
      printallocerr("s");
      free_netcdf(file->ncid);
      return TRUE;
    }
    nc_get_att_text(file->ncid, var_id, "units",s);
    s[len]='\0';
    if(!strcmp(s,"years"))
      file->firstyear=0;
    else if(sscanf(s,"years since %d",&file->firstyear)!=1)
    {
      fprintf(stderr,"ERROR416: No start year in units in '%s'.\n",filename);
      free(s);
      free_netcdf(file->ncid);
      return TRUE;
    }
    free(s);
  }
  else
    file->firstyear=0;
  nc_inq_dimlen(file->ncid,time_id,&len);
  file->nyear=(int)len;
  file->isdaily=FALSE;
  time=newvec(int,len);
  if(time==NULL)
  {
    printallocerr("time");
    free_netcdf(file->ncid);
    return TRUE;
  }
  rc=nc_get_var_int(file->ncid,var_id,time);
  error("time",rc);
  file->firstyear+=time[0];
  free(time);
  /* get variable information */
  if(getvar_netcdf(file,filename,var,units,config))
  {
    free_netcdf(file->ncid);
    return TRUE;
  }
  /* get latitude/longitude information */
  if(getlatlon_netcdf(file,filename,config))
  {
    free_netcdf(file->ncid);
    return TRUE;
  }
  nc_inq_varndims(file->ncid,file->varid,&ndims);
  if(ndims!=3 && ndims!=4)
  {
    fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s'.\n",
            ndims,filename);
    free_netcdf(file->ncid);
    return TRUE;
  }
  if(ndims==3)
    file->var_len=1;
  else
  {
    dimids=newvec(int,ndims);
    if(dimids==NULL)
    {
      printallocerr("dimids");
      free_netcdf(file->ncid);
      return TRUE;
    }
    nc_inq_vardimid(file->ncid,file->varid,dimids);   
    nc_inq_dimlen(file->ncid,dimids[1],&file->var_len);
    free(dimids);
  }
  file->oneyear=FALSE;
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'openfile_netcdf' */
