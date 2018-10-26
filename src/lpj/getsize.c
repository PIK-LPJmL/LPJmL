/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  s  i  z  e  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines size of output for one year                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int getnyear(int index /**< index of output file */
            )          /** \return number of items per year */
{
  if(index==REGION || index==COUNTRY || index==GRID)
    return 0;
  if(ismonthlyoutput(index))
    return 12;
  if(isdailyoutput(index))
    return NDAYYEAR;
  return 1;
} /* of 'getnyear' */

size_t getsize(int index,           /**< index of output file */
               const Config *config /**< LPJ configuration */
              )                     /** \return size of output written in one year */
{
  size_t size;
  size=getnyear(config->outputvars[index].id);
  size*=outputsize(config->outputvars[index].id,
                   config->npft[GRASS]+config->npft[TREE],
                   config->nbiomass,
                   config->npft[CROP]);
  size*=typesizes[getoutputtype(config->outputvars[index].id)];
  if(config->outputvars[index].id==ADISCHARGE)
    size*=config->nall;
  else
    size*=config->total;
  return size;
} /* of 'getsize' */
