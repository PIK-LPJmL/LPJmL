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

static Bool writerotation(Bstruct file,const char *name,const Rotation *rotation)
{
  bstruct_writebeginstruct(file,name);
  bstruct_writeint(file,"grazing_days",rotation->grazing_days);
  bstruct_writeint(file,"recovery_dates",rotation->recovery_days);
  bstruct_writeint(file,"paddocks",rotation->paddocks);
  bstruct_writeint(file,"mode",rotation->mode);
  return bstruct_writeendstruct(file);
} /* of 'writerotation' */

Bool fwrite_grassland(Bstruct file,      /**< pointer to restart file */
                      const Stand *stand /**< stand pointer */
                     )                   /** \return TRUE on error */
{
  const Grassland *grassland;
  grassland=stand->data;
  fwrite_irrigation(file,"irrigation",&grassland->irrigation);
  bstruct_writeint(file,"growing_days",stand->growing_days);
  bstruct_writereal(file,"deficit_lsu_ne",grassland->deficit_lsu_ne);
  bstruct_writereal(file,"deficit_lsu_mp",grassland->deficit_lsu_mp);
  return writerotation(file,"rotation",&grassland->rotation);
} /* of 'fwrite_grassland' */
