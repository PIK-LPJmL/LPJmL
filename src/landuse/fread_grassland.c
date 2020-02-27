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
  if(fread_irrigation(file,&grassland->irrigation,swap))
    return TRUE;
  freadint1(&stand->growing_days,swap,file);
  freadreal1(&grassland->nr_of_lsus_ext,swap,file);
  freadreal1(&grassland->nr_of_lsus_int,swap,file);
  freadint1(&grassland->rotation.grazing_days,swap,file);
  freadint1(&grassland->rotation.recovery_days,swap,file);
  freadint1(&grassland->rotation.paddocks,swap,file);
  return freadint1((int *)(&grassland->rotation.mode),swap,file)!=1;
} /* of 'fread_grassland' */
