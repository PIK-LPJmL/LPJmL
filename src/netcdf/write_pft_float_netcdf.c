/**************************************************************************************/
/**                                                                                \n**/
/**           w r i t e _ p f t _ f l o a t _ n e t c d f . c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes PFT output in NetCDF file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF)
#include <netcdf.h>
#endif

Bool write_pft_float_netcdf(const Netcdf *cdf,const float vec[],int year,
                            int pft,int size)
{
#if defined(USE_NETCDF)
  int i,rc,index;
  size_t offsets[4],counts[4];
  float *grid;
  counts[0]=1;
  if(year==NO_TIME)
    index=0;
  else
  {
    index=1;
    counts[1]=1;
    offsets[0]=year;
  }
  counts[index+1]=cdf->index->nlat;
  counts[index+2]=cdf->index->nlon;
  offsets[index]=pft;
  offsets[index+1]=offsets[index+2]=0;
  grid=newvec(float,cdf->index->nlon*cdf->index->nlat);
  if(grid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  for(i=0;i<cdf->index->nlon*cdf->index->nlat;i++)
    grid[i]=cdf->missing_value.f;
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=vec[i];
  rc=nc_put_vara_float(cdf->ncid,cdf->varid,offsets,counts,grid);
  if(rc)
  {
    fprintf(stderr,"ERROR431: Cannot write output data: %s.\n",
            nc_strerror(rc));
    free(grid);
    return TRUE;
  }
  free(grid);
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'write_pft_float_netcdf' */
