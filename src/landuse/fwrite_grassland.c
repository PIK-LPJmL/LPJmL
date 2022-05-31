/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  _  g  r  a  s  s  l  a  n  d  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes grassland data of stand                                    \n**/
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

Bool fwrite_grassland(FILE *file,        /**< pointer to binary file */
                      const Stand *stand /**< stand pointer */
                     )                   /** \return TRUE on error */
{
  const Grassland *grassland;
  grassland=stand->data;
  fwrite_irrigation(file,&grassland->irrigation);
  fwrite(&stand->growing_days,sizeof(int),1,file);
  fwrite(&grassland->nr_of_lsus_ext,sizeof(Real),1,file);
  fwrite(&grassland->nr_of_lsus_int,sizeof(Real),1,file);
  return (fwrite(&grassland->rotation,sizeof(Rotation),1,file)!=1);
} /* of 'fwrite_grassland' */
