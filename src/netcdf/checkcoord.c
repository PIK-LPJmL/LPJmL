/**************************************************************************************/
/**                                                                                \n**/
/**                c  h  e  c  k  c  o  o  r  d  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks whether LPJ cell is within NetCDF data set                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool checkcoord(const size_t offsets[], /**< offsets in NetCDF file */
                int cell,               /**< cell index */
                const Coord *coord,     /**< cell coordinate */
                const Climatefile *file /**< climate data file */
               )                        /** \return cell is within NetCDF data (TRUE/FALSE) */
{
#if defined(USE_NETCDF)
  String line;
  if(offsets[0]>=file->nlat)
  {
    fprintf(stderr,"ERROR422: Invalid latitude coordinate for cell %d (%s) in data file, must be in [",
            cell,sprintcoord(line,coord));
    if(file->lat_min<0)
      fprintf(stderr,"%.6gS,",-file->lat_min);
    else
      fprintf(stderr,"%.6gN,",file->lat_min);
    if(file->lat_min+file->lat_res*(file->nlat-1)<0)
      fprintf(stderr,"%.6gS].\n",-(file->lat_min+file->lat_res*(file->nlat-1)));
    else
      fprintf(stderr,"%.6gN].\n",file->lat_min+file->lat_res*(file->nlat-1));
     return TRUE;
  }
  if(offsets[1]>=file->nlon)
  {
    fprintf(stderr,"ERROR422: Invalid longitude coordinate for cell %d (%s) in data file, must be in [",
            cell,sprintcoord(line,coord));
    if(file->lon_min<0)
      fprintf(stderr,"%.6gW,",-file->lon_min);
    else
      fprintf(stderr,"%.6gE,",file->lon_min);
    if(file->lon_min+file->lon_res*(file->nlon-1)<0)
      fprintf(stderr,"%.6gW].\n",-(file->lon_min+file->lon_res*(file->nlon-1)));
    else
      fprintf(stderr,"%.6gE].\n",file->lon_min+file->lon_res*(file->nlon-1));
     return TRUE;
  }
#endif
  return FALSE;
} /* of 'checkcoord' */
