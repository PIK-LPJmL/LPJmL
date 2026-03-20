/**************************************************************************************/
/**                                                                                \n**/
/**            m p i _ o p e n c l i m a t e _ n e t c d f . c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens climate data file in NetCDF format on one task              \n**/
/**     and broadcasts information to all other tasks                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_NETCDF
#include <netcdf.h>
#endif

Bool mpi_openclimate_netcdf(Climatefile *file,        /**< climate data file */
                            Map **map,                /**< pointer to map or NULL */
                            Attr **attrs,             /**< pointer to array of attributes or NULL */
                            int *n_attr,              /**< size of array attribute */
                            const Filename *filename, /**< filename */
                            const char *units,        /**< units or NULL */
                            const Config *config      /**< LPJ configuration */
                           )                          /** \return TRUE on error */
{
#ifdef USE_NETCDF
  int rc;
#ifdef USE_MPI
  int i,len,len2;
#endif
  if(isroot(*config))
    rc=openclimate_netcdf(file,map,attrs,n_attr,filename->name,filename,units,config);
#ifdef USE_MPI
  /* broadcast return code */
  MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
#endif
  if(rc)
    return TRUE;
#ifdef USE_MPI
  /* broadcast necessary data */
  MPI_Bcast(&file->datatype,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->var_len,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->time_step,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->delta_year,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->isleap,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->n,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->firstyear,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->nyear,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->nlon,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->nlat,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->is360,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->lon_min,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->lat_min,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->lon_res,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->lat_res,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->slope,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->intercept,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->offset,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->missing_value,sizeof(file->missing_value),MPI_BYTE,0,
            config->comm);
  /* distribute map to all tasks */
  if(map!=NULL)
  {
    if(isroot(*config))
      len=(*map==NULL) ? 0 : getmapsize(*map);
    MPI_Bcast(&len,1,MPI_INT,0,config->comm);
    if(len==0)
      *map=NULL;
    else
    {
      if(!isroot(*config))
        *map=newmap(FALSE,len);
      for(i=0;i<getmapsize(*map);i++)
      {
        if(isroot(*config))
          len=(getmapitem(*map,i)==NULL) ? 0 : strlen(getmapitem(*map,i))+1;
        MPI_Bcast(&len,1,MPI_INT,0,config->comm);
        if(len==0)
          getmapitem(*map,i)=NULL;
        else
        {
          if(!isroot(*config))
          {
            getmapitem(*map,i)=malloc(len);
            check(getmapitem(*map,i));
          }
          MPI_Bcast(getmapitem(*map,i),len,MPI_CHAR,0,config->comm);
        }
      }
    }
  }
  /* distribute global attributes to all tasks */
  if(attrs!=NULL)
  {
    if(isroot(*config))
      len=(*attrs==NULL) ? 0 : *n_attr;
    MPI_Bcast(&len,1,MPI_INT,0,config->comm);
    if(n_attr!=NULL)
      *n_attr=len;
    if(len==0)
      *attrs=NULL;
    else
    {
      if(!isroot(*config))
        *attrs=newvec(Attr,len);
      for(i=0;i<len;i++)
      {
        if(isroot(*config))
          len2=strlen((*attrs)[i].name)+1;
        MPI_Bcast(&len2,1,MPI_INT,0,config->comm);
        if(!isroot(*config))
        {
          (*attrs)[i].name=malloc(len2);
          check((*attrs)[i].name);
        }
        MPI_Bcast((*attrs)[i].name,len2,MPI_CHAR,0,config->comm);
        if(isroot(*config))
          len2=strlen((*attrs)[i].value)+1;
        MPI_Bcast(&len2,1,MPI_INT,0,config->comm);
        if(!isroot(*config))
        {
          (*attrs)[i].value=malloc(len2);
          check((*attrs)[i].value);
        }
        MPI_Bcast((*attrs)[i].value,len2,MPI_CHAR,0,config->comm);
      }
    }
  }
#endif
  return FALSE;
#else
  if(isroot(*config))
    fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif

} /* of 'mpi_openclimate_netcdf' */
