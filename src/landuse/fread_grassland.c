/**************************************************************************************/
/**                                                                                \n**/
/**        f  r  e  a  d  _  g  r  a  s  s  l  a  n  d  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads grassland data of stand                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

static Bool readrotation(Bstruct file,const char *name,Rotation *rotation)
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  if(bstruct_readint(file,"grazing_days",&rotation->grazing_days))
    return TRUE;
  if(bstruct_readint(file,"recovery_dates",&rotation->recovery_days))
    return TRUE;
  if(bstruct_readint(file,"paddocks",&rotation->paddocks))
    return TRUE;
  if(bstruct_readint(file,"mode",(int *)(&rotation->mode)))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'readrotation' */

Bool fread_grassland(Bstruct file, /**< pointer to restart file */
                     Stand *stand  /**< stand pointer */
                    )              /** \return TRUE on error */
{
  Grassland *grassland;
  grassland=new(Grassland);
  stand->data=grassland;
  if(grassland==NULL)
  {
    printallocerr("grassland");
    return TRUE;
  }
  if(fread_irrigation(file,"irrigation",&grassland->irrigation))
    return TRUE;
  if(bstruct_readint(file,"growing_days",&stand->growing_days))
    return TRUE;
  if(bstruct_readreal(file,"deficit_lsu_ne",&grassland->deficit_lsu_ne))
    return TRUE;
  if(bstruct_readreal(file,"deficit_lsu_mp",&grassland->deficit_lsu_mp))
    return TRUE;
  return readrotation(file,"rotation",&grassland->rotation);
} /* of 'fread_grassland' */
