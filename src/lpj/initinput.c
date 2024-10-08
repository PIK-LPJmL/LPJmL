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
               Config *config       /**< LPJ configuration */
              )                     /** \return TRUE on error */
{
  if((input->climate=initclimate(grid,config))==NULL)
    return TRUE;
  if(config->extflow)
  {
    if((input->extflow=initextflow(config))==NULL)
      return TRUE;
  }
  else
    input->extflow=NULL;
  if(config->withlanduse!=NO_LANDUSE)
  {
    if((input->landuse=initlanduse(config))==NULL)
      return TRUE;
  }
  else
    input->landuse=NULL; /* no landuse */
  if(config->wateruse)
  {
    if((input->wateruse=initwateruse(&config->wateruse_filename,config))==NULL)
      return TRUE;
#ifdef IMAGE
    if((input->wateruse_wd=initwateruse(&config->wateruse_wd_filename,config))==NULL)
      return TRUE;
#endif
  }
  else
  {
    input->wateruse=NULL;
#ifdef IMAGE
    input->wateruse_wd=NULL;
#endif
  }
  if(config->ispopulation)
  {
    if((input->popdens=initpopdens(config))==NULL)
      return TRUE;
  }
  else
    input->popdens=NULL;
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if((input->human_ignition=inithumanignition(config))==NULL)
      return TRUE;
  }
  else
    input->human_ignition=NULL;
  if(config->prescribe_landcover != NO_LANDCOVER)
  {
    if((input->landcover=initlandcover(npft,config))==NULL)
      return TRUE;
  }
  else
    input->landcover=NULL;
  return FALSE;
} /* of 'initinput' */
