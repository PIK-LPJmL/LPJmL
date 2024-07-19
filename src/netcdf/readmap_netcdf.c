/**************************************************************************************/
/**                                                                                \n**/
/**              r  e  a  d  m  a  p  _  n  e  t  c  d   f  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads string array from NetCDF file                               \n**/
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

Map *readmap_netcdf(int ncid,        /**< id of NetCDF file */
                    const char *name /**< name of map to read */
                   )                 /** \return Map array or NULL */
{
  Map *map=NULL;
  size_t len,offset[2],count[2];
  char *s;
  double *d;
  nc_type type;
  int i,rc,var_id,ndims,*dimids;
  rc=nc_inq_varid(ncid,name,&var_id);
  if(!rc)
  {
    nc_inq_vartype(ncid,var_id,&type);
    if(type==NC_CHAR)
    {
      nc_inq_varndims(ncid,var_id,&ndims);
      if(ndims!=2)
      {
        fprintf(stderr,"ERROR408: Invalid number of dimensions %d for map '%s', must be 2.\n",
                ndims,name);
        return NULL;
      }
      dimids=newvec(int,ndims);
      if(dimids==NULL)
      {
        printallocerr("dimids");
        return NULL;
      }
      nc_inq_vardimid(ncid,var_id,dimids);
      nc_inq_dimlen(ncid,dimids[0],&len);
      map=newmap(FALSE,len);
      if(map==NULL)
      {
        printallocerr("map");
        free(dimids);
        return NULL;
      }
      nc_inq_dimlen(ncid,dimids[1],&len);
      s=malloc(len);
      if(s==NULL)
      {
        printallocerr("s");
        free(dimids);
        return NULL;
      }
      offset[1]=0;
      count[0]=1;
      count[1]=len;
      for(i=0;i<getmapsize(map);i++)
      {
        offset[0]=i;
        rc=nc_get_vara_text(ncid,var_id,offset,count,s);
        if(!strcmp(s,NULL_NAME) || strlen(s)==0)
          getmapitem(map,i)=NULL;
        else
        {
          getmapitem(map,i)=strdup(s);
          if(getlistitem(map->list,i)==NULL)
          {
            printallocerr("name");
            return NULL;
          }
        }
      }
      free(s);
      free(dimids);
    }
    else if(type==NC_DOUBLE)
    {
      nc_inq_varndims(ncid,var_id,&ndims);
      if(ndims!=1)
      {
        fprintf(stderr,"ERROR408: Invalid number of dimensions %d for map '%s', must be 1.\n",
                ndims,name);
        return NULL;
      }
      dimids=newvec(int,ndims);
      if(dimids==NULL)
      { 
        printallocerr("dimids");
        return NULL;
      }
      nc_inq_vardimid(ncid,var_id,dimids);
      nc_inq_dimlen(ncid,dimids[0],&len);
      map=newmap(TRUE,len);
      if(map==NULL)
      { 
        free(dimids);
        printallocerr("map");
        return NULL;
      }
      count[0]=1;
      for(i=0;i<getmapsize(map);i++)
      {
        offset[0]=i;
        d=new(double);
        if(d==NULL)
        {
          free(dimids);
          printallocerr("map");
          return NULL;
        }
        rc=nc_get_vara_double(ncid,var_id,offset,count,d);
        getmapitem(map,i)=d;
      }
      free(dimids);
    }
    else
    {
      fprintf(stderr,"ERROR428: Invalid dataype for map '%s', must be char or double.\n",
              name);
      return NULL;
    }
  }
  return map;
} /* of 'readmap_netcdf' */

#endif
