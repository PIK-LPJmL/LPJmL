/**************************************************************************************/
/**                                                                                \n**/
/**                c  l  o  s  e  c  l  i  m  a  t  e  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function closes open file                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void closeclimatefile(Climatefile *file, /**< pointer to climate data file */
                      Bool isroot        /**< task is root task (TRUE/FALSE) */
                     )                   /** \return void */
{
  if(file->fmt!=FMS)
  {
    if(file->fmt==CDF)
      closeclimate_netcdf(file,isroot);
    else
      fclose(file->file);
  }
} /* of 'closeclimatefile' */
