/**************************************************************************************/
/**                                                                                \n**/
/**           r  e  a  d  i  n  t  i  n  p  u  t  d  a  t  a  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads int data from clm or NetCDF file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readintinputdata(Infile *file,            /**< Pointer to data file */
                      int *data,               /**< data to read from file */
                      Bool *ismissing,         /**< pointer to bool or NULL */
                      const Coord *coord,      /**< coordinate of cell */
                      int cell,                /**< cell id */
                      const Filename *filename /**< filename of data */
                     )                         /** \return TRUE on error */
{
  char *name;
  Bool missing;
  if(file->fmt==CDF)
  {
    if(readintinput_netcdf(file->cdf,data,coord,&missing))
    {
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              filename->name,cell);
      return TRUE;
    }
    if(ismissing!=NULL)
      *ismissing=missing;
    else if(missing)
    {
      fprintf(stderr,"ERROR190: Missing value in '%s' for cell %d.\n",
              filename->name,cell);
      return TRUE;
    }
  }
  else
  {
    if(readintvec(file->file,data,1,file->swap,file->type))
    {
      name=getrealfilename(filename);
      fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
              name,cell);
      free(name);
      return TRUE;
    }
  }
  return FALSE;
} /* of 'readintinputdata' */
