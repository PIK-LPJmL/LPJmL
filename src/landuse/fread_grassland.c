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

static Bool readrotation(FILE *file,const char *name,Rotation *rotation,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readint(file,"grazing_days",&rotation->grazing_days,swap))
    return TRUE;
  if(readint(file,"recovery_dates",&rotation->recovery_days,swap))
    return TRUE;
  if(readint(file,"paddocks",&rotation->paddocks,swap))
    return TRUE;
  if(readint(file,"mode",(int *)(&rotation->mode),swap))
    return TRUE;
  return readendstruct(file);
} /* of 'readrotation' */

Bool fread_grassland(FILE *file,   /**< pointer to binary file */
                     Stand *stand, /**< stand pointer */
                     Bool swap     /**< byte order has to be changed */
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
  if(fread_irrigation(file,"irrigation",&grassland->irrigation,swap))
    return TRUE;
  if(readint(file,"growing_days",&stand->growing_days,swap))
    return TRUE;
  if(readreal(file,"deficit_lsu_ne",&grassland->deficit_lsu_ne,swap))
    return TRUE;
  if(readreal(file,"deficit_lsu_mp",&grassland->deficit_lsu_mp,swap))
    return TRUE;
  return readrotation(file,"rotation",&grassland->rotation,swap);
} /* of 'fread_grassland' */
