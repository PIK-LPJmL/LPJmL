/**************************************************************************************/
/**                                                                                \n**/
/**                   i  n  i  t  i  n  p  u  t  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes input data                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool initinput(Input *input,        /**< Input data */
               const Cell grid[],   /**< LPJ grid */
               int npft,            /**< number of natural PFTs */
               int ncft,            /**< number of crop PFTs */
               const Config *config /**< LPJ configuration */
              )                     /** \return TRUE on error */
{
  if((input->climate=initclimate(grid,config))==NULL)
    return TRUE;
  if(config->withlanduse!=NO_LANDUSE)
  {
    if((input->landuse=initlanduse(ncft,config))==NULL)
      return TRUE;
  }
  else
    input->landuse=NULL; /* no landuse */
  if(config->wateruse)
  {
    if((input->wateruse=initwateruse(config))==NULL)
      return TRUE;
  }
  else
    input->wateruse=NULL;
  if(config->ispopulation)
  {
    if((input->popdens=initpopdens(config))==NULL)
      return TRUE;
  }
  else
    input->popdens=NULL;
  if(config->prescribe_landcover != NO_LANDCOVER)
  {
    if((input->landcover=initlandcover(npft,config))==NULL)
      return TRUE;
  }
  else
    input->landcover=NULL;    
  return FALSE;
} /* of 'initinput' */
