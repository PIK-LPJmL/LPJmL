/**************************************************************************************/
/**                                                                                \n**/
/**       g  e  t  g  l  o  b  a  l  a  t  t  r  s _  n  e  t  c  d  f  .  c       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads all global attributes of NetCDF file                        \n**/
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

Bool getglobalattrs_netcdf(int ncid,     /**< Netcdf id */
                           Attr **attrs, /**< pointer to array of attributes or NULL */
                           int *n_attr   /**< size of array attribute */
                          )              /** \return TRUE on error */
{
#ifdef USE_NETCDF
  int i,len;
  char name[NC_MAX_NAME];
  if(attrs==NULL)
    return FALSE;
  if(nc_inq_natts(ncid,&len))
    *n_attr=0;
  else
  {
    *attrs=newvec(Attr,len);
    *n_attr=0;
    if(*attrs==NULL)
    {
      printallocerr("attrs");
      return TRUE;
    }
    for(i=0;i<len;i++)
    {
      if(!nc_inq_attname(ncid,NC_GLOBAL,i,name))
      {
        (*attrs)[*n_attr].value=getattr_netcdf(ncid,NC_GLOBAL,name);
        if((*attrs)[*n_attr].value!=NULL)
          (*attrs)[(*n_attr)++].name=strdup(name);
      }
    }
  }
  return FALSE;
#else
  if(attrs!=NULL)
  {
    *attrs=NULL;
    *n_attr=0;
  }
  return TRUE;
#endif
} /* 'getglobalattrs_netcdf' */
