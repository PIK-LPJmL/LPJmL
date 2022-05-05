/**************************************************************************************/
/**                                                                                \n**/
/**                 r  e  a  d  i  n  p  u  t  d  a  t  a  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads real data from clm or NetCDF file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readinputdata(Infile *file,            /**< Pointer to data file */
                   Real *data,              /**< data to read from file */ 
                   const Coord *coord,      /**< coordinate of cell */
                   int cell,                /**< cell id */
                   const Filename *filename /**< filename of data */
              )                             /** \return TRUE on error */
{
  char *name;
  if(file->fmt==CDF)
  {
    if(readinput_netcdf(file->cdf,data,coord))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              filename->name,cell);
      return TRUE;
    }
  }
  else
  {
    if(readrealvec(file->file,data,0,file->scalar,1,file->swap,file->type))
    {
      name=getrealfilename(filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              name,cell);
      free(name);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'readinputdata' */
